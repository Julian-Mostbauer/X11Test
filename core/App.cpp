//
// Created by julian on 9/22/25.
//

#include "App.h"

#include <format>
#include <iostream>
#include <climits>
#include <ranges>
#include <stdexcept>

#define QUIT_EARLY_WITH_DEBUG_TRAP(ASSERTION, MSG, ...)  if (ASSERTION) return debug_trap(MSG, __VA_ARGS__); // silently ignore



#define REQUIRE_WINDOW(WIN_ID, MSG)  if (!windowCheckOpen(WIN_ID)) throw std::runtime_error(MSG);

// https://www.mankier.com/
namespace X11App {
    // |*********************************************|
    // |               Window Management             |
    // |*********************************************|

    void App::windowOpen(const int winId, const PixelPos x, const PixelPos y, const PixelPos width,
                         const PixelPos height, const long event_mask,
                         const char *title) {
#if DEBUG
        std::cout << "Creating window ID " << winId << " at (" << x << "," << y << ") with size " << width << "x"
                << height << " titled \"" << title << "\"" << std::endl;
#endif
        if (windowCheckOpen(winId))
            throw std::runtime_error("Window ID already exists");

        const Window window = XCreateSimpleWindow(m_Display, RootWindow(m_Display, m_ScreenId), x, y, width, height, 1,
                                                  BlackPixel(m_Display, m_ScreenId), WhitePixel(m_Display, m_ScreenId));

        XSelectInput(m_Display, window, event_mask);
        XMapWindow(m_Display, window);
        XStoreName(m_Display, window, title);
        XSetWMProtocols(m_Display, window, &m_WM_DELETE_WINDOW_ATOM, 1);

        m_Windows[winId] = window;
    }

    std::future<bool> App::windowOpenAsync(int winId, PixelPos x, PixelPos y, PixelPos width, PixelPos height,
        long event_mask, const char *title) {
        return std::async(std::launch::async,
                          [this, winId, x, y, width, height, event_mask, title] {
                              windowOpen(winId, x, y, width, height, event_mask, title);
                              return true;
                          });
    }

    void App::windowClose(const int winId) noexcept {
        QUIT_EARLY_WITH_DEBUG_TRAP(!windowCheckOpen(winId), "Trying to force close a non-existent window ID %d", winId)

        XDestroyWindow(m_Display, m_Windows[winId]);
        m_Windows.erase(winId);
    }

    std::future<void> App::windowCloseAsync(int winId) noexcept {
        return std::async(std::launch::async, [this, winId] { windowClose(winId); });
    }

    void App::windowClear(const int winId, const bool flush) const noexcept {
        QUIT_EARLY_WITH_DEBUG_TRAP(!windowCheckOpen(winId), "Trying to force clear a non-existent window ID %d", winId)

        XClearWindow(m_Display, m_Windows.at(winId));
        if (flush) XFlush(m_Display);
    }

    void App::windowForceRedraw(const int winId) noexcept {
        QUIT_EARLY_WITH_DEBUG_TRAP(!windowCheckOpen(winId), "Trying to force redraw of non-existent window ID %d",
                                   winId)

        const Window activeWindow = m_Windows.at(winId);

        auto emptyEvent = XExposeEvent{
            .type = Expose, .serial = 0, .send_event = False, .display = m_Display,
            .window = activeWindow, .x = 0, .y = 0, .width = 0, .height = 0, .count = 0
        };

        handleExpose(emptyEvent);
    }

    bool App::windowCheckOpen(const int winId) const noexcept {
        return m_Windows.contains(winId);
    }

    XWindowAttributes App::windowGetAttributes(const int winId) const {
        if (!windowCheckOpen(winId)) throw std::runtime_error("No window with id " + std::to_string(winId) + " exits");

        const Window activeWindow = m_Windows.at(winId);
        XWindowAttributes attrs{};
        XGetWindowAttributes(m_Display, activeWindow, &attrs);

        return attrs;
    }

