/***************************************************************************
                          edgedetector.h  -  description
                             -------------------
    begin                : Tue Oct 30 2001
    copyright            : (C) 2001 by Chris Bilderback
    email                : cbilder@umre.du
 ***************************************************************************/

 ***************************************************************************/
#ifndef EDGEDETECTOR_H
#define EDGEDETECTOR_H


#include <stdexcept>
#include "ImageLib/ImageIFile.h"
#include "ImageLib/ImageOFile.h"


/**
 *Abstract base class for edge detection methods
 *@author Chris Bilderback
 **/
class EdgeDetector
{
public:

  //for large images....
  virtual void edgeDetect(USGSImageLib::ImageIFile * inimg,
            USGSImageLib::ImageOFile *outimg) throw(std::runtime_error) = 0;
  //in-memory version...
  //Output data image is assumed to be the same bps and spp and width
  // and height of the input image.
  /*virtual void edgeDetect(void * inData, int inbps, int inspp,
                      void * outData, int outbps, int outspp )
                     throw(std::runtime_error)=0;
   */
};

#endif
