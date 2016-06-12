/***************************************************************************
                          cannyedgedetector.h  -  description
                             -------------------
    begin                : Thu Nov 1 2001
    copyright            : (C) 2001 by Chris Bilderback
    email                : cbilder@umre.du
 ***************************************************************************/


#ifndef CANNYEDGEDETECTOR_H
#define CANNYEDGEDETECTOR_H

//this defines all possible edge conditions for tracking
#define  CANNY_POSSIBLEEDGE 128
#define  CANNY_NOEDGE       0
#define  CANNY_EDGE         255


#include "edgedetector.h"
#include <vector>
#include "MathLib/Point.h"

/**
  *@author Chris Bilderback
  */

class CannyEdgeDetector : public EdgeDetector
{
public: 
  //main constructor for canny
  //sigma is the stddev for the gaussian smoothing
  //lowval is the is the value to stop a edge and
  //highval is a value to start a edge
  CannyEdgeDetector(float insigma, int inlowval, int inhighval);

  //perform canny edge detection
  virtual void edgeDetect(USGSImageLib::ImageIFile * inimg,
            USGSImageLib::ImageOFile *outimg) throw(std::runtime_error);


protected:
   /** Computes the gaussian for the input vector. */
  void computeGaussian(std::vector<float> & gauss) throw();
  /** Smooths the input image and stores it in output. */
  void smooth(USGSImageLib::GreyPixel* input, USGSImageLib::GreyPixel * output,
        long int width, long int height, std::vector<float> gauss) throw();
  /** Computes the gradient map of the input image
      returns the maximum gradient */
  float ders(USGSImageLib::GreyPixel * input,
            USGSImageLib::GreyPixel * output, 
             std::vector<MathLib::Point> & outders,
            long int width, long int height) throw();
  /** performs the edge tracking herstasis */
  void htracker(USGSImageLib::GreyPixel* input,
         USGSImageLib::GreyPixel * output, long int width, long int height)
         throw();

  /** performs non-maximal suppression on the gradient mag based off
      gradient direction. */
  void nonmax(USGSImageLib::GreyPixel * output,
              const std::vector<MathLib::Point> & inders,
              long int width, long int height) throw();


  float sigma;
  int lowvalper, highvalper, lowval, highval;


};

#endif
