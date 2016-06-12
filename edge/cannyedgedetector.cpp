/***************************************************************************
                          cannyedgedetector.cpp  -  description
                             -------------------
    begin                : Thu Nov 1 2001
    copyright            : (C) 2001 by Chris Bilderback
    email                : cbilder@umre.du
 ***************************************************************************/


#include "cannyedgedetector.h"
#include <math.h>
#include <stack>


//**********************************************************************
CannyEdgeDetector::
CannyEdgeDetector(float insigma, int inlowval, int inhighval)
: sigma(insigma), lowvalper(inlowval), highvalper(inhighval),
  lowval(0), highval(0)
{}

//**************************************************************************
void CannyEdgeDetector::edgeDetect(USGSImageLib::ImageIFile * inimg,
            USGSImageLib::ImageOFile *outimg) throw(std::runtime_error)
{
  long int lcounter(0);
  USGSImageLib::GreyPixel * greybuff(0);     //the input grey buffer.
  USGSImageLib::GreyPixel * outbuff(0);      //the output image
  int outphoto(0);
  long int width(0), height(0);           //image metrics
  std::vector<float> gauss;               //for the smoothing
  unsigned int gausssize(0);              //try to make this always odd
  float maxgrad(0);
  std::vector<MathLib::Point> directions; //gradient directions


  if (!inimg || !outimg)
  {
    throw std::runtime_error("Canny - edgeDetect files invalid");
  }

  //also check the photometric
  outimg->getPhotometric(outphoto);

  if (outphoto != PHOTO_MINISWHITE)
  {
    throw std::runtime_error("Canny - Output is unsupported image type!!");
  }

  //resize the gaussian to 3*sigma
  gausssize = static_cast<unsigned int>(ceil(3*sigma));
  if (gausssize%2 == 0) //make sure it is odd
    gausssize++;

  gauss.resize(gausssize);

  //calculate the gaussian.
  computeGaussian(gauss);

  //get the width and height
  inimg->getWidth(width);
  inimg->getHeight(height);

  //This is noneffienct and I will fix it later
  if (!(greybuff = new (std::nothrow) USGSImageLib::GreyPixel[width*height]))
    throw std::runtime_error("Canny - Failed to create input buffer");

  //this will need  to change if the is a really big images
  if (!(outbuff = new (std::nothrow) USGSImageLib::GreyPixel[width*height]))
    throw std::runtime_error("Canny - Failed to create output image");

  //resize the direction vector
  directions.resize(width*height);

  //read in the entire input image (again not good)
  for(lcounter = 0; lcounter < height; ++lcounter)
  {
    inimg->getScanline(lcounter, (greybuff + lcounter*width));
  }

  smooth(greybuff, outbuff, width, height, gauss); //smooth original image

  //get the gradient
  maxgrad = ders(outbuff, greybuff, directions, width, height);
  //ders are now stored in greybuff...

  //compute upper and lowers
  highval = static_cast<int>(static_cast<float>(highvalper)*maxgrad/100.0);
  lowval =  static_cast<int>(static_cast<float>(lowvalper)*maxgrad/100.0);

  //suppress non-maximal local edge points
  nonmax(outbuff,  directions, width, height);
 
  //do the edge tracking
  htracker(greybuff, outbuff, width, height);

  //output the image
  for (lcounter = 0; lcounter < height; ++lcounter)
  {
    outimg->putScanline(lcounter, outbuff + width*lcounter);
  }

  //delete the buffers
  delete [] greybuff;
  delete [] outbuff;


}


/** Computes the gaussian for the input vector. */
void CannyEdgeDetector::computeGaussian(std::vector<float> & gauss) throw()
{
  int xcounter(0);
  int midpoint(static_cast<int>(gauss.size()*0.5));
  float frontval(1.0/(sqrt(2*3.14159)*sigma));

  //loop through the array and calculate the gaussian
  for(xcounter = 0; static_cast<unsigned int>(xcounter) 
        < gauss.size(); ++xcounter)
  {
    //here is what we want
    // 1/sqrt(2PI)sigma  * e^(-x^2/2sigma)
    gauss[xcounter] =frontval*
             exp(-(xcounter-midpoint)*(xcounter-midpoint)/(2*sigma));
  }

  //that should do it

}

