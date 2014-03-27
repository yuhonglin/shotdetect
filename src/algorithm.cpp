
#include "algorithm.hpp"

#include "option.hpp"

Algorithm::Algorithm(char** argv, int argc)
{
  
}


Algorithm::Algorithm(char** argv, int argc, string n) : name(n)
{
  dms::Option op;
  
  op.add_string( "-i",
		 "The input video file path");
  
  op.add_string( "-o",
		 "The path you want to output" );

  op.parse(argc, argv, true);

  videoFilePath = op.get_string( "-i" );

}


bool Algorithm::setVideo( Video* v )
{
  video = v;

}


const list< pair<float, float> > & Algorithm::getShotBoundary()
{
  return shotBoundary;
  
}
