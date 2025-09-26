//
// Created by julian on 9/22/25.
//

#ifndef X11TEST_TESTAPP_H
#define X11TEST_TESTAPP_H
#include <unordered_set>

#include "../core/App.h"

namespace ExampleApp {
    enum TestAppWindows {
        MAIN_WINDOW = 1,
        POPUP_MENU = 2,
    };

    struct Player {
        XPoint pos;
        int size;
        int stepSize;
    };

    class ExampleApp final : public X11App::App {
        friend App;

        static long defaultMask;

        std::unordered_set<KeySym> pressedKeys;
        Player player;
        bool running;

        explicit ExampleApp(Display *display)
            : App(display), player({100, 100}, 20, 10), running(true) {
        }

        bool updatePlayer();

        void updatePopupMenu();

        void handleExpose(XExposeEvent &event) override;

        void handleKeyPress(XKeyEvent &event) override;

        void handleKeyRelease(XKeyEvent &event) override;

    public:
        void run() override;
    };
}

#endif //X11TEST_TESTAPP_H
