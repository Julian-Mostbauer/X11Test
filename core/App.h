//
// Created by julian on 9/22/25.
//

#ifndef X11TEST_APP_H
#define X11TEST_APP_H

#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <stdexcept>
#include <vector>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "FontDescriptor.h"
#include "KeyStateManager.h"

using u16 = unsigned short;
using PixelPos = unsigned short;
using str = std::string_view;

namespace X11App {
    class App {
    protected:
        Display *m_Display = nullptr;
        std::map<int, Window> m_Windows;
        KeyStateManager m_KeyStateManager;
        std::queue<int> m_RedrawQueue;
        Atom m_WM_DELETE_WINDOW_ATOM; // todo: atom management class
        Atom m_WM_PROTOCOLS_ATOM;
        int m_ScreenId;

        explicit App(Display *display) : m_Display(display), m_Windows({}),
                                         m_ScreenId(DefaultScreen(display)) {
            m_WM_DELETE_WINDOW_ATOM = XInternAtom(m_Display, "WM_DELETE_WINDOW", true);
            m_WM_PROTOCOLS_ATOM = XInternAtom(m_Display, "WM_PROTOCOLS", true);

            if (!m_WM_DELETE_WINDOW_ATOM || !m_WM_PROTOCOLS_ATOM)
                throw std::runtime_error("Failed to get WM_DELETE_WINDOW or WM_PROTOCOLS atom");
        }

        // |*********************************************|
        // |               Window Management             |
        // |*********************************************|

        /// Create and open a new window with the specified parameters. If the window ID already exists, throws std::runtime_error.
        /// @param winId The ID of the window to create.
        /// @param x The X position of the top-left corner of the window.
        /// @param y The Y position of the top-left corner of the window.
        /// @param width The width of the window in pixels.
        /// @param height The height of the window in pixels.
        /// @param event_mask The event mask to set for the window. This determines which events the window will receive.
        /// @param title The title of the window.
        void windowOpen(int winId, PixelPos x, PixelPos y, PixelPos width, PixelPos height, long event_mask,
                        const char *title);

        /// WARNING: This function comes with way more overhead than windowOpen. The sync version is way faster.
        /// Asynchronous version of windowOpen. Returns a std::promise that will be set once the window is created.
        /// If the window ID already exists, the promise will be set with an exception.
        /// @param winId The ID of the window to create.
        /// @param x The X position of the top-left corner of the window.
        /// @param y The Y position of the top-left corner of the window.
        /// @param width The width of the window in pixels.
        /// @param height The height of the window in pixels.
        /// @param event_mask The event mask to set for the window. This determines which events the window will receive.
        /// @param title The title of the window.
        /// @return A std::promise that will be set to true once the window is created, or set with an exception if the window ID already exists.
        std::future<bool> windowOpenAsync(int winId, PixelPos x, PixelPos y, PixelPos width, PixelPos height,
                                          long event_mask, const char *title);

        /// Close and destroy the specified window. If the window ID does not exist calls debug_trap.
        /// @param winId The ID of the window to close.
        void windowClose(int winId) noexcept;

        /// WARNING: This function comes with way more overhead than windowClose. The sync version is way faster.
        /// Asynchronous version of windowClose. Returns a std::future that will be set once the window is closed.
        /// @param winId The ID of the window to close.
        /// @return A std::future that will be set once the window is closed.
        std::future<void> windowCloseAsync(int winId) noexcept;

        /// Clear the contents of the specified window. Optionally flush the display after clearing.
        /// @param winId The ID of the window to clear.
        /// @param flush If true, flush the display after clearing. Flushing ensures that the clear operation is sent to the X server immediately.
        void windowClear(int winId, bool flush) const noexcept;

        /// Force a redraw of all windows by calling handleExpose with an empty event.
        /// @param winId The ID of the window to redraw.
        void windowForceRedraw(int winId) noexcept;

        /// Check if a window with the specified ID is currently open.
        /// @param winId The ID of the window to check.
        /// @return True if the window is open, false otherwise.
        [[nodiscard]] bool windowCheckOpen(int winId) const noexcept;

        /// Get the attributes of the specified window.
        /// @param winId The ID of the window to get attributes for.
        /// @return The XWindowAttributes structure containing the window's attributes.
        [[nodiscard]] XWindowAttributes windowGetAttributes(int winId) const;

        /// Get the window ID corresponding to the given raw Window handle.
        /// @param window The raw Window handle to look up.
        /// @return An optional containing the window ID if found, or std::nullopt if not found.
        [[nodiscard]] std::optional<int> windowRawToId(Window window) const;

        /// Schedule a redraw of the specified window by adding it to the redraw queue.
        /// The actual redraw will be processed later when windowProcessRedrawQueue is called.
        /// @param winId The ID of the window to schedule for redraw.
        inline void windowScheduleRedraw(const int winId) noexcept { m_RedrawQueue.push(winId); };

