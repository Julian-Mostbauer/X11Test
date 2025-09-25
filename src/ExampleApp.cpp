//
// Created by julian on 9/22/25.
//

#include "ExampleApp.h"

#include <iostream>
#include <ranges>

namespace ExampleApp {
    void ExampleApp::run() {
        XEvent event;

        // create main window
        windowOpen(MAIN_WINDOW, 100, 100, 550, 300, "Test Window 1");

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

    void ExampleApp::handleExpose(const XExposeEvent &xexpose) {
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

    void ExampleApp::handleKeyPress(const XKeyEvent &xkey) {
        if (keyCheckEqual(xkey, XK_space)) {
            if (windowCheckOpen(POPUP_MENU)) {
                windowClose(POPUP_MENU);
            } else {
                windowOpen(POPUP_MENU, 500, 150, 500, 500, "Popup Menu");
            }
        }
        std::cout << "KeyPress event on window: " << xkey.window << " keycode: " << xkey.keycode << std::endl;
    }

    void ExampleApp::drawMainWindow() const {
        const auto color = colorCreate(0, 65535, 0);
        drawCircle(MAIN_WINDOW, color, 100, 100, 50);
        drawText(MAIN_WINDOW, color, 10, 250, X11App::FontDescriptor("helvetica", 120),
                 "Press SPACE to open/close popup menu. ESC to exit.");
    }

    void ExampleApp::drawPopupMenu() const {
        const auto color = colorCreate(0, 0, 65535);

        drawText(POPUP_MENU, color, 50, 460, X11App::FontDescriptor("helvetica", 150),
                 "This is a popup menu. Press SPACE to close.");
        drawRectangle(POPUP_MENU, color, 20, 20, 200, 100);
        drawCircle(POPUP_MENU, color, 300, 200, 75);

        std::vector<XPoint> triangle = {{400, 400}, {450, 300}, {500, 400}};
        drawPolygon(POPUP_MENU, color, triangle);
    }
}
