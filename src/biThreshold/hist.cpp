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
#include "logger.hpp"

Hist::Hist() : histSize(3), histRange(3), ranges(3), nchannel(3) {

  histSize[0] = 15;
  histSize[1] = 10;
  histSize[2] = 1;

  histRange[0] = {0.0, 181};
  histRange[1] = {0.0, 256};
  histRange[2] = {0.0, 256};

  ranges[0] = histRange[0].data();
  ranges[1] = histRange[1].data();
  ranges[2] = histRange[2].data();
  
  grid.x = 1;
  grid.y = 1;
  
}

Hist::~Hist() {
}

void Hist::setGrid(int x, int y) {
  grid.x = x;
  grid.y = y;
  if (!channel_hist.empty()) {
    channel_hist.clear();
  }
}

void Hist::calculate() {

  assert(!image.empty());

  int n = grid.x * grid.y;

  // resize hist
  channel_hist.clear(); for (int ch=0; ch<nchannel; ch++) channel_hist[ch].resize(n);
  
  // The input image is already transformed to HSV in Hist::setImage()

  // split the channels
  std::vector<cv::Mat> planes;
  split(image, planes);

  /// Compute the histograms
  // define the iterate roi
  cv::Rect ROI;
  ROI.width = image.size().width / grid.x;
  ROI.height = image.size().height / grid.y;

  const int channels = 0;
  int iter_hist = 0;

  for (int i = 0; i < grid.y; i++) {
    ROI.y = i * ROI.height;
    for (int j = 0; j < grid.x; j++) {
      ROI.x = j * ROI.width;
      for(int ch=0; ch<nchannel; ch++) {
	auto tmp = planes[ch](ROI);
	cv::calcHist(&tmp,
		     1,   // nimages
		     &channels,       // channels used
		     cv::Mat(),       // mask (no mask)
		     channel_hist[ch][iter_hist], // hist
		     1,                     // 1-d histogram
		     histSize.data(),       // hist size
		     const_cast<const float**>(ranges.data()), // ranges
		                                               // about the cast: stupid interface
		     true,                  // uniform?
		     false);                // accumulate?
      }

      iter_hist++;
		   
    }
  }
}

void Hist::setImageRGB(const cv::Mat& im) {
  cv::cvtColor(im, image, cv::COLOR_BGR2HSV); // convert to HSV
}
