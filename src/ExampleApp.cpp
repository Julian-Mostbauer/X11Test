//
// Created by julian on 9/22/25.
//

#include "ExampleApp.h"

#include <iostream>
#include <ranges>
#include "../core/EventMask.h"

using X11App::EventMask;
using X11App::App;
using X11App::FontDescriptor;

namespace ExampleApp {
    void ExampleApp::run() {
        XPoint playerPos = {50, 50};

        // create main window
        const auto defaultMask = EventMask().useKeyPressMask().useExposureMask().mask;
        windowOpen(MAIN_WINDOW, 100, 100, 550, 300, defaultMask, "Test Window 1");

        // event loop
        while (true) {
            constexpr short stepSize = 10;
            XEvent event;
            XNextEvent(m_Display, &event);
            bool needsRedraw = false;
            const auto winAttr = windowGetAttributes(MAIN_WINDOW);

            switch (event.type) {
                case Expose:
                    handleExpose(event.xexpose, playerPos);
                    break;
                case KeyPress:
                    if (keyCheckEqual(event.xkey, XK_Escape)) return;

                    if (keyCheckEqual(event.xkey, XK_Left)) {
                        playerPos.x = (playerPos.x >= stepSize) ? playerPos.x - stepSize : 0;
                        needsRedraw = true;
                    }

                    if (keyCheckEqual(event.xkey, XK_Right) && playerPos.x <= winAttr.width) {
                        playerPos.x = (playerPos.x <= winAttr.width - stepSize) ? playerPos.x + stepSize : winAttr.width;
                        needsRedraw = true;
                    }

                    if (keyCheckEqual(event.xkey, XK_Up) && playerPos.y >= stepSize) {
                        playerPos.y = (playerPos.y >= stepSize) ? playerPos.y - stepSize : 0;
                        needsRedraw = true;
                    }

                    if (keyCheckEqual(event.xkey, XK_Down) && playerPos.y <= winAttr.height) {
                        playerPos.y = (playerPos.y <= winAttr.height - stepSize) ? playerPos.y + stepSize : winAttr.height;
                        needsRedraw = true;
                    }

                    break;
                default:
                    break;
            }

            if (needsRedraw) {
                windowClear(MAIN_WINDOW, true);
                handleExpose(XExposeEvent{.type = Expose, .count = 0}, playerPos); // force redraw todo: better way?
            }
        }
    }

    void ExampleApp::handleExpose(const XExposeEvent &xexpose, const XPoint &playerPos) {
        if (xexpose.count == 0) {
            for (const auto &winId: m_Windows | std::views::keys) {
                if (!windowCheckOpen(winId)) continue;
                switch (winId) {
                    case MAIN_WINDOW: {
                        const auto green = colorCreate(0, 65535, 0);
                        drawCircle(MAIN_WINDOW, green, playerPos.x, playerPos.y, 20);
                    }

                    break;
                    case POPUP_MENU: {
                        const auto blue = colorCreate(0, 0, 65535);

                        drawText(POPUP_MENU, blue, 50, 460, FontDescriptor("helvetica", 150),
                                 "This is a popup menu. Press SPACE to close.");
                        drawRectangle(POPUP_MENU, blue, 20, 20, 200, 100);
                        drawCircle(POPUP_MENU, blue, 300, 200, 75);

                        std::vector<XPoint> triangle = {{400, 400}, {450, 300}, {500, 400}};
                        drawPolygon(POPUP_MENU, blue, triangle);
                    }
                    break;
                    default: break;
                }
            }
        }
    }
}
