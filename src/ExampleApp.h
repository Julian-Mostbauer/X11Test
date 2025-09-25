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

    class ExampleApp final : public X11App::App {
        friend App;

        explicit ExampleApp(Display *display)
            : App(display) {
        }

        void drawMainWindow() const;

        void drawPopupMenu() const;

        void handleExpose(const XExposeEvent &xexpose);

        void handleKeyPress(const XKeyEvent &xkey);

    public:
        void run() override;
    };
}

#endif //X11TEST_TESTAPP_H
