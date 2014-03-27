/**
 * @file   video.hpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Mon Jul  1 21:08:20 2013
 * 
 * @brief  declare the Video class
 * 
 * 
 */

#ifndef  VIDEO_H
#define  VIDEO_H

#include <list>
#include <map>
#include "cv.h"
#include "highgui.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"
#include "libavutil/dict.h"
}

#define VIDEO_ITERATION_MOD 0
#define VIDEO_READ_MOD 1


/**
 * This Video Class can open a video and get any frame
 * (driver: FFmpeg, output format IplImage) at any timestamp
 * 
 */

class Video{
 public:
  Video();
  ~Video();

  bool open(const char* filename);
  /** 
   * iterate though the video one frame
   * by another
   * 
   * @return a pointer to the IplImage
   */
  IplImage* iterate();
  void close();

  /** 
   * get a frame in IplImage format at time stamp ts
   * 
   * @param ts the timestamp of the frame
   * 
   * @return a pointer to the IplImage
   */
  IplImage* getFrame(double ts=0);
  /** 
   * wrapper of IplImage* getFrame(double ts=0)
   * 
   * @param ts the timestamp of the frame
   * 
   * @return a pointer to the IplImage
   */
  IplImage* operator [] (double ts){return getFrame(ts);}

 private:
  char filename[1024];      // store the filename
  char fnPrefix[20];       // store the prefix of filename  

  IplImage* frame;        // the pointer to the IplImage

  int width;              // the width of the output frame
  int height;             // the height of the output frame
  
  int count;              // number of frames
  
  int index;              // the index of current frame
  
  int originalWidth;      // the original frame width of the video
  int originalHeight;     // the original frame height of the video
  
  std::map<int, IplImage*> frameCache;  // the frames that cached in memory

  double currentTimeStamp;       // current time stamp

  /**
   * the followings are mostly FFmpeg variables
   * 
   */
  AVFormatContext *pFormatCtx;  
  AVCodecContext *pCodecCtx;
  AVCodec *pCodec;
  AVFrame *pFrame;
  AVFrame *pFrameRGB;
  uint8_t *buffer;
  int videoStream;
  struct SwsContext *img_convert_ctx;
  AVPacket packet;
  int bytesRemaining;
  uint8_t *rawData;
  bool fFirstTime;
  int numStream;
  
  /** 
   * extract the next frame and 
   * store it in pFrame
   * 
   * @return 
   */
  bool GetNextFrame();
  void FrameToIplImage();
  void scaleAndTransform();

  CvRect roi;

 public:
  
  double getCurrentTimeStamp(){return currentTimeStamp;}
  int getWidth(){return width;}
  int getHeight(){return height;}
  
  int getCount(){return count;}

  int getIndex(){return index;}

  void seekTo(double ts);

  void setFnPrefix(const char* fnp);

  void setSize(int w, int h){width = w; height = h;};

  void setWidth(int w);

  void setHeight(int h);

  CvRect getROI();
  void setROI(CvRect & cr);
  void setROI(int x=-1, int y=-1, int w=-1, int h=-1);
  void resetROI();

  
  int str2int(const char*);
};



#endif
