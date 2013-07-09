/**
 * @file   histogram.hpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Fri Jul  5 15:39:46 2013
 * 
 * @brief  This file declare the Hist class
 * 
 * 
 */

#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include "cv.h"
#include "highgui.h"

struct Grid {
  int x;
  int y;
};
class Hist{
 public:
  Hist();
  Hist(Hist& h);
  ~Hist();

  void setImage(IplImage* i);
  void calculate();
  void setGrid(int x, int y);
  CvHistogram* get(int index);
  CvHistogram* operator[](int index);
  Grid getGrid(){return grid;}

  int* getHistSize(){return histSize;};
  float** getHistRange(){return histRange;}
  IplImage* getImage(){return image;}
  CvHistogram** getHist(){return hist;}

  void operator=(Hist&h);
  
 private:
  int histSize[3];
  float** histRange;
  IplImage* image;
  CvHistogram** hist;
  Grid grid;

};



#endif /* _HISTOGRAM_H_ */