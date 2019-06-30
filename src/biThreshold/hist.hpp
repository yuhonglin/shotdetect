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

#include <vector>
#include <unordered_map>

#include "opencv2/opencv.hpp"

struct Grid {
  int x;
  int y;
};
class Hist {
public:
  Hist();
  //  Hist(Hist &h) = delete;
  ~Hist();

  // assume the input image is always RGB
  void setImageRGB(const cv::Mat& m);
  void calculate();
  void setGrid(int x, int y);
  Grid getGrid() { return grid; }

  std::vector<int>& getHistSize() {
    return histSize;
  };
  std::vector<std::vector<float>>& getHistRange() { return histRange; }
  cv::Mat getImage() { return image; }
  std::unordered_map<int, std::vector<cv::Mat>>& getHist() { return channel_hist; }

  //  void operator=(Hist &h) = delete;

private:
  // number of channels
  int nchannel;
  std::vector<int> histSize;
  // hist range for each channel
  std::vector<std::vector<float>> histRange;
  std::vector<float*> ranges; // same data s histRange, used to feed cv::calcHist
  
  cv::Mat image;
  // for each channel, there is a list of histogram
  std::unordered_map<int, std::vector<cv::Mat>> channel_hist;
  Grid grid;
};

#endif /* _HISTOGRAM_H_ */
