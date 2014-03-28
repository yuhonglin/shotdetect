/**
 * @file   histogram.cpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Thu Jul  4 21:45:18 2013
 * 
 * @brief  Implement the histogram features
 *         Every frame is splitted into a few sub graph
 *         For avery
 * 
 * 
 */

#include <iostream>
#include <assert.h>
#include "hist.hpp"

Hist::Hist(){
 
  histSize[0] = 15;
  histSize[1] = 10;
  histSize[2] = 1;


  histRange = new float*[3];
  histRange[0] = new float[2];
  histRange[0][0] = 0.0;
  histRange[0][1] = 181;
  histRange[1] = new float[2];
  histRange[1][0] = 0.0;
  histRange[1][1] = 256;
  histRange[2] = new float[2];
  histRange[2][0] = 0.0;
  histRange[2][1] = 256;

  grid.x = 1;
  grid.y = 1;

  hist = NULL;
  image = NULL;
}


Hist::~Hist(){
  if(hist != NULL) {delete hist; hist = NULL;}
  if(histRange[0] != NULL) { delete histRange[0];}
  if(histRange[1] != NULL) { delete histRange[1];}
  if(histRange[2] != NULL) { delete histRange[2];}
  if(histRange != NULL) { delete histRange; histRange = NULL;}
  if(image != NULL) {cvReleaseImage(&image); image = NULL;}
}

void Hist::setGrid(int x, int y){
  grid.x = x;
  grid.y = y;
  if(hist != NULL) {delete hist; hist = NULL;}
}

void Hist::calculate(){

  assert(image!=NULL);

  if(hist== NULL){
    hist = new CvHistogram*[grid.x*grid.y];
    int length = grid.x*grid.y;
    while(length>0){
      length--;
      hist[length] = cvCreateHist( 3, histSize, CV_HIST_ARRAY, histRange, 1 );
    }
  }


  CvRect originROI, ROI;
  originROI.x = 0;
  originROI.y = 0;
  originROI.width = image->width;
  originROI.height = image->height;
  ROI.x = 0;
  ROI.y = 0;
  ROI.width = image->width/grid.x;
  ROI.height = image->height/grid.y;
 
  CvSize size;
  size.width = ROI.width;
  size.height = ROI.height;
  IplImage* hsvframe = cvCreateImage( size, 8, 3);
  IplImage* h_plane = cvCreateImage( size, 8, 1 );
  IplImage* s_plane = cvCreateImage( size, 8, 1 );
  IplImage* v_plane = cvCreateImage( size, 8, 1 );
  IplImage* planes[] = { h_plane, s_plane, v_plane };

  for (int i = 0; i<grid.y; i++){
    ROI.y=i*ROI.height;
    for(int j = 0; j<grid.x; j++){
      ROI.x = j*ROI.width;
      cvSetImageROI( image, ROI);
      cvCvtColor( image, hsvframe, CV_BGR2HSV);
      cvCvtPixToPlane( hsvframe, h_plane, s_plane, v_plane, 0 );
      cvCalcHist(planes, hist[i*grid.x+j]);
    }
  }

  cvReleaseImage(&hsvframe);
  cvReleaseImage(&h_plane);
  cvReleaseImage(&s_plane);
  cvReleaseImage(&v_plane);
  cvSetImageROI(image, originROI);
}

CvHistogram* Hist::get(int index){
  assert(index<grid.x*grid.y);
  return hist[index];
}

CvHistogram* Hist::operator[](int index){
  return get(index);
}

Hist::Hist(Hist& h){
  grid = h.grid;
    
  histSize[0]=h.getHistSize()[0];
  histSize[1]=h.getHistSize()[1];
  histSize[2]=h.getHistSize()[2];

  histRange[0][0] = h.getHistRange()[0][0];
  histRange[0][1] = h.getHistRange()[0][1];
  histRange[1][0] = h.getHistRange()[1][0];
  histRange[1][1] = h.getHistRange()[1][1];
  histRange[2][0] = h.getHistRange()[2][0];
  histRange[2][1] = h.getHistRange()[2][1];

  if(image == NULL){
    CvSize size;
    size.width = h.getImage()->width;
    size.height = h.getImage()->height;
    image = cvCreateImage( size, 8, 3);
  }
  cvCopy(h.getImage(), image);

  int length = grid.x*grid.y;
  
  if(hist != NULL) delete hist;
  hist = new CvHistogram*[length];
  
  while(length>0){
    length--;
    cvCopyHist(h.getHist()[length], &hist[length]);    
  }
}

void Hist::setImage(IplImage* im){
  if(image != NULL){cvReleaseImage(&image);}

  CvSize size;
  if(im->roi!=NULL){
  size.width = im->roi->width;
  size.height = im->roi->height;
  }else{
    size.width = im->width;
    size.height = im->height;
  }
  image = cvCreateImage( size, 8, 3);
  cvCopy( im, image);
}

void Hist::operator=(Hist& h){


    
  if(hist != NULL){
    int length = grid.x*grid.y;
    while(length--){
      cvReleaseHist(&hist[length]);
    }
    delete hist;
  }

  
  grid = h.grid;
    
  histSize[0]=h.getHistSize()[0];
  histSize[1]=h.getHistSize()[1];
  histSize[2]=h.getHistSize()[2];

  histRange[0][0] = h.getHistRange()[0][0];
  histRange[0][1] = h.getHistRange()[0][1];
  histRange[1][0] = h.getHistRange()[1][0];
  histRange[1][1] = h.getHistRange()[1][1];
  histRange[2][0] = h.getHistRange()[2][0];
  histRange[2][1] = h.getHistRange()[2][1];

  if(image == NULL){
    CvSize size;
    size.width = h.getImage()->width;
    size.height = h.getImage()->height;
    image = cvCreateImage( size, 8, 3);
  }
  cvCopy(h.getImage(), image);

  int length = grid.x*grid.y;

  hist = new CvHistogram*[length];
  while(length>0){
    length--;
    hist[length] = cvCreateHist( 3, histSize, CV_HIST_ARRAY, histRange, 1 );
    cvCopyHist(h.getHist()[length], &hist[length]);    
  }
}
