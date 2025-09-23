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

namespace X11App {
    class App {
    protected:
        Display *m_Display = nullptr;
        std::map<int, Window> m_Windows;
        int m_ScreenId;

        explicit App(Display *display) : m_Display(display), m_Windows({}),
                                         m_ScreenId(DefaultScreen(display)) {
        }

        void cleanup() const;

        virtual ~App() {
            cleanup();
        };

        void windowOpen(int winId, int x, int y, int width, int height, const char *title);

        void windowClose(int winId);

        [[nodiscard]] bool windowCheckOpen(int winId) const;

        [[nodiscard]] static bool keyCheckEqual(const XKeyEvent &event, KeySym XK_Key);

        [[nodiscard]] XWindowAttributes windowGetAttributes(int winId) const;

        void drawRectangle(int winId, int x, int y, XColor color, int width = 1, int height = 1) const;

        [[nodiscard]] XColor colorCreate(unsigned short red, unsigned short green, unsigned short blue) const;

    public:
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
