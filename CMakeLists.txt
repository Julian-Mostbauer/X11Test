cmake_minimum_required(VERSION 3.31.6)
project(X11Test)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG=1 -DTRACK_ALLOCATIONS=0 -Wall -Wextra -Wpedantic -Werror")

add_executable(X11Test src/main.cpp
        core/App.cpp
        core/App.h
        src/example/ExampleApp.cpp
        src/example/ExampleApp.h
        core/lib/FontDescriptor.h
        core/lib/EventMask.h
        src/helper/allocTracker.h
        core/lib/KeyStateManager.h
        src/helper/ScopeTimer.h
        core/lib/AtomManager.h)
target_link_libraries(X11Test PRIVATE X11)