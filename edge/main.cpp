/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Tue Oct 30 12:37:58 EST 2001
    copyright            : (C) 2001 by Chris Bilderback
    email                : cbilder@umre.du
 ***************************************************************************/


#include "sobeledgedetector.h"
#include "lateralinhibedgedetector.h"
#include "ImageLib/TIFFImageIFile.h"
#include "ImageLib/TIFFImageOFile.h"
#include <iostream>

int main(int argc, char *argv[])
{
  USGSImageLib::TIFFImageIFile * inimg(0);
  USGSImageLib::TIFFImageOFile * outimg(0);
  long int width(0), height(0);
  std::string infile("in2.tif");
  std::string outfile("out.tif");
  CannyEdgeDetector edge(3, 5, 7);
  // LateralInhibEdgeDetector edge(3, 15, 20);
  // SobelEdgeDetector edge(15);
  try
  {
    //create the input file
    if(!(inimg = new (std::nothrow) USGSImageLib::TIFFImageIFile(infile)))
      throw std::runtime_error("Could not create input image");


    //get the width and height
    inimg->getWidth(width);
    inimg->getHeight(height);

    //create the output image
    if(!(outimg = new (std::nothrow) USGSImageLib::TIFFImageOFile(outfile,
        width, height, PHOTO_MINISWHITE)))
      throw std::runtime_error("Could not create the output image");

    outimg->setPlanarConfig(1);
    outimg->setSubfileType(0);
    outimg->setCompression(COMPRESSION_NONE);
    outimg->setRowsPerStrip(1);
    outimg->setSamplesPerPixel(1);
    outimg->setBitsPerSample(8);

    edge.edgeDetect(inimg, outimg);

    delete inimg;
    delete outimg;
  }
  catch(std::runtime_error & err)
  {
    std::cout << err.what() << std::endl;
    delete inimg;
    delete outimg;
  }
  catch(...)
  {
    std::cout << "Exception thrown" << std::endl;
    delete inimg;
    delete outimg;
  }

  return 0;
}
