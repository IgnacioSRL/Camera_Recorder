cmake_minimum_required(VERSION 2.8.3)
project(recorder)

##Set FLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

find_package(OpenCV REQUIRED)
find_package(Qt5Core REQUIRED)


include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${OpenCV_INCLUDE_DIR})
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${Qt5Core_INCLUDE_DIRS} 
)


add_library(cam_reader cam_reader.cpp)

add_executable(main main.cpp)


add_dependencies(main cam_reader)


target_link_libraries(cam_reader
  Qt5::Core
  ${OpenCV_LIBS}
)

target_link_libraries(main
  Qt5::Core
  cam_reader
  pthread
  ${OpenCV_LIBS}
)
