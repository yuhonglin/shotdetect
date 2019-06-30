/**
 * @file   bithreshold.hpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Fri Jul  5 15:48:03 2013
 *
 * @brief
 *
 *
 */

#ifndef _BITHRESHOLD_H_
#define _BITHRESHOLD_H_

#include <list>
#include <string>
using std::list;
using std::string;

#include "opencv2/opencv.hpp"

#include "hist.hpp"
#include "video.hpp"
#include "algorithm.hpp"

class BiThreshold : public Algorithm {
public:
  BiThreshold(char **argv, int argc);
  virtual ~BiThreshold();

  virtual bool detect();

private:
  /**
   * the thresholds needed for the algorithm
   *
   */

  int numGridWidth;
  int numGridHeight;
  int gridBegin;
  int gridEnd;

  float lowThreshold;
  float midThreshold;
  float highThreshold;

  float minShotLength;

  float timeStep;

  float widthMarginRate;
  float heightMarginRate;

  float cutBias;

  float margin;

  Video video;

  int nchannel;
  
  // will sort the input array
  float midAverage(std::vector<float>& array);
  float compare(Hist& foo, Hist& bar,
		cv::HistCompMethods mode = cv::HISTCMP_CORREL);
};

#endif /* _BITHRESHOLD_H_ */