/** Smooths the input image and stores it in output. */
void CannyEdgeDetector::smooth(USGSImageLib::GreyPixel* input,
        USGSImageLib::GreyPixel * output, long int width, long int height,
        std::vector<float> gauss) throw()
{
  long int xcounter(0), ycounter(0), gcounter(0);       //counters
  long int mid(static_cast<long int>(gauss.size()*0.5)), tempindex(0);
  float xtemp(0.0f), ytemp(0.0f);
  unsigned short tempgrey(0);

  for(ycounter = 0; ycounter < height; ++ycounter)
    for(xcounter = 0; xcounter < width; ++xcounter)
    {
      //do xdirection first
      xtemp = ytemp = 0.0f;
      for(gcounter = 0; static_cast<unsigned int>(gcounter) 
            < gauss.size(); gcounter++)
      {
        tempindex = xcounter + (gcounter - mid);
        //check the index
        if (!(tempindex < 0 || tempindex >= width))
        {
          input[tempindex + width*ycounter].getGrey(tempgrey);
          xtemp += gauss[gcounter]*tempgrey;
        }

        //do the other direction
        tempindex = ycounter + (gcounter - mid);

        if(!(tempindex < 0 || tempindex >= height))
        {
          input[tempindex*width + xcounter].getGrey(tempgrey);
          ytemp += gauss[gcounter]*tempgrey;
        }
      }
      tempgrey = static_cast<unsigned short>(xtemp + ytemp);
      output[xcounter + ycounter*width].setGrey(tempgrey);
    }

  //done smoothing

}

/** Computes the gradient map of the input image
    returns the maximum gradient
    and return a map of gradient directions dy and dx
  */
float CannyEdgeDetector::ders(USGSImageLib::GreyPixel * input,
            USGSImageLib::GreyPixel * output,
             std::vector<MathLib::Point> & outders,
            long int width, long int height) throw()
{
  long int xcounter(0), ycounter(0), gcounter(0);       //counters
  long int tempindex(0);
  float xtemp(0.0f), ytemp(0.0f);
  unsigned short tempgrey(0);
  float grdmax(0), grdtemp(0);             //maximum gradient
  float derv[3] = {-1.0f, 0.0f, 1.0f};     //don't know how good this is
                                           //it was used in a master's thesis?
  for(ycounter = 0; ycounter < height; ++ycounter)
    for(xcounter = 0; xcounter < width; ++xcounter)
    {
      //do xdirection first
      xtemp = ytemp = 0.0f;
      for(gcounter = 0; gcounter < 3; gcounter++)
      {
        tempindex = xcounter + (gcounter - 1);
        //check the index
        if (!(tempindex < 0 || tempindex >= width))
        {
          input[tempindex + width*ycounter].getGrey(tempgrey);
          xtemp += derv[gcounter]*tempgrey;
        }
        else
        {
          input[xcounter + ycounter*width].getGrey(tempgrey);
          xtemp += derv[gcounter]*tempgrey;
        }

        //do the other direction
        tempindex = ycounter + (gcounter - 1);
        //check the index
        if(!(tempindex < 0 || tempindex >= height))
        {
          input[tempindex*width + xcounter].getGrey(tempgrey);
          ytemp += derv[gcounter]*tempgrey;
        }
        else
        {
          input[xcounter + ycounter*width].getGrey(tempgrey);
          ytemp+= derv[gcounter]*tempgrey;
        }

      }
      //store the gradient 
      outders[xcounter + width*ycounter].x = xtemp;
      outders[xcounter + width*ycounter].y = ytemp;

      //store the gradient estimate in the output buffer
      grdtemp = sqrt(xtemp*xtemp + ytemp*ytemp);
      if(grdmax < grdtemp)
        grdmax = grdtemp;
      tempgrey = static_cast<unsigned short>(grdtemp);
      output[xcounter + ycounter*width].setGrey(tempgrey);
    }

  //give back maximum gradient
  return grdmax;

}

