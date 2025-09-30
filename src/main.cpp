#include <iostream>

#include "ExampleApp.h"
#include "../core/App.h"
#include "allocTracker.h"
using X11App::App;

#if defined(__unix__) || defined(__APPLE__)
#include <X11/Xlib.h>

bool isX11Installed() {
    if (Display *d = XOpenDisplay(nullptr)) {
        XCloseDisplay(d);
        return true;
    }
    return false;
}
#else
bool isX11Installed() {
    return false;
}
#endif

int main() {
    if (!isX11Installed()) {
        std::cout << "Failed to open X11 display" << std::endl;
        return -1;
    }
#if DEBUG
    std::cout << "====================================\n"
            << "WARNING!!!\nRunning in DEBUG mode!\n"
            << "====================================\n" << std::endl;
#endif
    try {
        const auto app = App::Create<ExampleApp::ExampleApp>();
        app->run();
    } catch (const std::exception &e) {
        fprintf(stderr, "Error: %s\n", e.what());
    }
#if TRACK_ALLOCATIONS
    printAllocStats();
#endif
    return 0;
}
