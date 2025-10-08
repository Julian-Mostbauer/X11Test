#include <iostream>

#include "examples/GameOfLife.h"
#include "../core/App.h"
#include "helper/allocTracker.h"
#include "helper/x11Detection.h"

using X11App::App;

int main() {
    if (!isX11Installed()) {
        fprintf(stderr, "Error: Your system does not have X11 installed or running.\n");
        return -1;
    }

#if DEBUG
    std::cout << "====================================\n"
            << "WARNING!!!\nRunning in DEBUG mode!\n"
            << "====================================\n" << std::endl;
#endif

    try {
        const auto app = App::Create<GameOfLife::GameOfLifeApp>();
        app->run();
    } catch (const std::exception &e) {
        fprintf(stderr, "Error: %s\n", e.what());
    }

#if TRACK_ALLOCATIONS
    printAllocStats();
#endif

    return 0;
}
