/**
 * @file   main.cpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Fri Mar 28 11:06:01 2014
 *
 * @brief  short detection program
 *
 *
 */

#include "shotdetector.hpp"
#include "logger.hpp"

int main(int argc, char *argv[]) {
  ShotDetector sd(argv, argc);

  sd.run();

  sd.outputKeyFrame();

  return 0;
}
