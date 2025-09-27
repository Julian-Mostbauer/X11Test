#include <iostream>

#include "ExampleApp.h"
#include "../core/App.h"
#include "allocTracker.h"

int main() {
#if DEBUG
    std::cout << "====================================\n"
            << "WARNING!!!\nRunning in DEBUG mode!\n"
            << "====================================\n" << std::endl;
#endif
    try {
        const auto app = X11App::App::Create<ExampleApp::ExampleApp>();
        app->run();
    } catch (const std::exception &e) {
        fprintf(stderr, "Error: %s\n", e.what());
    }
#if TRACK_ALLOCATIONS
    printAllocStats();
#endif
    return 0;
}
