//
// Created by julian on 9/22/25.
//

#ifndef X11TEST_TESTAPP_H
#define X11TEST_TESTAPP_H

#include "../../core/App.h"
#include "../../core/lib/EventMask.h"


namespace ExampleApp {
    enum TestAppWindows {
        MAIN_WINDOW = 1
    };

    constexpr int gridWidth = 20;
    constexpr int gridHeight = 20;
    constexpr int stepIntervalMs = 250;

    class ExampleApp final : public X11App::App {
        friend App;

        bool grid[gridWidth][gridHeight];
        bool running;
        bool isPaused;

        std::vector<XPoint> polygonPoints;
        const long defaultMask;
        const std::string defaultFont;

        explicit ExampleApp(Display *display)
            : App(display), running(true), isPaused(true), polygonPoints({}), defaultMask(
                  X11App::EventMask().useExposureMask().useKeyPressMask().useKeyReleaseMask().
                  useButtonPressMask().mask), defaultFont(X11App::FontDescriptor("helvetica", 150).toString()) {
            std::fill_n(&grid[0][0], gridWidth * gridHeight, false);
        }

        void handleExpose(XExposeEvent &event) override;

        void handleButtonPress(XButtonEvent &event) override;

        void gridStep();
    public:
        void run() override;
    };
}

#endif //X11TEST_TESTAPP_H
