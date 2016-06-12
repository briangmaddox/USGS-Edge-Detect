#ifndef LATERALINHIBEDGEDETECTOR_H
#define LATERALINHIBEDGEDETECTOR_H


#include "cannyedgedetector.h"
#include <math.h>


//canny with edge enhancement prestep.
class LateralInhibEdgeDetector : public CannyEdgeDetector
{
public:
  LateralInhibEdgeDetector(float insigma, int inlowval, int inhighval);
  
  virtual void edgeDetect(USGSImageLib::ImageIFile * inimg,
             USGSImageLib::ImageOFile *outimg) throw(std::runtime_error);

protected:
  
  //attemps to highlight egde pixel by using the principal of mach bands
  void llatinhib(USGSImageLib::GreyPixel * input, 
                 USGSImageLib::GreyPixel * output,
                 long int width, long int height) throw();
};

#endif
