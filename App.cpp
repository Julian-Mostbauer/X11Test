//
// Created by julian on 9/22/25.
//

#include "App.h"

#include <format>
#include <iostream>
#include <ranges>
#include <stdexcept>

namespace X11App {
    void App::windowOpen(const int winId, const int x, const int y, const int width, const int height,
                         const char *title) {
#if DEBUG
        std::cout << "Creating window ID " << winId << " at (" << x << "," << y << ") with size " << width << "x"
                << height << " titled \"" << title << "\"" << std::endl;
#endif
        if (m_Windows.contains(winId))
            throw std::runtime_error("Window ID already exists");

        const Window window = XCreateSimpleWindow(m_Display, RootWindow(m_Display, m_ScreenId), x, y, width, height, 1,
                                                  BlackPixel(m_Display, m_ScreenId), WhitePixel(m_Display, m_ScreenId));

        XSelectInput(m_Display, window, ExposureMask | KeyPressMask);
        XMapWindow(m_Display, window);
        XStoreName(m_Display, window, title);

        m_Windows[winId] = (window);
    }

    void App::windowClose(const int winId) {
#if DEBUG
        if (!m_Windows.contains(winId)) {
            std::cout << "Trying to close non-existing window ID " << winId << std::endl;
            __builtin_trap();
        }
#endif
        // silently ignore
        if (m_Windows.contains(winId)) {
            XDestroyWindow(m_Display, m_Windows[winId]);
            m_Windows.erase(winId);
        }
    }

    bool App::windowCheckOpen(const int winId) const {
        return m_Windows.contains(winId);
    }

    bool App::keyCheckEqual(const XKeyEvent &event, const KeySym XK_Key) {
        return XLookupKeysym(const_cast<XKeyEvent *>(&event), 0) == XK_Key;
    }

    XWindowAttributes App::windowGetAttributes(const int winId) const {
        const Window activeWindow = m_Windows.at(winId);
        XWindowAttributes attrs{};
        XGetWindowAttributes(m_Display, activeWindow, &attrs);

        return attrs;
    }

    void App::drawRectangle(const int winId, const int x, const int y, const XColor color, const int width,
                            const int height) const {
#if DEBUG
        const auto attrs = windowGetAttributes(winId);

        if (width <= 0 || height <= 0 || x + width >= attrs.width || y + height >= attrs.height) {
            std::cout << "Trying to draw rectangle outside of window(id:" << winId
                    << ") bounds:\nWindow size is " << attrs.width << "x" << attrs.height
                    << ", trying to draw rectangle at (" << x << "," << y << ") with size "
                    << width << "x" << height << std::endl;
            __builtin_trap();
        }
#endif
        if (!m_Windows.contains(winId))
            throw std::runtime_error("Window ID does not exist");

        const Window activeWindow = m_Windows.at(winId);
        GC gc = XCreateGC(m_Display, activeWindow, 0, nullptr);
        XSetForeground(m_Display, gc, color.pixel);
        XFillRectangle(m_Display, activeWindow, gc, x, y, width, height);
        XFreeGC(m_Display, gc);
    }

    XColor App::colorCreate(const unsigned short red, const unsigned short green, const unsigned short blue) const {
        XColor color{};
        color.red = red;
        color.green = green;
        color.blue = blue;
        color.flags = DoRed | DoGreen | DoBlue;
        if (!XAllocColor(m_Display, DefaultColormap(m_Display, m_ScreenId), &color))
            throw std::runtime_error("Failed to allocate color");
        return color;
    }


    void App::cleanup() const {
# if DEBUG
        std::cout << "Cleaning up " << m_Windows.size() << " windows" << std::endl;
# endif
        for (const Window window: m_Windows | std::views::values) XDestroyWindow(m_Display, window);
        if (m_Display) XCloseDisplay(m_Display);
    }
}
