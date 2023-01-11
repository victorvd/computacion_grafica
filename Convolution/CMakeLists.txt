cmake_minimum_required(VERSION 3.5)

project(ImgConv)

find_package(OpenCV REQUIRED)

add_executable(ImgConv imagconv.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
