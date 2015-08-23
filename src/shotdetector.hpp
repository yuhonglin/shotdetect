/**
 * @file   shotdetector.hpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Thu Jul  4 21:47:25 2013
 *
 * @brief  wrap the shot detector
 *
 *
 */

#ifndef _SHOTDETECTOR_H_
#define _SHOTDETECTOR_H_

#include <string>
using std::string;

#include "video.hpp"
#include "algorithm.hpp"

class ShotDetector {
public:
  ShotDetector(char **argv, int argc);
  virtual ~ShotDetector();

  /**
   * run the shotdection
   *
   * @return
   */
  bool run();

  /**
   * output the frames of every shot
   *
   * @return
   */
  bool outputKeyFrame();

private:
  Algorithm *alg;

  string videoFilePath;

  string outputPath;

  Video *video;
};

#endif /* _SHOTDETECTOR_H_ */
