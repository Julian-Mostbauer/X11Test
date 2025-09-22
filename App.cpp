//
// Created by julian on 9/22/25.
//

#include "App.h"

#include <iostream>
#include <ranges>
#include <stdexcept>

void App::openWindow(const int winId, const int x, const int y, const int width, const int height,
                     const char *title) {
#if DEBUG
    std::cout << "Creating window ID " << winId << " at (" << x << "," << y << ") with size " << width << "x"
            << height << " titled \"" << title << "\"" << std::endl;
#endif
    const Window window = XCreateSimpleWindow(m_Display, RootWindow(m_Display, m_ScreenId), x, y, width, height, 1,
                                              BlackPixel(m_Display, m_ScreenId), WhitePixel(m_Display, m_ScreenId));

    XSelectInput(m_Display, window, ExposureMask | KeyPressMask);
    XMapWindow(m_Display, window);
    XStoreName(m_Display, window, title);

    if (m_Windows.contains(winId))
        throw std::runtime_error("Window ID already exists");

    m_Windows[winId] = (window);
}

void App::closeWindow(const int winId) {
#if DEBUG
    std::cout << "Closing window ID " << winId << std::endl;
    if (!m_Windows.contains(winId))
        throw std::runtime_error("Window ID does not exist");
#endif
    // silently ignore
    if (m_Windows.contains(winId)) {
        XDestroyWindow(m_Display, m_Windows[winId]);
        m_Windows.erase(winId);
    }
}

bool App::isWindowOpen(const int winId) const {
    return m_Windows.contains(winId);
}

bool App::isKey(const XKeyEvent &event, const KeySym XK_Key) {
    return XLookupKeysym(const_cast<XKeyEvent*>(&event), 0) == XK_Key;
}



void App::cleanup() const {
# if DEBUG
    std::cout << "Cleaning up " << m_Windows.size() << " windows" << std::endl;
# endif
    for (const Window window: m_Windows | std::views::values) XDestroyWindow(m_Display, window);
    if (m_Display) XCloseDisplay(m_Display);
}
