//
// Created by julian on 9/22/25.
//

#include "TestApp.h"

#include <iostream>
#include <ranges>
#include <stdexcept>

namespace ExampleApp {
    void TestApp::run() {
        XEvent event;

        // create main window
        this->openWindow(MAIN_WINDOW, 100, 100, 400, 300, "Test Window 1");

        // event loop
        while (true) {
            XNextEvent(m_Display, &event);
            switch (event.type) {
                case Expose:
                    this->handleExpose(event.xexpose);
                    break;
                case KeyPress:
                    if (isKey(event.xkey, XK_Escape)) return;

                    this->handleKeyPress(event.xkey);
                    break;
                default:
                    std::cout << "Unknown event type: " << event.type << std::endl;
                    break;
            }
        }
    }

    void TestApp::drawMainWindow() const {
        const auto color = this->createColor(0, 65535, 0);
        this->drawRectangle(MAIN_WINDOW, 20, 20, color, 100, 100);
    }

    void TestApp::drawPopupMenu() const {
        const auto color = this->createColor(0, 0, 65535);
        this->drawRectangle(POPUP_MENU, 10, 10, color, 180, 80);
    }

    void TestApp::handleExpose(const XExposeEvent &xexpose) {
        std::cout << "Expose event on window: " << xexpose.window << std::endl;
        if (xexpose.count == 0) {
            auto color = this->createColor(65535, 0, 0);
            for (const auto &winId: m_Windows | std::views::keys) {
                if (!this->isWindowOpen(winId)) continue;
                switch (winId) {
                    case MAIN_WINDOW:
                        this->drawMainWindow();
                        break;
                    case POPUP_MENU:
                        this->drawPopupMenu();
                        break;
                    default:
                        break;
                }
            }
        }
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
}
