/**
 * @file   video.cpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Mon Jul  1 21:28:00 2013
 * 
 * @brief  implement the Video Class
 * 
 * 
 */

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <cmath>
#include "assert.h"
extern "C"{
#include <stdint.h>
}
#include "cv.h"

#include "video.hpp"
#include "logger.hpp"

int Video::str2int(const char* s){
  int len = strlen(s);
  int result = 0;
  for(int i=0; i<len; i++){
    result+=(int(s[len-i-1]) - 48)*pow(10, i);
  }
  return result;
}


Video::Video(){
  
  width = 360;
  height = 200;
  originalWidth = 0;
  originalHeight = 0;
  count = 0;
  index = 0;
  frame = NULL;
  strcpy(filename, "");
  strcpy(fnPrefix, "");
  roi.x = -1;
  roi.y = -1;
  roi.width=-1;
  roi.height=-1;

  currentTimeStamp = 0;

  /**
   * initialise FFmpeg variables 
   * 
   */

  pFormatCtx = NULL; /*important*/
  pCodecCtx = NULL;
  buffer = NULL;
  videoStream = -11;
  img_convert_ctx = NULL;
  pCodec = NULL;
  pCodecCtx = NULL;
  bytesRemaining = 0;
  fFirstTime = true;
  
}


Video::~Video(){

  if(frame != NULL){
    cvReleaseImage(&frame);
    frame = NULL;
  }

  fFirstTime = true;
  bytesRemaining = 0;

}


bool Video::open(const char* fn )
{

  avcodec_register_all();
  avfilter_register_all();
  av_register_all(); // register all the component 
  avformat_network_init();
  strcpy(filename, fn);

  /**
   * I do not know why that if I directly run
   *     avformat_open_input(&pFormatCtx, filename, NULL, NULL)
   * then can not open the video ( "moov atom not found" )
   * But if I use a local variable tmp, then every thing works
   */
  AVFormatContext * tmp = NULL;
  if(avformat_open_input(&tmp, filename, NULL, NULL)!=0)
    {
      LOG_WARNING("Can not open video file");
      return false;
    }
  pFormatCtx = tmp;
  numStream = pFormatCtx->nb_streams;

  AVDictionary ** opts=NULL;
  if (avformat_find_stream_info(pFormatCtx, opts)<0)
    {
      LOG_WARNING("Can not find stream information");
      return false;
    }

  for(int i=0; i<pFormatCtx->nb_streams; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==0)
      {
        videoStream=i;
        break;
      }
  
  if(videoStream<-1){
    LOG_WARNING("Can not find video stream.");
    return false;
  }

  pCodecCtx=pFormatCtx->streams[videoStream]->codec;
  pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
  if(pCodec==NULL){
    LOG_WARNING("Can not find decoder");
    return 1;
  }
 

  opts = NULL;
  if(avcodec_open2(pCodecCtx, pCodec, opts)<0)
    LOG_WARNING("Can not open codec");


  count = (int)(pFormatCtx->streams[videoStream]->nb_frames);

  originalWidth = pCodecCtx->width;
  originalHeight = pCodecCtx->height;

  frame = cvCreateImage(cvSize(width, height), 8, 3);

  if(roi.x>0){
    cvSetImageROI(frame, roi);
  }
  
  pFrame = avcodec_alloc_frame();
  pFrameRGB=avcodec_alloc_frame();
  buffer=new uint8_t[avpicture_get_size(PIX_FMT_RGB24, width, height)];
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, width, height); 
  img_convert_ctx = sws_getContext( originalWidth, originalHeight, pCodecCtx->pix_fmt, width, height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

  //avformat_seek_file(pFormatCtx, -1, (-9223372036854775807L-1), 0, 9223372036854775807L, AVSEEK_FLAG_FRAME);
  //avformat_seek_file(pFormatCtx, videoStream, -1, 0, 10000000, AVSEEK_FLAG_FRAME);
  
  return true;
}


void Video::close(){

  if(frame != NULL){
    cvReleaseImage(&frame);
    frame = NULL;
  }

  if(pFrameRGB!=NULL){
    av_free(pFrameRGB);
    pFrameRGB = NULL;
  }

  if(pFrame!=NULL){
    av_free(pFrame);
    pFrame = NULL;
  }

  if(pFormatCtx!=NULL){
    avformat_close_input(&pFormatCtx);
    pFormatCtx = NULL;
  }

  av_free_packet(&packet);

  bytesRemaining = 0;
  fFirstTime = true;

  currentTimeStamp = 0;
}

