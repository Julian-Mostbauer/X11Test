//
// Created by julian on 9/22/25.
//

#ifndef X11TEST_TESTAPP_H
#define X11TEST_TESTAPP_H
#include "App.h"

enum TestAppWindows {
    MAIN_WINDOW = 1,
    POPUP_MENU = 2,
};

class TestApp final : public App {
    friend App;

    explicit TestApp(Display *display)
        : App(display) {
    }

    void handleExpose(const XExposeEvent &xexpose);

    void handleKeyPress(const XKeyEvent &xkey);

public:
    void run() override;
};


#endif //X11TEST_TESTAPP_H
