//laterinhibedgedetector.cpp - yet another edge detecting experiment
#include "lateralinhibedgedetector.h"

//*******************************************************************
LateralInhibEdgeDetector::
LateralInhibEdgeDetector(float insigma, int inlowval, int inhighval)
  : CannyEdgeDetector(insigma, inlowval, inhighval)
{}

//******************************************************************
void LateralInhibEdgeDetector::
edgeDetect(USGSImageLib::ImageIFile * inimg,
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

  llatinhib(greybuff, outbuff, width, height); //do the inhibited thing

  smooth(outbuff, greybuff,width, height, gauss); //smooth original image

  //get the gradient
   maxgrad = ders(greybuff, outbuff, directions, width, height);
  //ders are now stored in outbuff...

  //compute upper and lowers
  highval = static_cast<int>(static_cast<float>(highvalper)*maxgrad/100.0);
  lowval =  static_cast<int>(static_cast<float>(lowvalper)*maxgrad/100.0);

  //suppress non-maximal local edge points
  nonmax(greybuff,  directions, width, height);
 
  //do the edge tracking
  htracker(outbuff, greybuff, width, height);

  //output the image
  for (lcounter = 0; lcounter < height; ++lcounter)
  {
    outimg->putScanline(lcounter, greybuff + width*lcounter);
  }

  //delete the buffers
  delete [] greybuff;
  delete [] outbuff;

}

//*******************************************************************
void LateralInhibEdgeDetector::llatinhib(USGSImageLib::GreyPixel * input, 
                                         USGSImageLib::GreyPixel * output,
                                         long int width, long int height) 
  throw()
{
  
  long int xcounter(0), ycounter(0), kcounter(0);
  long int tempindex(0);
  double result(0);
  double inconv[9] = {-1, -1, -1,
                      -1, 8, -1,
                      -1, -1, -1}; //should produce zeros in uniform field
  long int xplace[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
  long int yplace[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
  long int size(width*height);
  unsigned short tempgrey(0);


  for (ycounter = 0; ycounter < height; ++ycounter)
  {
    for(xcounter = 0; xcounter < width; ++xcounter)
    {
      result = 0;
      
      for(kcounter = 0; kcounter < 9; ++kcounter)
      {
        //get the index
        tempindex = xcounter + xplace[kcounter] + 
          (ycounter + yplace[kcounter])*width;
        
        //check to see that it a valid pixel
        if ((tempindex >= 0) && (tempindex < size))
        {
          //get the point
          input[tempindex].getGrey(tempgrey);
        }
        else
        {
          input[xcounter+ycounter*width].getGrey(tempgrey);
         
        }
        result += inconv[kcounter]*static_cast<double>(tempgrey);
      }

      //assign the point
      if (result < 0)
      {
        //map it to zero
        result = 0;
      }
      output[xcounter + ycounter*width].setGrey
        (static_cast<unsigned short>(result));
    }
  }

  //done
}
        
      

  
