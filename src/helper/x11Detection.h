//
// Created by julian on 10/8/25.
//

#ifndef X11TEST_X11DETECTION_H
#define X11TEST_X11DETECTION_H

#if defined(__unix__) || defined(__APPLE__)
#include <X11/Xlib.h>

inline bool isX11Installed() {
    if (Display *d = XOpenDisplay(nullptr)) {
        XCloseDisplay(d);
        return true;
    }
    return false;
}
#else
inline bool isX11Installed() {
    return false;
}
#endif

#endif //X11TEST_X11DETECTION_H