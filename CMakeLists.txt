cmake_minimum_required(VERSION 4.0)
project(X11Test)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG=1")

add_executable(X11Test src/main.cpp
        lib/App.cpp
        lib/App.h
        src/ExampleApp.cpp
        src/ExampleApp.h)
target_link_libraries(X11Test PRIVATE X11)