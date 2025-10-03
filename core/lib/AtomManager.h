//
// Created by julian on 10/3/25.
//

#ifndef X11TEST_ATOMMANAGER_H
#define X11TEST_ATOMMANAGER_H
#include <stdexcept>
#include <X11/X.h>
#include <X11/Xlib.h>


struct AtomManager {
    Atom WM_DELETE_WINDOW_ATOM;
    Atom WM_PROTOCOLS_ATOM;

    explicit AtomManager(Display *display) : WM_DELETE_WINDOW_ATOM(XInternAtom(display, "WM_DELETE_WINDOW", true)),
                                             WM_PROTOCOLS_ATOM(XInternAtom(display, "WM_PROTOCOLS", true)) {
        if (!WM_DELETE_WINDOW_ATOM || !WM_PROTOCOLS_ATOM) throw std::runtime_error("Failed to get an atom");
    }
};

#endif //X11TEST_ATOMMANAGER_H
