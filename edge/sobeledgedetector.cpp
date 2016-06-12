/***************************************************************************
                          sobeledgedetector.cpp  -  description
                             -------------------
    begin                : Tue Oct 30 2001
    copyright            : (C) 2001 by Chris Bilderback
    email                : cbilder@umre.du
 ***************************************************************************/


#include "sobeledgedetector.h"
#include <math.h>

//**************************************************************************
SobelEdgeDetector::SobelEdgeDetector(int inpercentage) :
percentage(inpercentage)
{}

//***************************************************************************
void SobelEdgeDetector::edgeDetect(USGSImageLib::ImageIFile * inimg,
            USGSImageLib::ImageOFile *outimg) throw(std::runtime_error)
{
  long int wcounter(0), lcounter(0), jcounter(0), kcounter(0);
  USGSImageLib::GreyPixel * greybuff(0);     //the input grey buffer.
  USGSImageLib::GreyPixel * outbuff(0);      //the output image
  int outphoto(0);
  long int width(0), height(0);     //image metrics
  long int gx(0), gy(0);                     //gradients
  long int xconv[9] = {-1, 0, 1,
                       -2, 0, 2,
                       -1, 0, 1};
  long int yconv[9] = {1, 2, 1,
                       0, 0, 0,
                       -1, -2, -1};
  unsigned short tempgrey(0);
  //do some checks
  if (!inimg || !outimg)
  {
    throw std::runtime_error("Sobel - edgeDetect files invalid");
  }

  //also check the photometric
  outimg->getPhotometric(outphoto);

  if (outphoto != PHOTO_MINISWHITE)
  {
    throw std::runtime_error("Sobel - Output is unsupported image type!!");
  }

  //get the width and height
  inimg->getWidth(width);
  inimg->getHeight(height);

  //This is noneffienct and I will fix it later
  if (!(greybuff = new (std::nothrow) USGSImageLib::GreyPixel[width*height]))
    throw std::runtime_error("Sobel - Failed to create input buffer");

  //this will need  to change if the is a really big images
  if (!(outbuff = new (std::nothrow) USGSImageLib::GreyPixel[width*height]))
    throw std::runtime_error("Sobel - Failed to create output image");

  //read in the input image (again not good)
  for(lcounter = 0; lcounter < height; ++lcounter)
  {
    inimg->getScanline(lcounter, (greybuff + lcounter*width));
  }

  //perform the convlusion
  for(lcounter = 0; lcounter < height; ++lcounter)
  {
    for(wcounter = 0; wcounter < width; ++wcounter)
    {
       gx = gy = 0;
       for(jcounter = lcounter-1; jcounter <= lcounter+1; ++jcounter)
       {
         for(kcounter = wcounter -1; kcounter <= wcounter+1; ++kcounter)
         {
           if (!((jcounter < 0) || (jcounter >= height) || (kcounter < 0) ||
                (kcounter >= width)))
           {
             greybuff[kcounter+jcounter*width].getGrey(tempgrey);
             gx += xconv[(kcounter - (wcounter-1)) +
                      3*(jcounter-(lcounter-1))]*tempgrey;
             gy += yconv[(kcounter - (wcounter-1)) +
                      3*(jcounter-(lcounter-1))]*tempgrey;
           }
           else
           {
             greybuff[lcounter*width+wcounter].getGrey(tempgrey);
             gx += xconv[(kcounter - (wcounter-1)) +
                         3*(jcounter-(lcounter-1))]*tempgrey;
             gy += yconv[(kcounter - (wcounter-1)) +
                           3*(jcounter-(lcounter-1))]*tempgrey;
           }
             
         }
       }
       outbuff[wcounter + lcounter*width].setGrey(
               static_cast<short unsigned>(sqrt(gx*gx + gy*gy)));
    }
  }

  //loop through and find max gradient
  gx = 0;
  for(lcounter = 0; lcounter < width*height; ++lcounter)
  {
    outbuff[lcounter].getGrey(tempgrey);
    if (tempgrey > gx)
      gx = tempgrey;
  }

  //now apply edge suppression
  for(lcounter = 0; lcounter < width*height; ++lcounter)
  {
    outbuff[lcounter].getGrey(tempgrey);

    if(static_cast<float>(tempgrey) <
             static_cast<float>(gx)*percentage/100.0f)
    {
      outbuff[lcounter].setGrey(0);
    }
    else
    {
      outbuff[lcounter].setGrey(255);
    }
  }

  //output the image
  for (lcounter = 0; lcounter < height; ++lcounter)
  {
    outimg->putScanline(lcounter, outbuff + width*lcounter);
  }

  //delete the buffers
  delete [] greybuff;
  delete [] outbuff;


}
