cmake_minimum_required(VERSION 3.20)
project(myproject CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE ${OpenCV_LIBS})