//
// Created by julian on 9/25/25.
//

#ifndef X11TEST_EVENTMASK_H
#define X11TEST_EVENTMASK_H

#include <X11/Xlib.h>

namespace X11App {
#undef KeyPress
#undef KeyRelease
#undef ButtonPress
#undef ButtonRelease

    /**
    * - NoEventMask		            No events wanted
    * - KeyPressMask		        Keyboard down events wanted
    * - KeyReleaseMask		        Keyboard up events wanted
    * - ButtonPressMask		        Pointer button down events wanted
    * - ButtonReleaseMask		    Pointer button up events wanted
    * - EnterWindowMask		        Pointer window entry events wanted
    * - LeaveWindowMask		        Pointer window leave events wanted
    * - PointerMotionMask		    Pointer motion events wanted
    * - PointerMotionHintMask	    Pointer motion hints wanted
    * - Button1MotionMask		    Pointer motion while button 1 down
    * - Button2MotionMask		    Pointer motion while button 2 down
    * - Button3MotionMask		    Pointer motion while button 3 down
    * - Button4MotionMask		    Pointer motion while button 4 down
    * - Button5MotionMask		    Pointer motion while button 5 down
    * - ButtonMotionMask		    Pointer motion while any button down
    * - KeymapStateMask		        Keyboard state wanted at window entry and focus in
    * - ExposureMask		        Any exposure wanted
    * - VisibilityChangeMask	    Any change in visibility wanted
    * - StructureNotifyMask		    Any change in window structure wanted
    * - ResizeRedirectMask		    Redirect resize of this window
    * - SubstructureNotifyMask		Substructure notification wanted
    * - SubstructureRedirectMask    Redirect structure requests on children
    * - FocusChangeMask		        Any change in input focus wanted
    * - PropertyChangeMask		    Any change in property wanted
    * - ColormapChangeMask		    Any change in colormap wanted
    * - OwnerGrabButtonMask		    Automatic grabs should activate with owner_events set to True
     */
    class EventMask {
        unsigned long mask = 0;

    public:
        static constexpr unsigned long NoEvent = NoEventMask;
        static constexpr unsigned long KeyPress = KeyPressMask;
        static constexpr unsigned long KeyRelease = KeyReleaseMask;
        static constexpr unsigned long ButtonPress = ButtonPressMask;
        static constexpr unsigned long ButtonRelease = ButtonReleaseMask;
        static constexpr unsigned long EnterWindow = EnterWindowMask;
        static constexpr unsigned long LeaveWindow = LeaveWindowMask;
        static constexpr unsigned long PointerMotion = PointerMotionMask;
        static constexpr unsigned long PointerMotionHint = PointerMotionHintMask;
        static constexpr unsigned long Button1Motion = Button1MotionMask;
        static constexpr unsigned long Button2Motion = Button2MotionMask;
        static constexpr unsigned long Button3Motion = Button3MotionMask;
        static constexpr unsigned long Button4Motion = Button4MotionMask;
        static constexpr unsigned long Button5Motion = Button5MotionMask;
        static constexpr unsigned long ButtonMotion = ButtonMotionMask;
        static constexpr unsigned long KeymapState = KeymapStateMask;
        static constexpr unsigned long Exposure = ExposureMask;
        static constexpr unsigned long VisibilityChange = VisibilityChangeMask;
        static constexpr unsigned long StructureNotify = StructureNotifyMask;
        static constexpr unsigned long ResizeRedirect = ResizeRedirectMask;
        static constexpr unsigned long SubstructureNotify = SubstructureNotifyMask;
        static constexpr unsigned long SubstructureRedirect = SubstructureRedirectMask;
        static constexpr unsigned long FocusChange = FocusChangeMask;
        static constexpr unsigned long PropertyChange = PropertyChangeMask;
        static constexpr unsigned long ColormapChange = ColormapChangeMask;
        static constexpr unsigned long OwnerGrabButton = OwnerGrabButtonMask;


        EventMask &operator|=(const unsigned long other) {
            mask |= other;
            return *this;
        }

        explicit operator unsigned long() const {
            return mask;
        }
    };
}

#endif //X11TEST_EVENTMASK_H
