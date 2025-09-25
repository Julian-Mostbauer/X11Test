#include "../core/App.h"
#include "ExampleApp.h"
#include <iostream>

int main() {
#if DEBUG
    std::cout << "====================================\n"
            << "WARNING!!!\nRunning in DEBUG mode!\n"
            << "====================================\n" << std::endl;
#endif
    try {
        const auto app = X11App::App::Create<ExampleApp::ExampleApp>();
        app->run();
        return 0;
    } catch (const std::exception &e) {
        fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }
}
