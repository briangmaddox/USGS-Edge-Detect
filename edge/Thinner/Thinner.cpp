/*! \file Thinner.cpp
    \brief Contains the definition for the thinner algrothm class.
    \author Chris Bilderback

    The thinner class performs thining on bilevel images.
*/

#include "Thinner.h"

//***********************************************************
Thinner::Thinner() : imin(0), imax(255),  pruneit(0)
                                         
{
  
  edgeStruct[0] = 0; edgeStruct[1] = 0; edgeStruct[2] = 0;
  edgeStruct[3] = 2; edgeStruct[4] = 1; edgeStruct[5] = 2;
  edgeStruct[6] = 1; edgeStruct[7] = 1; edgeStruct[8] = 1;
  
  cornerStruct[0] = 2; cornerStruct[1] = 0; cornerStruct[2] = 0;
  cornerStruct[3] = 1; cornerStruct[4] = 1; cornerStruct[5] = 0;
  cornerStruct[6] = 2; cornerStruct[7] = 1; cornerStruct[8] = 2;

  //get the 90's
  rot(edgeStruct, edge90Struct);
  rot(cornerStruct, corner90Struct);
  
  //get the 180's
  rot(edge90Struct, edge180Struct);
  rot(corner90Struct, corner180Struct);
  
  //get the 270's
  rot(edge180Struct, edge270Struct);
  rot(corner180Struct, corner270Struct);

  //set the connection
  setFourConnected();
  
  prunel[0] = 0; prunel[1] = 0; prunel[2] = 0;
  prunel[3] = 0; prunel[4] = 1; prunel[5] = 0;
  prunel[6] = 0; prunel[7] = 2; prunel[8] = 2;
  
  pruner[0] = 0; pruner[1] = 0; pruner[2] = 0;
  pruner[3] = 0; pruner[4] = 1; pruner[5] = 0;
  pruner[6] = 2; pruner[7] = 2; pruner[8] = 0;

  //get the 90's
  rot(prunel, prunel90);
  rot(pruner, pruner90);
  
  //get the 180's
  rot(prunel90, prunel180);
  rot(pruner90, pruner180);
  
  //get the 270's
  rot(prunel180, prunel270);
  rot(pruner180, pruner270);
  
}
  
//************************************************************
Thinner::~Thinner()
{}

//************************************************************
void Thinner::setMinMax(int inMin, int inMax) throw()
{
  imin = inMin;
  imax = inMax;
}
  

//*****************************************************************
void Thinner::thin(unsigned char * outImg,
          long int width, long int height) 
  throw(std::runtime_error)
{
  unsigned char buffer[9] = {0}; //buffer for struct
  bool nchanged(true);
  int *cycle[9] = {edgeStruct, cornerStruct, 
                   edge90Struct, corner90Struct,
                   edge180Struct, corner180Struct,
                   edge270Struct, corner270Struct,
                   0};// connect};
  int cyclecounter(0);
  
  int* prunecycle[8] = {prunel, pruner,
                       prunel90, pruner90,
                       prunel180, pruner180,
                       prunel270, pruner270};
  int prunecounter(0);
  int pcyclecounter(0);
  long int ycounter(0), xcounter(0), wcounter(0);
  int xcorr[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1}; 
  int ycorr[9] = {-1, -1, -1,   0, 0, 0,   1, 1, 1};
  long int x, y;
  

  
  //do it until it does not change
  while(nchanged)
  {
    nchanged = false; //set the flage
    
    for(cyclecounter =0; cyclecounter < 9; ++cyclecounter)
    {

      for(ycounter = 0; ycounter < height; ++ycounter)
      {
        for(xcounter = 0; xcounter < width; ++xcounter)
        {
          //get the block
          for(wcounter = 0; wcounter < 9; ++wcounter)
          {
            x = xcounter + xcorr[wcounter];
            y = ycounter + ycorr[wcounter];
            
            if((x < 0) || (x >= width) || (y < 0) || (y >= height))
            {
              //just use the same point?
              buffer[wcounter] = outImg[ycounter*width + xcounter];
            }
            else
            {
              buffer[wcounter] = outImg[x + y*width];
            }
            
          }
          
          //have the block
          if(cycle[cyclecounter])
          {
            if(match(buffer, cycle[cyclecounter]))
            {
              outImg[ycounter*width + xcounter] = 
                static_cast<unsigned char>(imin);
              nchanged = true; //we have changed
            }
          }
          
        } //xcounter
      }   //ycounter
     
      //if we are pruning
      for(prunecounter = 0; prunecounter < pruneit; ++prunecounter)
      {
        for(pcyclecounter = 0; pcyclecounter < 8; ++pcyclecounter)
        {
          for(ycounter = 0; ycounter < height; ++ycounter)
          {
            for(xcounter = 0; xcounter < width; ++xcounter)
            {
            
              //get the block
              for(wcounter = 0; wcounter < 9; ++wcounter)
              {
                x = xcounter + xcorr[wcounter];
                y = ycounter + ycorr[wcounter];
                
                if((x < 0) || (x >= width) || (y < 0) || (y >= height))
                {
                  //just use the same point?
                  buffer[wcounter] = outImg[ycounter*width + xcounter];
                }
                else
                {
                  buffer[wcounter] = outImg[x + y*width];
                }
                
              }//wcounter
          
              //have the block
              if(match(buffer, prunecycle[pcyclecounter]))
              {
                outImg[ycounter*width + xcounter] = 
                  static_cast<unsigned char>(imin);
              }
          
            } //xcounter
          }   //ycounter
      
        } //pcycle

      } //prune counter
 
     
    }//cycle counter

  }
           
}

//********************************************************************
bool Thinner::match(const unsigned char * inImg, 
                   int * strucpointer) throw(std::runtime_error)
{
  int counter(0);
  bool matches = true;

  //go through and check the struct 
  for(counter = 0; counter < 9; ++counter)
  {
    switch(strucpointer[counter])
    {
    case 0:
      if(static_cast<int>(inImg[counter]) != imin)
        {
          matches = false;
          return matches;
        }
      break;
    case 1:
      if(static_cast<int>(inImg[counter]) != imax)
      {
          matches = false;
          return matches;
      }
      break;
    case 2:
      break;
      //ignore anything else
    }
  }

  return matches;
}

//*****************************************************
void Thinner::rot(const int * inMat, int * storMat) throw()
{
  int counter, xcoord(0), ycoord(0), newspot(0);

  //get copies of the 90 edge struct
  for(counter = 0; counter < 9; ++counter)
  {
    ycoord = (counter)/3 -1;
    xcoord = counter%3 -1;
    
    newspot = -ycoord  + xcoord*3 +4;
    
    storMat[newspot] = inMat[counter];
  }

}

//****************************************************************
void Thinner::setEightConnected() throw()
{
  connect[0] = 2; connect[1] = 1; connect[2] = 2;
  connect[3] = 1; connect[4] = 1; connect[5] = 1;
  connect[6] = 2; connect[7] = 1; connect[8] = 2;

}  

//******************************************************************
void Thinner::setFourConnected() throw()
{
  int counter;
  
  for(counter = 0; counter < 9; ++counter)
  {
    connect[counter] = 1;
  }
}

//*************************************************************
void Thinner::setPrune(int inIter) throw()
{
  pruneit = inIter;
}
