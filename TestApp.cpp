//
// Created by julian on 9/22/25.
//

#include "TestApp.h"

#include <iostream>
#include <ranges>

namespace ExampleApp {
    void TestApp::run() {
        XEvent event;

        // create main window
        windowOpen(MAIN_WINDOW, 100, 100, 400, 300, "Test Window 1");

        // event loop
        while (true) {
            XNextEvent(m_Display, &event);
            switch (event.type) {
                case Expose:
                    handleExpose(event.xexpose);
                    break;
                case KeyPress:
                    if (keyCheckEqual(event.xkey, XK_Escape)) return;

                    handleKeyPress(event.xkey);
                    break;
                default:
                    std::cout << "Unknown event type: " << event.type << std::endl;
                    break;
            }
        }
    }

    void TestApp::drawMainWindow() const {
        const auto color = colorCreate(0, 65535, 0);
        drawRectangle(MAIN_WINDOW, 20, 20, color, 100, 100);
    }

    void TestApp::drawPopupMenu() const {
        const auto color = colorCreate(0, 0, 65535);
        drawRectangle(POPUP_MENU, 10, 10, color, 180, 80);
    }

    void TestApp::handleExpose(const XExposeEvent &xexpose) {
        std::cout << "Expose event on window: " << xexpose.window << std::endl;
        if (xexpose.count == 0) {
            auto color = colorCreate(65535, 0, 0);
            for (const auto &winId: m_Windows | std::views::keys) {
                if (!windowCheckOpen(winId)) continue;
                switch (winId) {
                    case MAIN_WINDOW:
                        drawMainWindow();
                        break;
                    case POPUP_MENU:
                        drawPopupMenu();
                        break;
                    default:
                        break;
                }
            }
        }
    }

    void TestApp::handleKeyPress(const XKeyEvent &xkey) {
        if (keyCheckEqual(xkey, XK_space)) {
            if (windowCheckOpen(POPUP_MENU)) {
                windowClose(POPUP_MENU);
            } else {
                windowOpen(POPUP_MENU, 150, 150, 200, 100, "Popup Menu");
            }
        }
        std::cout << "KeyPress event on window: " << xkey.window << " keycode: " << xkey.keycode << std::endl;
    }
}
