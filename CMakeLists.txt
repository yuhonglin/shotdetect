cmake_minimum_required(VERSION 2.8)

project ( shotdetect )
set (CMAKE_CXX_STANDARD 11)

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/ )

# for ffmpeg
find_package( PkgConfig )
if( PKG_CONFIG_FOUND )
    # If pkg-config finds libavcodec, this will set:
    # PC_LIBAVCODEC_FOUND (to TRUE)
    # PC_LIBAVCODEC_INCLUDEDIR
    # PC_LIBAVCODEC_INCLUDE_DIRS
    # PC_LIBAVCODEC_LIBDIR
    # PC_LIBAVCODEC_LIBRARY_DIRS
    # These variables are then used as hints to find_path()
    # and find_library()
    pkg_search_module( PC_LIBAVCODEC libavcodec )
    pkg_search_module( PC_LIBAVFORMAT libavformat )
    pkg_search_module( PC_LIBAVFILTER libavfilter )
    pkg_search_module( PC_LIBSWSCALE libswscale )
    pkg_search_module( PC_LIBAVUTIL libavutil )
    pkg_search_module( PC_LIBAVDEVICE libavdevice )
endif( PKG_CONFIG_FOUND )
set( FFMEPGLIBS ${PC_LIBAVCODEC_LDFLAGS} ${PC_LIBAVFORMAT_LDFLAGS} ${PC_LIBAVFILTER_LDFLAGS} ${PC_LIBSWSCALE_LDFLAGS} ${PC_LIBAVUTIL_LDFLAGS} ${PC_LIBAVDEVICE_LDFLAGS})
# to use some c micros (used by libavtuil)
add_definitions(-D__STDC_CONSTANT_MACROS)

# check for openCV
find_package( OpenCV )

LINK_DIRECTORIES(${PC_LIBAVCODEC_LIBDIR} ${PC_LIBAVFORMAT_LIBDIR} ${PC_LIBAVFILTER_LIBDIR} ${PC_LIBSWSCALE_LIBDIR} ${PC_LIBAVUTIL_LIBDIR} ${PC_LIBAVDEVICE_LIBDIR})

add_subdirectory( lib )

add_subdirectory( src )