    std::optional<int> App::windowRawToId(const Window window) const {
        // linear search, because the expected number of windows is low, usually 1-2
        for (const auto &[id, win]: m_Windows) if (win == window) return id;
        return std::nullopt;
    }

    void App::windowProcessRedrawQueue() noexcept {
        while (!m_RedrawQueue.empty()) {
            if (const auto winId = m_RedrawQueue.front(); windowCheckOpen(winId)) {
                windowClear(winId, true);
                windowForceRedraw(winId);
            }
            m_RedrawQueue.pop();
        };
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
        REQUIRE_WINDOW(winId, "Attempting to draw a rectangle on a non-existent window ID " + std::to_string(winId))

        const Window activeWindow = m_Windows.at(winId);

        const GC gc = XCreateGC(m_Display, activeWindow, 0, nullptr);
        XSetForeground(m_Display, gc, color.pixel);
        XFillRectangle(m_Display, activeWindow, gc, x, y, width, height);

        XFreeGC(m_Display, gc);
    }

    void App::drawCircle(const int winId, const XColor &color, const PixelPos x, const PixelPos y,
                         const PixelPos radius) const {
        REQUIRE_WINDOW(winId, "Attempting to draw a circle on a non-existent window ID " + std::to_string(winId))

        const Window activeWindow = m_Windows.at(winId);

        const GC gc = XCreateGC(m_Display, activeWindow, 0, nullptr);
        XSetForeground(m_Display, gc, color.pixel);
        XFillArc(m_Display, activeWindow, gc, x - radius, y - radius, radius * 2, radius * 2, 0, 360 * 64);

        XFreeGC(m_Display, gc);
    }

    void App::drawText(const int winId, const XColor &color, const PixelPos x, const PixelPos y, const str fontStr,
                       const str text) const {
        REQUIRE_WINDOW(winId, "Attempting to draw text on a non-existent window ID " + std::to_string(winId))

        if (text.empty() || text.size() >= INT_MAX) return;
        const Window activeWindow = m_Windows.at(winId);

        const GC gc = XCreateGC(m_Display, activeWindow, 0, nullptr);

        const Font fontObj = XLoadFont(m_Display, fontStr.data());
        XSetFont(m_Display, gc, fontObj);

        XSetForeground(m_Display, gc, color.pixel);
        XDrawString(m_Display, activeWindow, gc, x, y, text.data(), static_cast<int>(text.size()));

        XUnloadFont(m_Display, fontObj);
        XFreeGC(m_Display, gc);
    }

    void App::drawPolygon(const int winId, const XColor &color, std::vector<XPoint> &points) const {
        REQUIRE_WINDOW(winId, "Attempting to draw a polygon on a non-existent window ID " + std::to_string(winId))

        const Window activeWindow = m_Windows.at(winId);
        if (points.size() < 3 || points.size() > INT_MAX)
            throw std::runtime_error(
                "A polygon must have at least 3 points");

        const GC gc = XCreateGC(m_Display, activeWindow, 0, nullptr);
        XSetForeground(m_Display, gc, color.pixel);
        XFillPolygon(m_Display, activeWindow, gc, points.data(), static_cast<int>(points.size()), Convex,
                     CoordModeOrigin);

        XFreeGC(m_Display, gc);
    }

    // |*********************************************|
    // |               Event Handling                |
    // |*********************************************|

    void App::handleAllQueuedEvents() {
        XEvent event;
        while (XPending(m_Display)) {
            XNextEvent(m_Display, &event);
            handleEvent(event);
        }
    }

