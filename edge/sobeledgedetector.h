/***************************************************************************
                          sobeledgedetector.h  -  description
                             -------------------
    begin                : Tue Oct 30 2001
    copyright            : (C) 2001 by Chris Bilderback
    email                : cbilder@umre.du
 ***************************************************************************/


#ifndef SOBELEDGEDETECTOR_H
#define SOBELEDGEDETECTOR_H

#include "edgedetector.h"

/**
 *
 *@author Chris Bilderback
 **/
class SobelEdgeDetector : public EdgeDetector
{
public:
  //constructor takes the percentage of the highest magnitude gradient to
  //keep.
  SobelEdgeDetector(int inpercentage);

  //edge detector interface
  virtual void edgeDetect(USGSImageLib::ImageIFile * inimg,
            USGSImageLib::ImageOFile *outimg) throw(std::runtime_error);

protected:

  int percentage;  //the percentage of the largest gradient to keep.

};

#endif
