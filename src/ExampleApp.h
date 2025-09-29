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

    class KeyStateManager {
        std::unordered_map<KeySym, bool> pressedKeys;
        std::unordered_map<KeySym, bool> prevState;

    public:
        void reserve(const size_t size) { pressedKeys.reserve(size); prevState.reserve(size); }

        void setKeyPressed(const KeySym key) { pressedKeys[key] = true; }

        void setKeyReleased(const KeySym key) { pressedKeys[key] = false; }

        // checks if a key is currently held down
        bool isKeyDown(const KeySym key) const { return pressedKeys.contains(key) && pressedKeys.at(key); }

        // checks if a key was pressed (transition from not pressed to pressed)
        bool isKeyPressed(const KeySym key) {
            const bool currentlyDown = isKeyDown(key);
            const bool wasDown = prevState.contains(key) && prevState.at(key);
            prevState[key] = currentlyDown;
            return currentlyDown && !wasDown;
        }

        bool stateChanged() const {
            return pressedKeys != prevState;
        }
    };


    class ExampleApp final : public X11App::App {
        friend App;

        KeyStateManager keyStateManager;
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

        void handleKeyPress(XKeyEvent &event) override;

        void handleKeyRelease(XKeyEvent &event) override;

        void handleButtonPress(XButtonEvent &event) override;

    public:
        void run() override;
    };
}

#endif //X11TEST_TESTAPP_H
