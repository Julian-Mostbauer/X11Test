//
// Created by julian on 9/22/25.
//

#include "App.h"

#include <format>
#include <iostream>
#include <limits.h>
#include <ranges>
#include <stdexcept>


namespace X11App {
    // |*********************************************|
    // |               Window Management             |
    // |*********************************************|

    void App::windowOpen(const int winId, const PixelPos x, const PixelPos y, const PixelPos width,
                         const PixelPos height, const unsigned long event_mask,
                         const char *title) {
#if DEBUG
        std::cout << "Creating window ID " << winId << " at (" << x << "," << y << ") with size " << width << "x"
                << height << " titled \"" << title << "\"" << std::endl;
#endif
        if (m_Windows.contains(winId))
            throw std::runtime_error("Window ID already exists");

        const Window window = XCreateSimpleWindow(m_Display, RootWindow(m_Display, m_ScreenId), x, y, width, height, 1,
                                                  BlackPixel(m_Display, m_ScreenId), WhitePixel(m_Display, m_ScreenId));

        XSelectInput(m_Display, window, event_mask);
        XMapWindow(m_Display, window);
        XStoreName(m_Display, window, title);

        m_Windows[winId] = (window);
    }

    void App::windowClose(const int winId) noexcept {
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

    void App::windowClear(const int winId, const bool flush) const {
        XClearWindow(m_Display, m_Windows.at(winId));
        if (flush) XFlush(m_Display);
    }

    bool App::windowCheckOpen(const int winId) const {
        return m_Windows.contains(winId);
    }

    XWindowAttributes App::windowGetAttributes(const int winId) const {
        const Window activeWindow = m_Windows.at(winId);
        XWindowAttributes attrs{};
        XGetWindowAttributes(m_Display, activeWindow, &attrs);

        return attrs;
    }


    //|*********************************************|
    //|                  Drawing                    |
    //|*********************************************|

    XColor App::colorCreate(const u16 red, const u16 green, const u16 blue) const {
        XColor color{};
        color.red = red;
        color.green = green;
        color.blue = blue;
        color.flags = DoRed | DoGreen | DoBlue;
        if (!XAllocColor(m_Display, DefaultColormap(m_Display, m_ScreenId), &color))
            throw std::runtime_error("Failed to allocate color");
        return color;
    }

    void App::drawRectangle(const int winId, const XColor &color, const PixelPos x, const PixelPos y,
                            const PixelPos width,
                            const PixelPos height) const {
        helperValidateDrawingArgs(winId, x, y, x + width, y + height);
        const Window activeWindow = m_Windows.at(winId);

        GC gc = XCreateGC(m_Display, activeWindow, 0, nullptr);
        XSetForeground(m_Display, gc, color.pixel);
        XFillRectangle(m_Display, activeWindow, gc, x, y, width, height);

        XFreeGC(m_Display, gc);
    }

    void App::drawCircle(const int winId, const XColor &color, const PixelPos x, const PixelPos y,
                         const PixelPos radius) const {
        helperValidateDrawingArgs(winId, x - radius, y - radius, x + radius, y + radius);
        const Window activeWindow = m_Windows.at(winId);

        GC gc = XCreateGC(m_Display, activeWindow, 0, nullptr);
        XSetForeground(m_Display, gc, color.pixel);
        XFillArc(m_Display, activeWindow, gc, x - radius, y - radius, radius * 2, radius * 2, 0, 360 * 64);

        XFreeGC(m_Display, gc);
    }

    void App::drawText(const int winId, const XColor &color, const PixelPos x, const PixelPos y,
                       const FontDescriptor &fontDescriptor,
                       const str &text) const {
        drawText(winId, color, x, y, fontDescriptor.toString(), text);
    }

    void App::drawText(const int winId, const XColor &color, const PixelPos x, const PixelPos y,
                       const str &fontStr,
                       const str &text) const {
        helperValidateDrawingArgs(winId, x, y, x + text.size() * 10, y + 20); // todo: better text size estimation
        if (text.empty() || text.size() >= INT_MAX) return;
        const Window activeWindow = m_Windows.at(winId);

        GC gc = XCreateGC(m_Display, activeWindow, 0, nullptr);

        const Font fontObj = XLoadFont(m_Display, fontStr.data());
        XSetFont(m_Display, gc, fontObj);

        XSetForeground(m_Display, gc, color.pixel);
        XDrawString(m_Display, activeWindow, gc, x, y, text.data(), static_cast<int>(text.size()));

        XUnloadFont(m_Display, fontObj);
        XFreeGC(m_Display, gc);
    }

    void App::drawPolygon(const int winId, const XColor &color, std::vector<XPoint> &points) const {
        helperValidateDrawingArgs(winId, 0, 0, 0, 0);
        const Window activeWindow = m_Windows.at(winId);
        if (points.size() < 3) throw std::runtime_error("A polygon must have at least 3 points");

#if DEBUG
        // todo: look into better handling of validation
        for (const auto &[x, y]: points) {
            helperValidateDrawingArgs(winId, x, y, x, y);
        }
#endif

        GC gc = XCreateGC(m_Display, activeWindow, 0, nullptr);
        XSetForeground(m_Display, gc, color.pixel);
        XFillPolygon(m_Display, activeWindow, gc, points.data(), points.size(), Convex, CoordModeOrigin);

        XFreeGC(m_Display, gc);
    }

    // |*********************************************|
    // |                    Misc                     |
    // |*********************************************|

    void App::cleanup() const {
# if DEBUG
        std::cout << "Cleaning up " << m_Windows.size() << " windows" << std::endl;
# endif
        for (const Window window: m_Windows | std::views::values) XDestroyWindow(m_Display, window);
        if (m_Display) XCloseDisplay(m_Display);
    }


    bool App::keyCheckEqual(const XKeyEvent &event, const KeySym XK_Key) {
        return XLookupKeysym(const_cast<XKeyEvent *>(&event), 0) == XK_Key;
    }

    // |*********************************************|
    // |               Helper Functions              |
    // |*********************************************|

    void App::helperValidateDrawingArgs(const int winId, const PixelPos x1, const PixelPos y1, const PixelPos x2,
                                        const PixelPos y2) const {
        if (!m_Windows.contains(winId))
            throw std::runtime_error("Window ID does not exist: " + std::to_string(winId));
#if DEBUG
        if (const auto attrs = windowGetAttributes(winId);
            x1 > attrs.width || y1 > attrs.height || x2 > attrs.width || y2 > attrs.height) {
            std::cout << "WARNING!!!\nTrying to draw outside of window(id:" << winId
                    << ") bounds:\nWindow size is " << attrs.width << "x" << attrs.height
                    << ", trying to draw between (" << x1 << "," << y1 << ") and "
                    << "(" << x2 << "," << y2 << ")"
                    << std::endl;
            std::cout << "Trap call at: Line" << __LINE__ + 1 << " in " << __FILE__ << std::endl;
            __builtin_trap();
        }
#endif
    }
}
