//
// Created by julian on 9/22/25.
//

#include "ExampleApp.h"

#include <algorithm>
#include <ranges>
#include <unistd.h>

// todo: create separate thread for input handling
namespace ExampleApp {
    void ExampleApp::run() {
        windowOpen(MAIN_WINDOW, 100, 100, 550, 300,
                   defaultMask, "Test Window 1");

        while (running) {
            handleAllQueuedEvents();

            if (!keyStateChanged()) continue;
            if (keyIsPressed(XK_Escape)) {
                running = false;
                continue;
            }

            updatePopupMenu();
            updatePlayer();

            if (keyIsPressed(XK_p)) soundPlayFile("../assets/explosion-42132.mp3");

            usleep(16000); // ~60 FPS // todo: replace with proper timing mechanism
            windowProcessRedrawQueue();
        }
    }

    void ExampleApp::updatePopupMenu() {
        if (keyIsPressed(XK_space)) {
            if (!windowCheckOpen(POPUP_MENU)) {
                windowOpen(POPUP_MENU, 700, 100, 600, 500, defaultMask, "Popup Menu");
                windowForceRedraw(POPUP_MENU);
            } else {
                windowClose(POPUP_MENU);
            }
        }
        if (!windowCheckOpen(POPUP_MENU)) return; // only check for clear if popup is open

        if (keyIsPressed(XK_c)) {
            polygonPoints.clear();
            drawText(POPUP_MENU, colorCreate(65535, 0, 0), 100, 50, defaultFont,
                     "Cleared points - Press anywhere to start over");
        }
    }

    void ExampleApp::updatePlayer() {
        const auto winAttr = windowGetAttributes(MAIN_WINDOW);
        bool needsRedraw = false;
        int dx = 0, dy = 0;

        if (keyIsDown(XK_Left)) dx -= player.stepSize;
        if (keyIsDown(XK_Right)) dx += player.stepSize;
        if (keyIsDown(XK_Up)) dy -= player.stepSize;
        if (keyIsDown(XK_Down)) dy += player.stepSize;

        // Normalize diagonal movement to maintain consistent speed
        if (dx != 0 && dy != 0) {
            dx = static_cast<int>(dx / 1.414213562);
            dy = static_cast<int>(dy / 1.414213562);
        }

        if (dx != 0 || dy != 0) {
            player.pos.x = static_cast<short>(std::clamp(player.pos.x + dx, player.size, winAttr.width - player.size));
            player.pos.y = static_cast<short>(std::clamp(player.pos.y + dy, player.size, winAttr.height - player.size));
            needsRedraw = true;
        }

        if (needsRedraw) windowScheduleRedraw(MAIN_WINDOW);
    }

    void ExampleApp::handleButtonPress(XButtonEvent &event) {
        const auto winId = windowRawToId(event.window);
        if (!winId.has_value() || !windowCheckOpen(winId.value())) return;

        if (winId == POPUP_MENU) {
            std::cout << event.x << ", " << event.y << std::endl;
            const XPoint p = {static_cast<short>(event.x), static_cast<short>(event.y)};
            polygonPoints.push_back(p);

            windowScheduleRedraw(POPUP_MENU);
        }
    }

    void ExampleApp::handleExpose(XExposeEvent &event) {
        if (event.count != 0) return;
        const auto winId = windowRawToId(event.window);
        if (!winId.has_value() || !windowCheckOpen(winId.value())) return;

        switch (winId.value()) {
            case MAIN_WINDOW: {
                const auto green = colorCreate(0, 65535, 0);
                drawCircle(MAIN_WINDOW, green, player.pos.x, player.pos.y, player.size);
            }

            break;
            case POPUP_MENU: {
                const auto blue = colorCreate(0, 0, 65535);
                const auto red = colorCreate(65535, 0, 0);

                drawText(POPUP_MENU, blue, 50, 100, defaultFont, "This is a popup menu. Press SPACE to close.");
                drawText(POPUP_MENU, blue, 50, 300, defaultFont, "Click anywhere to add points. Press C to clear.");

                if (polygonPoints.size() >= 3) drawPolygon(POPUP_MENU, blue, polygonPoints);
                else drawText(POPUP_MENU, red, 100, 400, defaultFont, "Not enough points to draw a polygon");
            }
            break;
            default: break;
        }
    }
}
