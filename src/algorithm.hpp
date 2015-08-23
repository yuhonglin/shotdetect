/**
 * @file   algorithm.hpp
 * @author yuhonglin <yuhonglin1986@gmail.com>
 * @date   Thu Jul  4 21:50:53 2013
 *
 * @brief  implement the algorithm interface
 *
 *
 */

#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include <list>
#include <string>
#include <utility>

#include "video.hpp"

using std::list;
using std::string;
using std::pair;

class Algorithm {
public:
  Algorithm(char **argv, int argc);
  Algorithm(char **argv, int argc, string n);
  virtual ~Algorithm() {};

  /**
   * The detect function, run the algorithm,
   * Must be overwritten by the children
   * Return the shot detect boundaries (timestamp)
   *
   */
  virtual bool detect() = 0;

  bool setVideo(Video *v);

  const list<pair<float, float> > &getShotBoundary();

protected:
  /**
   * Some common properties, can not be changed after
   * the object is created
   *
   */
  string name;

  /**
   * The video object
   *
   */

  Video *video;
  string videoFilePath;

  /**
   * the shot boundaries (s)
   *
   */
  list<pair<float, float> > shotBoundary;
};

#endif /* _ALGORITHM_H_ */
