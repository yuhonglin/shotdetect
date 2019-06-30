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
extern "C" {
#include <stdint.h>
}

#include "video.hpp"
#include "logger.hpp"

int Video::str2int(const char *s) {
  int len = strlen(s);
  int result = 0;
  for (int i = 0; i < len; i++) {
    result += (int(s[len - i - 1]) - 48) * pow(10, i);
  }
  return result;
}

Video::Video() {

  width = 360;
  height = 200;
  originalWidth = 0;
  originalHeight = 0;
  count = 0;
  index = 0;
  strcpy(filename, "");
  strcpy(fnPrefix, "");
  roi.x = -1;
  roi.y = -1;
  roi.width = -1;
  roi.height = -1;

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

Video::~Video() {

  fFirstTime = true;
  bytesRemaining = 0;
}

bool Video::open(const char *fn) {
  avcodec_register_all();
  //  avfilter_register_all();
  av_register_all(); // register all the component

  /**
   * 1. I do not know why that if I directly run
   *     avformat_open_input(&pFormatCtx, filename, NULL, NULL)
   * then can not open the video ( "moov atom not found" )
   * But if I use a local variable tmp, then every thing works
   *
   * 2. '/' in filename must be "//"
   */

  strcpy(filename, fn);

  AVFormatContext *tmp = NULL;

  if (avformat_open_input(&tmp, filename, NULL, NULL) != 0) {
    LOG_FATAL("Can not open video file : be sure that use \"//\", not \"/\" in "
              "filename ");
    return false;
  }
  pFormatCtx = tmp;
  numStream = pFormatCtx->nb_streams;

  AVDictionary **opts = NULL;
  if (avformat_find_stream_info(pFormatCtx, opts) < 0) {
    LOG_WARNING("Can not find stream information");
    return false;
  }

  for (int i = 0; i < pFormatCtx->nb_streams; i++)
    if (pFormatCtx->streams[i]->codec->codec_type == 0) {
      videoStream = i;
      break;
    }

  if (videoStream < -1) {
    LOG_WARNING("Can not find video stream.");
    return false;
  }

  pCodecCtx = pFormatCtx->streams[videoStream]->codec;
  pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
  if (pCodec == NULL) {
    LOG_WARNING("Can not find decoder");
    return 1;
  }

  opts = NULL;
  if (avcodec_open2(pCodecCtx, pCodec, opts) < 0)
    LOG_WARNING("Can not open codec");

  count = (int)(pFormatCtx->streams[videoStream]->nb_frames);

  originalWidth = pCodecCtx->width;
  originalHeight = pCodecCtx->height;

  //  frame = cvCreateImage(cvSize(width, height), 8, 3);

  if (roi.x > 0) {
    frame = frame(roi);
  }

  pFrame = av_frame_alloc();
  pFrameRGB = av_frame_alloc();
  buffer = new uint8_t[avpicture_get_size(PIX_FMT_RGB24, width, height)];
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, width, height);
  img_convert_ctx =
      sws_getContext(originalWidth, originalHeight, pCodecCtx->pix_fmt, width,
                     height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

  return true;
}

void Video::close() {

  if (pFrameRGB != NULL) {
    av_free(pFrameRGB);
    pFrameRGB = NULL;
  }

  if (pFrame != NULL) {
    av_free(pFrame);
    pFrame = NULL;
  }

  if (pFormatCtx != NULL) {
    avformat_close_input(&pFormatCtx);
    pFormatCtx = NULL;
  }

  av_free_packet(&packet);

  bytesRemaining = 0;
  fFirstTime = true;

  currentTimeStamp = 0;
}

cv::Mat Video::getFrame(double ts) {

  for (int p = 0; p < 100000; p++) {
    if (getNextFrame() == 0)
      return cv::Mat();
    if (currentTimeStamp >= ts) {
      break;
    }
  }
  scaleAndTransform();
  return frame;
}

void Video::scaleAndTransform() {
  sws_scale(img_convert_ctx, (const uint8_t * const *)pFrame->data,
            pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
            pFrameRGB->linesize);
  frameToCvMat();
}

void Video::seekTo(double ts) {

  fFirstTime = true;
  avformat_seek_file(pFormatCtx, -1, (-9223372036854775807L - 1), ts,
                     9223372036854775807L, 0);
  // avcodec_flush_buffers(pCodecCtx);
  currentTimeStamp = ts;
}

void Video::setFnPrefix(const char *fnp) { strcpy(fnPrefix, fnp); }

bool Video::getNextFrame() {

  int bytesDecoded;
  int frameFinished = 0;

  if (fFirstTime) {
    fFirstTime = false;
    av_init_packet(&packet);
  }

  while (frameFinished == 0) {
    do {
      /* free old packet */
      if (packet.data != NULL)
        av_free_packet(&packet);
      /* read new packet */
      if (av_read_frame(pFormatCtx, &packet) < 0) {
        // std::cerr<<"read packet error"<<std::endl;
        return false;
      }
    } while (packet.stream_index != videoStream);
    bytesRemaining -=
        avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
  }
  /* Get the PTS */
  if (pFrame->pkt_pts > 0) {
    /* the last packet decoded contains pts, so use it to updata timestamp */
    currentTimeStamp =
        pFrame->pkt_pts * av_q2d(pFormatCtx->streams[videoStream]->time_base);
  } else {
    /* The last packet decoded does not contain pts, use stream time base to
     * increase it. */
    currentTimeStamp = pFrame->best_effort_timestamp *
                       av_q2d(pFormatCtx->streams[videoStream]->time_base);
  }
  return true;
}

void Video::frameToCvMat() {

  // /// First method, first fill zero and then overwrite
  // frame = cv::Mat::zeros(height, width, CV_8UC3);

  // int num = width * height;
  // for (int i = 0; i < num; i++) {
  //   frame.data[i * 3] = pFrameRGB->data[0][i * 3 + 2];
  //   frame.data[i * 3 + 1] = pFrameRGB->data[0][i * 3 + 1];
  //   frame.data[i * 3 + 2] = pFrameRGB->data[0][i * 3];
  // }

  /// Second method, first fill pFrameRGB->data then swap red and blue
  frame = cv::Mat(height, width, CV_8UC3, pFrameRGB->data[0]);

  int num = width * height;
  for (int i = 0; i < num; i++) {
    std::swap(frame.data[i * 3], frame.data[i * 3 + 2]);
  }

}

void Video::setWidth(int w) { width = w; }

void Video::setHeight(int h) { height = h; }

cv::Rect Video::getROI() { return roi; }

void Video::setROI(cv::Rect &cr) {
  roi = cr;
  if (!frame.empty()) {
    frame = frame(roi);
  }
}

void Video::setROI(int x, int y, int w, int h) {
  roi.x = x;
  roi.y = y;
  roi.width = w;
  roi.height = h;
  if (!frame.empty()) {
    frame = frame(roi);
  }
}

