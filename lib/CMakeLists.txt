add_library(logger STATIC logger.cpp logger.hpp constant.hpp)
add_library(option STATIC option.cpp option.hpp)

target_link_libraries(option logger)