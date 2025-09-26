//
// Created by julian on 9/22/25.
//

#ifndef X11TEST_TESTAPP_H
#define X11TEST_TESTAPP_H
#include "../core/App.h"

namespace ExampleApp {
    enum TestAppWindows {
        MAIN_WINDOW = 1,
        POPUP_MENU = 2,
    };

    struct Player {
        XPoint pos;
        int size;
    };

    class ExampleApp final : public X11App::App {
        friend App;
        Player player;

        explicit ExampleApp(Display *display)
            : App(display), player({100, 100}, 20) {
        }

        void handleExpose(const XExposeEvent &event) override;

    public:
        void run() override;
    };
}

#endif //X11TEST_TESTAPP_H
