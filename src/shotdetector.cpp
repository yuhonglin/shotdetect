/**
 * @file   shotdetector.cpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Sat Jul  6 10:41:22 2013
 *
 * @brief  implement the class ShotDetector
 *
 *
 */

#include <set>
#include <string>
#include <sstream>

using std::set;
using std::string;
using std::stringstream;

#include "option.hpp"
#include "logger.hpp"
#include "shotdetector.hpp"
#include "bithreshold.hpp"

using dms::Option;

ShotDetector::ShotDetector(char **argv, int argc) {
  alg = NULL;
  video = NULL;
  /**
   * Parse the options and decide which algorithm to use
   *
   */

  Option op;

  op.set_head("Main option help: \n");
  op.set_tail("\n");

  op.add_string(
      "-a",                                 // the option name
      "The name algorithm you want to use", // the comment of this option
      "biThresholds");                      // default value

  op.add_string("-i", "The input video file path");

  op.add_string("-o", "The path you want to output");

  op.add_bool("--help", "show help infomation");

  if (op.parse(argc, argv, true) != true) {
    op.dump_help();
    LOG_FATAL("Parsing command line failed");
  }

  if (op.get_bool("--help") == true) {
    op.dump_help();
    string tmpalgname = op.get_string("-a");
    if (tmpalgname == "biThresholds") {
      alg = new BiThreshold(argv, argc);
    }
    exit(0);
  }

  /**
   * todo: add path checking
   *
   */

  outputPath = op.get_string("-o");

  videoFilePath = op.get_string("-i");

  string algname = op.get_string("-a");

  if (algname == "biThresholds") {
    alg = new BiThreshold(argv, argc);
  }
}

ShotDetector::~ShotDetector() {
  if (alg != NULL) {
    delete alg;
  }
  if (video != NULL) {
    delete video;
  }
}

bool ShotDetector::run() {
  if (alg == NULL) {
    LOG_FATAL("can not detect shots before specifying the algorithm");
  }

  return alg->detect();
}

bool ShotDetector::outputKeyFrame() {
  list<pair<float, float> > detectresult = alg->getShotBoundary();

  if (detectresult.size() == 0) {
    LOG_WARNING("the number of shot boundary is zero, no output");
    return true;
  }

  // video->seekTo(0);  // does not work very well, changed to reopen the video
  if (video == NULL) {
    video = new Video();
  } else {
    video->close();
  }

  video->open(videoFilePath.c_str());

  /**
   * begin to output the frames
   *
   */

  int i = 0; // tool num for loop and record the num of loops
  char fn[100] = { 0 }; // image file's name
  std::list<pair<float, float> >::iterator ci;

  double ts = 0;

  /**
   * store the original ROI and reset it
   *
   */
  cv::Rect cr = video->getROI();

  /**
   * get file name
   *
   */
  string clfn;

  string revop(videoFilePath.rbegin(),
               videoFilePath.rend()); // reversed version of the outputPath

  string revfn; // reversed version of the filename
  stringstream ss1(revop);
  std::getline(ss1, revfn, '/');

  stringstream ss2(string(revfn.rbegin(), revfn.rend()));
  std::getline(ss2, clfn, '.');

  for (ci = detectresult.begin(); ci != detectresult.end(); ci++) {
    ts = 0.5 * (*ci).first + 0.5 * (*ci).second;
    sprintf(fn, "%s/%s@%d@%f-%f-%f.jpg", outputPath.c_str(), clfn.c_str(), i,
            (*ci).first, ts, (*ci).second);
    //    cvSaveImage(fn, video->getFrame(ts));
    cv::imwrite(fn, video->getFrame(ts));
    i++;
  }

  /**
   * retrieve the original ROI
   *
   */
}
