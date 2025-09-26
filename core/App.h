//
// Created by julian on 9/22/25.
//

#ifndef X11TEST_APP_H
#define X11TEST_APP_H

#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "FontDescriptor.h"

using u16 = unsigned short;
using PixelPos = unsigned short;
using str = std::string;

namespace X11App {
    class App {
    protected:
        Display *m_Display = nullptr;
        std::map<int, Window> m_Windows;
        int m_ScreenId;

        explicit App(Display *display) : m_Display(display), m_Windows({}),
                                         m_ScreenId(DefaultScreen(display)) {
        }

        // |*********************************************|
        // |               Window Management             |
        // |*********************************************|
        void windowOpen(int winId, PixelPos x, PixelPos y, PixelPos width, PixelPos height, long event_mask,
                        const char *title);

        void windowClose(int winId) noexcept;

        void windowClear(int winId, bool flush) const;

        [[nodiscard]] bool windowCheckOpen(int winId) const;

        [[nodiscard]] XWindowAttributes windowGetAttributes(int winId) const;


        //|*********************************************|
        //|                  Drawing                    |
        //|*********************************************|

        [[nodiscard]] XColor colorCreate(u16 red, u16 green, u16 blue) const;

        void drawRectangle(int winId, const XColor &color, PixelPos x, PixelPos y, PixelPos width = 1,
                           PixelPos height = 1) const;

        void drawCircle(int winId, const XColor &color, PixelPos x, PixelPos y, PixelPos radius = 10) const;

        void drawText(int winId, const XColor &color, PixelPos x, PixelPos y, const FontDescriptor &fontDescriptor,
                      const str &text) const;

        void drawText(int winId, const XColor &color, PixelPos x, PixelPos y, const str &fontStr,
                      const str &text) const;

        void drawPolygon(int winId, const XColor &color, std::vector<XPoint> &points) const;

        // |*********************************************|
        // |                Event Handling               |
        // |*********************************************|
        void handleEvent(const XEvent &event);

        /// Template macro to define event handler functions for X11 events. Default implementation simply calls debug_trap.
        /// @param EVENT_NAME
        /// @param TYPE_NAME
#define HANDLE_EVENT_FUNC_TEMPLATE(EVENT_NAME, TYPE_NAME) \
    virtual void handle##EVENT_NAME(const X##TYPE_NAME##Event &event) { \
    /*Avoid unsued param warning*/ (void)event;\
    debug_trap("Unhandled " #EVENT_NAME " event for window");\
    };

        HANDLE_EVENT_FUNC_TEMPLATE(Expose, Expose)
        HANDLE_EVENT_FUNC_TEMPLATE(KeyPress, Key)
        HANDLE_EVENT_FUNC_TEMPLATE(KeyRelease, Key)
        HANDLE_EVENT_FUNC_TEMPLATE(ButtonPress, Button)
        HANDLE_EVENT_FUNC_TEMPLATE(ButtonRelease, Button)
        HANDLE_EVENT_FUNC_TEMPLATE(MotionNotify, Motion)
        HANDLE_EVENT_FUNC_TEMPLATE(EnterNotify, EnterWindow)
        HANDLE_EVENT_FUNC_TEMPLATE(LeaveNotify, LeaveWindow)
        HANDLE_EVENT_FUNC_TEMPLATE(FocusIn, FocusIn)
        HANDLE_EVENT_FUNC_TEMPLATE(FocusOut, FocusOut)
        HANDLE_EVENT_FUNC_TEMPLATE(ClientMessage, ClientMessage)
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

        // |*********************************************|
        // |                    Misc                     |
        // |*********************************************|

        virtual ~App();

        [[nodiscard]] static bool keyCheckEqual(const XKeyEvent &event, KeySym XK_Key);

    private:
        // |*********************************************|
        // |               Helper Functions              |
        // |*********************************************|

        /**
         * Helper function to validate drawing arguments:
         *
         * - NORMAL: will throw an exception if winId is invalid
         *
         * - DEBUG: will print error message and trap if drawing outside of window bounds
         *
         * @param winId ID of the window to draw on
         * @param x1 Top left X position
         * @param y1 Top left Y position
         * @param x2 Bottom right X position
         * @param y2 Bottom right Y position
         */
        void helperValidateDrawingArgs(int winId, u16 x1, u16 y1, u16 x2, u16 y2) const;

        /// Triggers a debug trap if DEBUG is defined.
        /// @param message Optional message to print before trapping.
        static void debug_trap(const char *message = nullptr) {
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
        static void debug_trap(const char *format, Args &&... args) {
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
        /**
         * A static factory method to create an instance of a class derived from App. Creates and opens a connection to the X server.
         *
         * @tparam TDerived The type of the derived class that inherits from App. This class must also friend App.
         * @return A pointer to the newly created instance of TDerived.
         * @throws std::runtime_error if the display cannot be opened.
         */
        template<class TDerived>
        static App *Create() {
            static_assert(std::is_base_of_v<App, TDerived>, "Type TDerived must derive from App");

            Display *display = XOpenDisplay(nullptr);
            if (!display) throw std::runtime_error("Cannot open display");

            return new TDerived(display);
        }

        virtual void run() = 0;
    };
}

#endif //X11TEST_APP_H
