/*! \file Thinner.h
    \brief Contains the definition for the thinner algrothm class.
    \author Chris Bilderback

    The thinner class performs thining on bilevel images.
*/
#ifndef THINNER_H
#define THINNER_H

#include <stdexcept>


//! Line thinning class.
/*!
  Thinning class that perfoms thining on bilevel images.
  The default values for the bilevel is 0 and 255 
  (where 0 is the background and
  255 is the forground).  These values may be changed by the setMinMax()
  function.  The class may operate on imagelib entities (using the 
  define NEED_USGSIMAGELIB) or just on in-memory buffers.
*/
class Thinner
{
public:
  //! Main constructor for the class
  /*! All class attributes are intialized.
   */
  Thinner();

  //! Main destructor for the class, deallocates any class stored memory.
  /*! 
   */
  ~Thinner();
  
  //! Allows the min and max to be set.
  /*!
    \param inMin the new mininum value
    \param inMax the new maximum value
   */
  void setMinMax(int inMin, int inMax) throw();
 
  //! Allows pruning to be used by specifying the number of iterations.
  /*!
    \param inIter the number of interations to use.
  */
  void setPrune(int inIter) throw();
  
  //! Sets 8 connected structure instead of 4 connected (default)
  /*!
   */
  void setEightConnected() throw();
  
  //! Sets 4 connected structure (default).
  /*!
   */
  void setFourConnected() throw();

  //! Performs thinning on a buffer in memory
  /*!
    \param outImg the output buffer.
    \param width  the width of the input buffer.
    \param height the height of the input buffer.
    \exception std::runtime_error unable to process the images.
  */
  void thin(unsigned char * outImg,
            long int width, long int height) 
    throw(std::runtime_error);

protected: 
  //! Matches a structural element to pixel
  /*!
    \param inImg the buffer to work on (8 pixels surrounding the pixel).
    \param strucpointer
    \return true if struct matches the (and the pixel should be deleted).
    \exception std::runtime_error unable to compare images
  */
  bool match(const unsigned char * inImg,  int * strucpointer) 
    throw(std::runtime_error);

  //! Simple thing for rotating matrices 90 degress
  /*!
    \param inMat is the matrix to be rotated.
    \param storMat is where to store the result
  */
  void rot(const int * inMat, int * storMat) throw();
  


  int imin, imax;
  int pruneit;                               //the number of pruning iterations
  int edgeStruct[9],    cornerStruct[9],     //have all rotation in these
      edge90Struct[9],  corner90Struct[9],   //seems like cheating though...
      edge180Struct[9], corner180Struct[9],
      edge270Struct[9], corner270Struct[9],
      connect[9],
      prunel[9], pruner[9],
      prunel90[9], pruner90[9],
      prunel180[9], pruner180[9],
      prunel270[9], pruner270[9];
}; 

#endif 
