#include "App.h"
#include "TestApp.h"
#include <iostream>

int main() {
#if DEBUG
    std::cout << "====================================\n"
            << "WARNING!!!\nRunning in DEBUG mode!\n"
            << "====================================\n" << std::endl;
#endif
    try {
        const auto app = App::Create<TestApp>();
        app->run();
        return 0;
    } catch (const std::exception &e) {
        fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }
}