IplImage* Video::getFrame(double ts){

  for(int p=0; p<100000; p++){
    if(GetNextFrame()==0)
      return NULL;
    if(currentTimeStamp>=ts){
      break;
    }
  }
  scaleAndTransform();
  return frame;
}

void Video::scaleAndTransform(){
  sws_scale( img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize,  0,  pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
  FrameToIplImage();
}


void Video::seekTo(double ts){

  fFirstTime = true;
  avformat_seek_file(pFormatCtx, -1, (-9223372036854775807L-1), ts, 9223372036854775807L, 0);
  //avcodec_flush_buffers(pCodecCtx);
  currentTimeStamp = ts;
}

void Video::setFnPrefix(const char* fnp){
  strcpy( fnPrefix, fnp);
}

bool Video::GetNextFrame(){

  int bytesDecoded;
  int frameFinished = 0;

  if(fFirstTime)
    {
      fFirstTime=false;
      av_init_packet(&packet);
    }

  while(frameFinished==0){
    do
      {
	/* free old packet */
	if(packet.data!=NULL)
	  av_free_packet(&packet);
	/* read new packet */
	if(av_read_frame(pFormatCtx, &packet)<0)
	  {
	    //std::cerr<<"read packet error"<<std::endl;
	    return false;
	  }
      } while(packet.stream_index!=videoStream);
    bytesRemaining -= avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
  }
  /* Get the PTS */
  if(pFrame->pkt_pts > 0){
    /* the last packet decoded contains pts, so use it to updata timestamp */
    currentTimeStamp = pFrame->pkt_pts*av_q2d(pFormatCtx->streams[videoStream]->time_base); 
  }else{
     /* The last packet decoded does not contain pts, use stream time base to increase it. */
    currentTimeStamp = pFrame->best_effort_timestamp* av_q2d(pFormatCtx->streams[videoStream]->time_base); 
  }
  return true;
}


  void Video::FrameToIplImage(){

  //frame->nSize = sizeof(IplImage);
  //frame->ID = 0; // always zero
  //frame->nChannels = 3;
  //ipliamge->alphaChannel = NULL; //ignored by opencv
  //frame->depth = IPL_DEPTH_8U;
  //ipliamge->colorModel = NULL; //ignored by opencv
  //ipliamge->channelSeq = NULL; //ignored by opencv
  //frame->dataOrder = 0; //interleaved color channels
  //frame->origin = 0; //top-left origin
  //iplimage->align = 4; //ignored by opencv
  frame->width = width;
  frame->height = height;
  //frame->roi = NULL;
  //frame->maskROI = NULL; //must be NULL in opencv
  //frame->imageId = NULL; //must be NULL in opencv
  //frame->tileInfo = NULL; //must be NULL in opencv
  frame->imageSize = height*width*3;
  frame->widthStep = width*3;
  //iplimage->BorderMode; //ignored by opencv
  //iplimage->BorderConst; //ignored by opencv
  //if(frame->imageData!=NULL){
  //  delete frame->imageData;
  //}
  //frame->imageData = new char[frame->imageSize];
  int num = width*height;
  for(int i=0; i<num; i++){
      frame->imageData[i*3] = pFrameRGB->data[0][i*3+2];
      frame->imageData[i*3+1] = pFrameRGB->data[0][i*3+1];
      frame->imageData[i*3+2] = pFrameRGB->data[0][i*3];
    }
	
  //	frame->imageData = (char*)(frame->imageDataOrigin);
      
}

    
void Video::setWidth(int w){
  width = w;
}

void Video::setHeight(int h){
  height = h;
}

CvRect Video::getROI()
{
  return roi;
}

void Video::setROI(CvRect & cr)
{
  roi = cr;
  if(frame!=NULL){
    cvSetImageROI(frame, roi);
  }

}

void Video::setROI(int x, int y, int w, int h)
{
  roi.x = x;
  roi.y = y;
  roi.width = w;
  roi.height = h;
  if(frame!=NULL){
    cvSetImageROI(frame, roi);
  }
}

void Video::resetROI(){
  roi.x = -1;
  roi.y = -1;
  roi.width = -1;
  roi.height = -1;
  cvResetImageROI(frame);
}
