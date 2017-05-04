#include <set>
#include <list>
#include "bithreshold.hpp"
#include "option.hpp"

using std::set;
using dms::Option;

#define KEY_GRID_WIDTH "-gw"
#define KEY_GRID_HEIGHT "-gh"
#define KEY_GRID_BEGIN "-gb"
#define KEY_GRID_END "-ge"

#define KEY_LOW_THRESHOLD "-lt"
#define KEY_MID_THRESHOLD "-mt"
#define KEY_HIGH_THRESHOLD "-ht"
#define KEY_MIN_SHOT_LENGTH "-ms"
#define KEY_TIME_STEP "-s"
#define KEY_WIDTH_MARGIN_RATE "-wmr"
#define KEY_HEIGHT_MARGIN_RATE "-hmr"
#define KEY_CUT_BIAS "-cb"
#define KEY_MARGIN "-m"

BiThreshold::BiThreshold(char **argv, int argc)
    : Algorithm(argv, argc, "biThreshold") {

  Option op;

  op.set_head("Help information for \"biThresholds\" algorithm\n");
  op.set_tail("\n");

  op.add_int(KEY_GRID_WIDTH, "number of grid horizontally", 3, set<int>());
  op.add_int(KEY_GRID_HEIGHT, "number of grid vertically", 4, set<int>());
  op.add_int(KEY_GRID_BEGIN, "the first index to pick", 2, set<int>());
  op.add_int(KEY_GRID_END, "the last index to pick", 8, set<int>());

  op.add_float(KEY_LOW_THRESHOLD, "low threshold", 0.6, set<float>());
  op.add_float(KEY_MID_THRESHOLD, "middle threshold", 0.3, set<float>());
  op.add_float(KEY_HIGH_THRESHOLD, "high threshold", 0.98, set<float>());
  op.add_float(KEY_MIN_SHOT_LENGTH, "min short length (s)", 0.5, set<float>());
  op.add_float(KEY_TIME_STEP, "time step (s)", 0.2, set<float>());
  op.add_float(KEY_WIDTH_MARGIN_RATE, "width margin rate", 0.05, set<float>());
  op.add_float(KEY_HEIGHT_MARGIN_RATE, "height margin rate", 0.05,
               set<float>());
  op.add_float(KEY_CUT_BIAS, "cut bias", 0.0, set<float>());
  op.add_float(KEY_MARGIN, "margin", 0.0, set<float>());

  op.add_bool("--help", "show help information", false);

  op.add_string("-i", "The input video file path");

  op.parse(argc, argv, true);

  if (op.get_bool("--help")) {
    op.dump_help();
    return;
  }

  videoFilePath = op.get_string("-i");

  numGridWidth = op.get_int(KEY_GRID_WIDTH);
  numGridHeight = op.get_int(KEY_GRID_HEIGHT);
  gridBegin = op.get_int(KEY_GRID_BEGIN);
  gridEnd = op.get_int(KEY_GRID_END);

  lowThreshold = op.get_float(KEY_LOW_THRESHOLD);
  midThreshold = op.get_float(KEY_MID_THRESHOLD);
  highThreshold = op.get_float(KEY_HIGH_THRESHOLD);
  minShotLength = op.get_float(KEY_MIN_SHOT_LENGTH);
  timeStep = op.get_float(KEY_TIME_STEP);
  widthMarginRate = op.get_float(KEY_WIDTH_MARGIN_RATE);
  heightMarginRate = op.get_float(KEY_HEIGHT_MARGIN_RATE);
  cutBias = op.get_float(KEY_CUT_BIAS);
  margin = op.get_float(KEY_MARGIN);

  video = new Video();

  if (video->open(videoFilePath.c_str()) != true) {
    LOG_FATAL("can not open the video");
  }

  int w = video->getWidth();
  int h = video->getHeight();
  video->setROI(w * widthMarginRate, h * heightMarginRate,
                w * (1 - 2 * widthMarginRate), h * (1 - 2 * heightMarginRate));
}

BiThreshold::~BiThreshold() {
  if (video != NULL) {
    delete video;
  }
}

/**
 * compute the "middle average"
 *
 * @param array
 * @param length
 *
 * @return
 */
float BiThreshold::midAverage(float *array, int length) {
  /**
   * sort it
   *
   */
  float swp = 0;
  int i, j;
  for (i = 0; i < length - 1; i++) {
    for (j = i + 1; j < length; j++) {
      if (array[i] > array[j]) {
        swp = array[i];
        array[i] = array[j];
        array[j] = swp;
      }
    }
  }
  /**
   * compute the sum of the middle of the array
   *
   */
  int m = gridBegin;
  float sum = 0.0;
  while (m < gridEnd) {
    sum += array[m];
    m++;
  }
  return sum / (gridEnd - gridBegin);
}

/**
 * the function to compare two histogram
 *
 */

float BiThreshold::compare(Hist *foo, Hist *bar, int mode) {

  Grid grid = foo->getGrid();
  assert(grid.x == bar->getGrid().x);
  assert(grid.y == bar->getGrid().y);
  int length = grid.x * grid.y;
  float *diff = new float[length];
  for (int i = 0; i < length; i++) {
    diff[i] = cvCompareHist((*foo)[i], (*bar)[i], mode);
  }
  float result = midAverage(diff, length);
  delete diff;
  return result;
}

bool BiThreshold::detect() {

  shotBoundary.clear(); // clear the former result

  float midiff = 0;
  double begin = 0;
  Hist foo, bar, cache;
  foo.setGrid(numGridWidth, numGridHeight);
  bar.setGrid(numGridWidth, numGridHeight);
  cache.setGrid(numGridWidth, numGridHeight);
  bar.setImage((*video)[0]);
  bar.calculate();
  int totalnum = 10000;
  double i = 0, j = 0; /*loop value*/
  IplImage *pv = NULL;
  int last = 0;
  i = -timeStep;

  while (true) {
    i += timeStep;
    foo = bar;
    pv = (*video)[i];
    if (pv == NULL) {
      last = i;
      break;
    }
    bar.setImage(pv);
    bar.calculate();
    midiff = compare(&foo, &bar);
    if (midiff < lowThreshold) {
      if (i - begin < minShotLength)
        continue;
      /* cut detected */
      shotBoundary.push_back(
          pair<float, float>(begin + margin, i - timeStep + margin));
      begin = i;
      continue;
    }
    if (midiff < highThreshold && i - begin > minShotLength) {
      /* suspected transition detected */
      cache = foo;
      for (j = i; j < totalnum; j += timeStep) {
        foo = bar;
        pv = (*video)[j + timeStep];
        if (pv == NULL) {
          last = j;
          break;
        }
        bar.setImage(pv);
        bar.calculate();
        midiff = compare(&cache, &foo);
        if (midiff < midThreshold) {
          shotBoundary.push_back(
              pair<float, float>(begin + margin, i - margin));
          i = j;
          begin = i;
          break;
        } else {
          midiff = compare(&foo, &bar);
          if (midiff > highThreshold) {
            /*false transition, continue to iterate i*/
            i = j;
            break;
          }
        }
      }
    }
  }

  shotBoundary.push_back(pair<float, float>(begin, last));

  return true;
}
