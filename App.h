//
// Created by julian on 9/22/25.
//

#ifndef X11TEST_APP_H
#define X11TEST_APP_H
#include <map>
#include <stdexcept>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <vector>

using u16 = unsigned short;

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
        void windowOpen(int winId, int x, int y, int width, int height, const std::string &title);

        void windowClose(int winId);

        [[nodiscard]] bool windowCheckOpen(int winId) const;

        [[nodiscard]] XWindowAttributes windowGetAttributes(int winId) const;


        //|*********************************************|
        //|                  Drawing                    |
        //|*********************************************|

        [[nodiscard]] XColor colorCreate(u16 red, u16 green, u16 blue) const;

        void drawRectangle(int winId, const XColor &color, u16 x, u16 y, u16 width = 1, u16 height = 1) const;

        void drawCircle(int winId, const XColor &color, u16 x, u16 y, u16 radius = 10) const;

        void drawText(int winId, const XColor &color, u16 x, u16 y, const std::string &text) const;

        void drawPolygon(int winId, const XColor &color, std::vector<XPoint> &points) const;


        // |*********************************************|
        // |                    Misc                     |
        // |*********************************************|

        void cleanup() const;

        virtual ~App() {
            cleanup();
        };

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
