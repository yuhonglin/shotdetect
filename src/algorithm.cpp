
#include "algorithm.hpp"


Algorithm::Algorithm(char** argv, int argc)
{
  
}


Algorithm::Algorithm(char** argv, int argc, Video* v, string n) : video(v), name(n)
{

}


bool Algorithm::setVideo( Video* v )
{
  video = v;

}


const list< pair<float, float> > & Algorithm::getShotBoundary()
{
  return shotBoundary;
  
}