/** Performs the edge tracking */
void CannyEdgeDetector::htracker(USGSImageLib::GreyPixel* input,
   USGSImageLib::GreyPixel * output, long int width, long int height) throw()
{
  std::stack<MathLib::Point> pointstack; //for moving around the image
  MathLib::Point tempoint;               //temporary point
  long int xsquence[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
  long int ysquence[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  unsigned short tempgrey(0), tempgrey1(0);
  long int wcounter(0), lcounter(0); //counter for loops
  long int tempx(0), tempy(0);

  //loop through and track edges.
  for(lcounter = 0; lcounter < height; ++lcounter)
  {
    for(wcounter = 0; wcounter < width; ++wcounter)
    {
      input[wcounter + lcounter*width].getGrey(tempgrey);
      output[wcounter+lcounter*width].getGrey(tempgrey1);

      if (tempgrey >= highval && tempgrey1 == CANNY_POSSIBLEEDGE)
      {
        //set it to a edge and follow it
        output[wcounter + lcounter*width].setGrey(CANNY_EDGE);
        //put this point on the stack
        tempoint.x = wcounter;
        tempoint.y = lcounter;
        tempoint.z = 0;
        //put it on the stack
        pointstack.push(tempoint);

        //loop
        while(pointstack.size() != 0)
        {
          //take the point off the stack
          tempoint = pointstack.top();
          pointstack.pop();

          //loop through possiblities
          for(; tempoint.z < 8; ++tempoint.z)
          {
            tempx = static_cast<long int>(tempoint.x + 
                    xsquence[static_cast<int>(tempoint.z)]);
            tempy = static_cast<long int>(tempoint.y + 
              ysquence[static_cast<int>(tempoint.z)]);

            //check bounds
            if ((tempx >= 0 && tempx < width) && 
                (tempy >= 0 && tempy < height))
            {
              //get the grey values
              input[tempx + tempy*width].getGrey(tempgrey);
              output[tempx + tempy*width].getGrey(tempgrey1);
              
               //check to see if we should follow
              if(tempgrey >= lowval && tempgrey1 == CANNY_POSSIBLEEDGE)
              {
                output[tempx + tempy*width].setGrey(CANNY_EDGE);
                //push this point back on the stack
                pointstack.push(tempoint);
                //rest the tempoint
                tempoint.x = tempx;
                tempoint.y = tempy;
                tempoint.z = 0;    //set the counter to zero.
                pointstack.push(tempoint);
                break;  //break out of for loop

              }
              else
              {
                if (tempgrey < lowval)
                {
                  //never will be a edge
                  output[tempx + tempy*width].setGrey(CANNY_NOEDGE);
                }
              }
            }
          }//for

        }//while

        

      }//start edge check

    }//width counter
  }//height counter

  //loop through output and suppress all non edges.
  for(wcounter = 0; wcounter < width*height; ++wcounter)
  {
    output[wcounter].getGrey(tempgrey);
    if(tempgrey == CANNY_POSSIBLEEDGE)
    {
      output[wcounter].setGrey(CANNY_NOEDGE);
    }
  }

  //done
}

 /** performs non-maximal suppression on the gradient mag based off
      gradient direction. */
void CannyEdgeDetector::nonmax(USGSImageLib::GreyPixel * output,
            const std::vector<MathLib::Point> & inders,
            long int width, long int height) throw()
{
  long int xcounter(0), ycounter(0);               //counters for image manip
  long int tempindex1(0), tempindex2(0);           //other points on edge
  float tempx(0), tempy(0);                        //for deteriming indices
  float mag2(0), mag3(0);                          //the magintudes for
  long int x(0), y(0);                             //looking at supression
  float tempmag(0);
  MathLib::Point grad1, grad2;                     //gradient points 

  //loop through all the points
  for(ycounter = 0; ycounter < height; ++ycounter)
  {
    for(xcounter = 0; xcounter < width; ++xcounter)
    {
      
      //look at the directions and determine the directions
      //this seems right (but no one has said this is the way to do it)
      //assuming this derivate approx is change in y and change in x
      tempx = inders[xcounter + ycounter*width].x;
      tempy = inders[xcounter + ycounter*width].y;

      //get the magintude and make a (rounded) unit vector.
      tempmag = sqrt(tempx*tempx + tempy*tempy);
      //check for zero mags (or less than the highvalue?)
      if(tempmag < highval || tempmag == 0)
      {
        //net change is zero?
        //suppress the edge
        output[xcounter + ycounter*width] = CANNY_NOEDGE;
      }
      else
      {
        //(it had at least one net change in a direction
        //and the grad is greater or equal to the high value
        //get the rounded up (unit vector) vector
        x = static_cast<long int>(static_cast<float>(tempx)/tempmag +
                       0.5f);
        y = static_cast<long int>(static_cast<float>(tempy)/tempmag +
                       0.5f);
        tempindex1 = xcounter - x + (ycounter - y)*width;
        tempindex2 = xcounter + x + (ycounter + y)*width;

        //get the magnitudes and do a cheesy second derivative 
        //estimate.
        if(tempindex1 > 0 && tempindex1 <= width*height)
        {
          grad1.x = inders[tempindex1].x;
          grad1.y = inders[tempindex1].y;

        }
        else
        {
          grad1.x = tempx;
          grad1.y = tempy;
        }

        //get the other magnitudes
        if(tempindex2 > 0 && tempindex2 <= width*height)
        {
          grad2.x = inders[tempindex1].x;
          grad2.y = inders[tempindex1].y;
        }
        else
        {
          grad1.x = tempx;
          grad1.y = tempy;
        }
        
        //get the component of the gradients that is in this 
        //gradient direction
        mag2 = (grad1.x*tempx + grad1.y*tempy)/tempmag;
        mag3 = (grad2.x*tempx + grad2.y*tempy)/tempmag;

        //check to see if this is a maximal point on the edge
        if((tempmag > mag2) && (tempmag > mag3 ))
        {
          //check to see if this greater than the low value
          if(static_cast<short unsigned int>(tempmag) < lowval)
            output[xcounter + ycounter*width].setGrey(CANNY_NOEDGE);
          else
            output[xcounter + ycounter*width].setGrey(CANNY_POSSIBLEEDGE);
        }
        else
        {
          //supress the point
          output[xcounter + ycounter*width].setGrey(CANNY_NOEDGE);
        }



      }
    }
  }


}
