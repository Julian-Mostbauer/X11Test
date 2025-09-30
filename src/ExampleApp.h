//
// Created by julian on 9/22/25.
//

#ifndef X11TEST_TESTAPP_H
#define X11TEST_TESTAPP_H
#include <unordered_map>
#include <unordered_set>

#include "../core/App.h"
#include "../core/EventMask.h"


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

        Player player;
        bool running;

        std::vector<XPoint> polygonPoints;
        const long defaultMask;
        const std::string defaultFont;

        explicit ExampleApp(Display *display)
            : App(display), player({100, 100}, 20, 10), running(true), polygonPoints({}), defaultMask(
                  X11App::EventMask().useExposureMask().useKeyPressMask().useKeyReleaseMask().
                  useButtonPressMask().mask), defaultFont(X11App::FontDescriptor("helvetica", 150).toString()) {
        }

        bool updatePlayer();

        void updatePopupMenu();

        void handleExpose(XExposeEvent &event) override;

        void handleButtonPress(XButtonEvent &event) override;

    public:
        void run() override;
    };
}

#endif //X11TEST_TESTAPP_H
