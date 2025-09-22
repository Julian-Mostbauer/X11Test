//
// Created by julian on 9/22/25.
//

#include "TestApp.h"

#include <iostream>
#include <stdexcept>

void TestApp::run() {
    XEvent event;

    // create main window
    this->openWindow(MAIN_WINDOW, 100, 100, 400, 300, "Test Window 1");

    while (true) {
        XNextEvent(m_Display, &event);
        switch (event.type) {
            case Expose:
                this->handleExpose(event.xexpose);
                break;
            case KeyPress:
                // handling of exit outside handleKeyPress for simpler control flow
                if (isKey(event.xkey, XK_Escape))
                    // event.xkey.keycode == XKeysymToKeycode(m_Display, XK_Escape)
                    return;

                this->handleKeyPress(event.xkey);
                break;
            default:
                std::cout << "Unknown event type: " << event.type << std::endl;
                break;
        }
    }
}

void TestApp::handleExpose(const XExposeEvent &xexpose) {
    std::cout << "Expose event on window: " << xexpose.window << std::endl;
}

void TestApp::handleKeyPress(const XKeyEvent &xkey) {
    if (isKey(xkey, XK_space)) {
        if (this->isWindowOpen(POPUP_MENU)) {
            this->closeWindow(POPUP_MENU);
        } else {
            this->openWindow(POPUP_MENU, 150, 150, 200, 100, "Popup Menu");
        }
    }
    std::cout << "KeyPress event on window: " << xkey.window << " keycode: " << xkey.keycode << std::endl;
}
