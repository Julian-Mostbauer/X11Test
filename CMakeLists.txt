cmake_minimum_required(VERSION 4.0)
project(X11Test)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG=1")

add_executable(X11Test main.cpp
        App.cpp
        App.h
        TestApp.cpp
        TestApp.h)
target_link_libraries(X11Test PRIVATE X11)