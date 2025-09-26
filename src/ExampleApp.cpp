//
// Created by julian on 9/22/25.
//

#include "ExampleApp.h"

#include <algorithm>
#include <ranges>
#include "../core/EventMask.h"
#include <set>
#include <unistd.h>

using X11App::EventMask;
using X11App::App;
using X11App::FontDescriptor;

// todo: create separate thread for input handling
namespace ExampleApp {
    void ExampleApp::run() {
        // create main window
        const auto defaultMask = EventMask().useKeyPressMask().useKeyReleaseMask().useExposureMask().mask;
        windowOpen(MAIN_WINDOW, 100, 100, 550, 300, defaultMask, "Test Window 1");

        std::set<KeySym> pressedKeys; // todo: write wrapper for this

        while (true) {
            XEvent event;
            bool needsRedraw = false;
            const auto winAttr = windowGetAttributes(MAIN_WINDOW);

            while (XPending(m_Display)) {
                XNextEvent(m_Display, &event);
                switch (event.type) {
                    case Expose:
                        handleExpose(event.xexpose);
                        break;
                    case KeyPress: {
                        KeySym sym = XLookupKeysym(&event.xkey, 0);
                        if (sym == XK_Escape) return;
                        pressedKeys.insert(sym);
                        break;
                    }
                    case KeyRelease: {
                        KeySym sym = XLookupKeysym(&event.xkey, 0);
                        pressedKeys.erase(sym);
                        break;
                    }
                    default:
                        break;
                }
            }

            // Pop up menu logic
            if (pressedKeys.contains(XK_space)) {
                if (!windowCheckOpen(POPUP_MENU)) {
                    windowOpen(POPUP_MENU, 700, 100, 600, 500, defaultMask, "Popup Menu");
                    windowForceRedraw(POPUP_MENU);
                } else {
                    windowClose(POPUP_MENU);
                }
                pressedKeys.erase(XK_space); // prevent repeated toggling
            }

            // Movement logic
            int dx = 0, dy = 0;
            if (pressedKeys.contains(XK_Left)) dx -= player.stepSize;
            if (pressedKeys.contains(XK_Right)) dx += player.stepSize;
            if (pressedKeys.contains(XK_Up)) dy -= player.stepSize;
            if (pressedKeys.contains(XK_Down)) dy += player.stepSize;

            // Normalize diagonal movement to maintain consistent speed
            if (dx != 0 && dy != 0) {
                dx = static_cast<int>(dx / 1.414213562);
                dy = static_cast<int>(dy / 1.414213562);
            }

            if (dx != 0 || dy != 0) {
                player.pos.x = std::clamp(player.pos.x + dx, player.size, winAttr.width - player.size);
                player.pos.y = std::clamp(player.pos.y + dy, player.size, winAttr.height - player.size);
                needsRedraw = true;
            }

            usleep(16000); // ~60 FPS // todo: replace with proper timing mechanism
            if (needsRedraw) {
                windowClear(MAIN_WINDOW, true);
                windowForceRedraw(MAIN_WINDOW);
            }
        }
    }

    void ExampleApp::handleExpose(const XExposeEvent &event) {
        if (event.count != 0) return;

        for (const auto &winId: m_Windows | std::views::keys) {
            // only draw if the expose event is for this window and the window is open
            if (const auto win = m_Windows.at(winId); win != event.window || !windowCheckOpen(winId)) continue;

            switch (winId) {
                case MAIN_WINDOW: {
                    const auto green = colorCreate(0, 65535, 0);
                    drawCircle(MAIN_WINDOW, green, player.pos.x, player.pos.y, player.size);
                }

                break;
                case POPUP_MENU: {
                    const auto blue = colorCreate(0, 0, 65535);

                    drawText(POPUP_MENU, blue, 50, 460, FontDescriptor("helvetica", 150).toString(),
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