        /// Process all windows in the redraw queue by calling windowClear and windowForceRedraw on each.
        void windowProcessRedrawQueue() noexcept;

        //|*********************************************|
        //|                  Drawing                    |
        //|*********************************************|

        /// Create an XColor from the specified RGB values. Allocates the color in the default colormap.
        /// @param red The red component (0-65535).
        /// @param green The green component (0-65535).
        /// @param blue The blue component (0-65535).
        /// @return The allocated XColor.
        [[nodiscard]] XColor colorCreate(u16 red, u16 green, u16 blue) const;

        /// Draw a filled rectangle on the specified window.
        /// @param winId The ID of the window to draw on.
        /// @param color The color to use for drawing.
        /// @param x The X position of the top-left corner of the rectangle.
        /// @param y The Y position of the top-left corner of the rectangle.
        /// @param width The width of the rectangle in pixels. Default is 1.
        /// @param height The height of the rectangle in pixels. Default is 1.
        void drawRectangle(int winId, const XColor &color, PixelPos x, PixelPos y, PixelPos width = 1,
                           PixelPos height = 1) const;

        /// Draw a filled circle on the specified window.
        /// @param winId The ID of the window to draw on.
        /// @param color The color to use for drawing.
        /// @param x The X position of the center of the circle.
        /// @param y The Y position of the center of the circle.
        /// @param radius The radius of the circle in pixels. Default is 10.
        void drawCircle(int winId, const XColor &color, PixelPos x, PixelPos y, PixelPos radius = 10) const;

        /// Draw text on the specified window using a FontDescriptor.
        /// @param winId The ID of the window to draw on.
        /// @param color The color to use for drawing.
        /// @param x The X position of the top-left corner of the text.
        /// @param y The Y position of the top-left corner of the text.
        /// @param fontStr The X-Logical-Font-Description of the font.
        /// @param text The text to draw.
        void drawText(int winId, const XColor &color, PixelPos x, PixelPos y, str fontStr, str text) const;

        /// Draw a filled polygon on the specified window.
        /// @param winId The ID of the window to draw on.
        /// @param color The color to use for drawing.
        /// @param points A vector of XPoint structures defining the vertices of the polygon.
        void drawPolygon(int winId, const XColor &color, std::vector<XPoint> &points) const;

        // todo: Make it possible to draw images https://stackoverflow.com/questions/6609281/how-to-draw-an-image-from-file-on-window-with-xlib
        // void drawImage(int winId, int x, int y, const str path) const;

        // |*********************************************|
        // |                Event Handling               |
        // |*********************************************|
        /// Process all pending X events by retrieving them from the X server and dispatching them to the appropriate handler functions.
        void handleAllQueuedEvents();

        /// Dispatch the given XEvent to the appropriate handler function based on its type. Uses a non const reference to allow more flexibility in handling.
        /// @param event The XEvent to handle.
        void handleEvent(XEvent &event);

        /// Template macro to define event handler functions for X11 events. Default implementation simply calls debug_trap. Uses a non const reference to allow more flexibility in handling.
        /// @param EVENT_NAME
        /// @param TYPE_NAME
#define HANDLE_EVENT_FUNC_TEMPLATE(EVENT_NAME, TYPE_NAME) \
    virtual void handle##EVENT_NAME(X##TYPE_NAME##Event &event) { \
    /*Avoid unsued param warning*/ (void)event;\
    debug_trap("Unhandled " #EVENT_NAME " event for window");\
    };

        HANDLE_EVENT_FUNC_TEMPLATE(Expose, Expose)
        HANDLE_EVENT_FUNC_TEMPLATE(ButtonPress, Button)
        HANDLE_EVENT_FUNC_TEMPLATE(ButtonRelease, Button)
        HANDLE_EVENT_FUNC_TEMPLATE(MotionNotify, Motion)
        HANDLE_EVENT_FUNC_TEMPLATE(EnterNotify, EnterWindow)
        HANDLE_EVENT_FUNC_TEMPLATE(LeaveNotify, LeaveWindow)
        HANDLE_EVENT_FUNC_TEMPLATE(FocusIn, FocusIn)
        HANDLE_EVENT_FUNC_TEMPLATE(FocusOut, FocusOut)
        HANDLE_EVENT_FUNC_TEMPLATE(MappingNotify, Mapping)
        HANDLE_EVENT_FUNC_TEMPLATE(ConfigureNotify, Configure)
        HANDLE_EVENT_FUNC_TEMPLATE(UnmapNotify, Unmap)
        HANDLE_EVENT_FUNC_TEMPLATE(MapNotify, Map)
        HANDLE_EVENT_FUNC_TEMPLATE(DestroyNotify, DestroyWindow)
        HANDLE_EVENT_FUNC_TEMPLATE(ReparentNotify, Reparent)
        HANDLE_EVENT_FUNC_TEMPLATE(PropertyNotify, Property)
        HANDLE_EVENT_FUNC_TEMPLATE(SelectionClear, SelectionClear)
        HANDLE_EVENT_FUNC_TEMPLATE(SelectionRequest, SelectionRequest)
        HANDLE_EVENT_FUNC_TEMPLATE(SelectionNotify, Selection)
        HANDLE_EVENT_FUNC_TEMPLATE(ColormapNotify, Colormap)
        HANDLE_EVENT_FUNC_TEMPLATE(VisibilityNotify, Visibility)
        HANDLE_EVENT_FUNC_TEMPLATE(NoExpose, NoExpose)
        HANDLE_EVENT_FUNC_TEMPLATE(GraphicsExpose, GraphicsExpose)
#undef HANDLE_EVENT_FUNC_TEMPLATE

