//
// Created by julian on 9/30/25.
//

#ifndef X11TEST_KEYSTATEMANAGER_H
#define X11TEST_KEYSTATEMANAGER_H
#include <unordered_map>
#include <X11/X.h>


class KeyStateManager {
    std::unordered_map<KeySym, bool> pressedKeys{};
    std::unordered_map<KeySym, bool> prevState{};

public:
    void reserve(const size_t size) { pressedKeys.reserve(size); prevState.reserve(size); }

    /// @param key The KeySym to mark as pressed.
    void setKeyPressed(const KeySym key) { pressedKeys[key] = true; }

    /// @param key The KeySym to mark as released.
    void setKeyReleased(const KeySym key) { pressedKeys[key] = false; }

    /// @param key The KeySym to check.
    /// @return True if the key is currently held down, false otherwise.
    bool isKeyDown(const KeySym key) const { return pressedKeys.contains(key) && pressedKeys.at(key); }

    /// @param key The KeySym to check.
    /// @return True if the key was just pressed (transitioned from up to down), false otherwise.
    bool isKeyPressed(const KeySym key) {
        const bool currentlyDown = isKeyDown(key);
        const bool wasDown = prevState.contains(key) && prevState.at(key);
        prevState[key] = currentlyDown;
        return currentlyDown && !wasDown;
    }

    /// @return True if the current state and the previous state differ, false otherwise.
    bool stateChanged() const {
        return pressedKeys != prevState;
    }
};


#endif //X11TEST_KEYSTATEMANAGER_H