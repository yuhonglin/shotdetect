
set( BITHRESHOLD_SOURCE ./biThreshold/bithreshold.hpp ./biThreshold/bithreshold.cpp ./biThreshold/hist.hpp ./biThreshold/hist.cpp )
add_executable( shotdetect main.cpp shotdetector.cpp shotdetector.hpp video.cpp video.hpp algorithm.cpp algorithm.hpp ${BITHRESHOLD_SOURCE})

# for openCV
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/lib/)
include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${CMAKE_SOURCE_DIR}/src/biThreshold/)

include_directories( PC_LIBAVCODEC_INCLUDEDIR PC_LIBAVFILTER_INCLUDEDIR PC_LIBSWSCALE_INCLUDEDIR PC_LIBSWSCALE_INCLUDEDIR PC_LIBAVUTIL_INCLUDEDIR PC_LIBAVDEVICE_INCLUDEDIR )

target_link_libraries( shotdetect option logger ${FFMEPGLIBS} ${OpenCV_LIBS})