    void App::handleEvent(XEvent &event) {
        switch (event.type) {
            case Expose: handleExpose(event.xexpose);
                break;
            case KeyPress: handleKeyPress(event.xkey);
                break;
            case KeyRelease: handleKeyRelease(event.xkey);
                break;
            case ButtonPress: handleButtonPress(event.xbutton);
                break;
            case ButtonRelease: handleButtonRelease(event.xbutton);
                break;
            case MotionNotify: handleMotionNotify(event.xmotion);
                break;
            case EnterNotify: handleEnterNotify(event.xcrossing);
                break;
            case LeaveNotify: handleLeaveNotify(event.xcrossing);
                break;
            case FocusIn: handleFocusIn(event.xfocus);
                break;
            case FocusOut: handleFocusOut(event.xfocus);
                break;
            case ClientMessage: handleClientMessage(event.xclient);
                break;
            case MappingNotify: handleMappingNotify(event.xmapping);
                break;
            case ConfigureNotify: handleConfigureNotify(event.xconfigure);
                break;
            case UnmapNotify: handleUnmapNotify(event.xunmap);
                break;
            case MapNotify: handleMapNotify(event.xmap);
                break;
            case DestroyNotify: handleDestroyNotify(event.xdestroywindow);
                break;
            case ReparentNotify: handleReparentNotify(event.xreparent);
                break;
            case PropertyNotify: handlePropertyNotify(event.xproperty);
                break;
            case SelectionClear: handleSelectionClear(event.xselectionclear);
                break;
            case SelectionRequest: handleSelectionRequest(event.xselectionrequest);
                break;
            case SelectionNotify: handleSelectionNotify(event.xselection);
                break;
            case ColormapNotify: handleColormapNotify(event.xcolormap);
                break;
            case VisibilityNotify: handleVisibilityNotify(event.xvisibility);
                break;
            case NoExpose: handleNoExpose(event.xnoexpose);
                break;
            case GraphicsExpose: handleGraphicsExpose(event.xgraphicsexpose);
                break;
            default: debug_trap("Unknown event type received");
                break;
        }
    }

    void App::handleKeyPress(XKeyEvent &event) {
        const KeySym sym = XLookupKeysym(&event, 0);
        m_KeyStateManager.setKeyPressed(sym);
    }

    void App::handleKeyRelease(XKeyEvent &event) {
        const KeySym sym = XLookupKeysym(&event, 0);
        m_KeyStateManager.setKeyReleased(sym);
    }

    void App::handleClientMessage(XClientMessageEvent &event) {
        const auto winId = windowRawToId(event.window);
        if (!winId.has_value() || !windowCheckOpen(winId.value())) return;

        if (event.message_type == m_WM_PROTOCOLS_ATOM) {
            if (static_cast<Atom>(event.data.l[0]) == m_WM_DELETE_WINDOW_ATOM) {
#if DEBUG
                std::cout << "Received WM_DELETE_WINDOW for window ID " << winId.value() << std::endl;
#endif
                windowClose(winId.value());
            }
        }
    }

    // temporary sound playing function using ffplay
    // todo: use a proper sound library
    void App::soundPlayFile(str path) const {
        // check if ffplay is installed
        if (system("which ffplay > /dev/null 2>&1") != 0) {
            std::cerr << "ffplay is not installed. Please install ffmpeg to use sound playback." << std::endl;
            return;
        }

        if (access(path.data(), F_OK) != 0) {
            std::cerr << "Sound file does not exist: " << path << std::endl;
            return;
        }

        const auto command = std::format("ffplay -nodisp -autoexit '{}' </dev/null >/dev/null 2>&1 &", path);

        system(command.c_str());
    }

    // |*********************************************|
    // |                    Key                     |
    // |*********************************************|

    bool App::keyCheckEqual(const XKeyEvent &event, const KeySym XK_Key) {
        return XLookupKeysym(const_cast<XKeyEvent *>(&event), 0) == XK_Key;
    }

    App::~App() {
#if DEBUG
        std::cout << "Cleaning up " << m_Windows.size() << " windows" << std::endl;
#endif
        for (const Window window: m_Windows | std::views::values) XDestroyWindow(m_Display, window);
        if (m_Display) XCloseDisplay(m_Display);
    }
}
