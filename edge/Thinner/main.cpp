#include <iostream>
#include "Thinner.h"
#include "ImageLib/TIFFImageIFile.h"
#include "ImageLib/TIFFImageOFile.h"


int main(int argc, char ** argv)
{
  Thinner thinalg; //the thinner
  USGSImageLib::TIFFImageIFile * inimg = 0;
  USGSImageLib::TIFFImageOFile * outimg = 0;
  long int width(0), height(0);
  std::string infile("in2.tif");
  std::string outfile("out.tif");
  unsigned char * buffer = 0;
  unsigned char * temp = 0;
  long int counter(0);
  thinalg.setMinMax(0, 255);
  thinalg.setEightConnected();
  //thinalg.setPrune(1);
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

    //create the buffer
    if(!(buffer = new unsigned char [width*height]))
      throw std::bad_alloc();

    //read in the buffer
    for(counter = 0; counter < height; ++counter)
    {
      temp = buffer+ counter*width; //move to the line
      inimg->getRawScanline(counter, temp);
    }

    //perfrom thining
    thinalg.thin(buffer, width, height);

    //output the image
    for(counter = 0; counter < height; ++counter)
    {
      temp = buffer + counter*width; 
      outimg->putRawScanline(counter, temp);
    }

    delete inimg;
    delete outimg;
    return 0;

  }
  catch(...)
  {
    std::cout << "You are the weakest link. Good-bye" << std::endl;
    delete inimg;
    delete outimg;
    return 0;
  }
}