        // |****** Handlers with base functionality *****|

        /// Handle a key press event by updating the KeyStateManager. Uses a non const reference to allow more flexibility in handling.
        /// Default implementation marks the key as pressed in the KeyStateManager.
        /// @param event The XKeyEvent to handle.
        virtual void handleKeyPress(XKeyEvent &event);

        /// Handle a key release event by updating the KeyStateManager. Uses a non const reference to allow more flexibility in handling.
        /// Default implementation marks the key as released in the KeyStateManager.
        /// @param event The XKeyEvent to handle.
        virtual void handleKeyRelease(XKeyEvent &event);

        virtual void handleClientMessage(XClientMessageEvent &event);

        // |*********************************************|
        // |                Sound System                 |
        // |*********************************************|

        void soundPlayFile(str path) const;

        // |*********************************************|
        // |                    Key                      |
        // |*********************************************|

        /// Check if the given XKeyEvent corresponds to the specified KeySym.
        /// @param event The XKeyEvent to check.
        /// @param XK_Key The KeySym to compare against (e.g., XK_Escape).
        /// @return True if the event matches the KeySym, false otherwise.
        [[nodiscard]] static bool keyCheckEqual(const XKeyEvent &event, KeySym XK_Key);


        /// Wrapper for m_KeyStateManager.isKeyDown
        /// @param key The KeySym to check.
        /// @return True if the key is currently held down, false otherwise.
        inline bool keyIsDown(const KeySym key) const { return m_KeyStateManager.isKeyDown(key); };

        /// Wrapper for m_KeyStateManager.isKeyPressed. Is not const because of the isKeyPressed implementation
        /// @param key The KeySym to check.
        /// @return True if the key was just pressed (transitioned from up to down), false otherwise.
        inline bool keyIsPressed(const KeySym key) { return m_KeyStateManager.isKeyPressed(key); };

        /// Wrapper for m_KeyStateManager.stateChanged
        /// @return True if the current state and the previous state differ, false otherwise.
        inline bool keyStateChanged() const { return m_KeyStateManager.stateChanged(); };

    private:
        // |*********************************************|
        // |               Helper Functions              |
        // |*********************************************|

        /// Triggers a debug trap if DEBUG is defined.
        /// @param message Optional message to print before trapping.
        static void debug_trap(const char *message [[maybe_unused]] = nullptr) {
#if DEBUG
            if (message) std::cout << message << std::endl;
            __builtin_trap();
#endif
        }

        /// Triggers a debug trap if DEBUG is defined, after formatting a message with printf-style formatting.
        /// @tparam buffSize Size of the internal buffer used for formatting the message. Default is 512.
        /// @tparam Args Variadic template parameters for the format arguments.
        /// @param format The format string, similar to printf.
        /// @param args The arguments to format into the string.
        template<size_t buffSize = 512, typename... Args>
        static void debug_trap(const char *format [[maybe_unused]], Args &&... args [[maybe_unused]]) {
#if DEBUG
            if (format) {
                char buffer[buffSize];
                // does not use std::format so it can be used outside constexpr
                std::snprintf(buffer, sizeof(buffer), format, std::forward<Args>(args)...);
                std::cout << buffer << std::endl;
            }
            __builtin_trap();
#endif
        }

    public:
        virtual ~App();

        /**
         * A static factory method to create an instance of a class derived from App. Creates and opens a connection to the X server.
         *
         * @tparam TDerived The type of the derived class that inherits from App. This class must also friend App.
         * @return A unique_ptr to the created instance of TDerived.
         * @throws std::runtime_error if the display cannot be opened.
         */
        template<class TDerived>
        static std::unique_ptr<App> Create() {
            static_assert(std::is_base_of_v<App, TDerived>, "Type TDerived must derive from App");

            Display *display = XOpenDisplay(nullptr);
            if (!display) throw std::runtime_error("Cannot open display");

            return std::unique_ptr<App>(new TDerived(display));
        }

        /// The main application loop. Must be implemented by derived classes.
        virtual void run() = 0;
    };
}

#endif //X11TEST_APP_H
