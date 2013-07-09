#include "shotdetector.hpp"
#include "logger.hpp"

int main(int argc, char *argv[])
{
  ShotDetector sd( argv, argc );

  sd.run();

  sd.outputKeyFrame();

  return 0;
}
