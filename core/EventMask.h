//
// Created by julian on 9/25/25.
//

#ifndef X11TEST_EVENTMASK_H
#define X11TEST_EVENTMASK_H

/// Macro meant for use in this file exclusively! Generates a function that enables the flag given as input
#define USE_FUNC_TEMPLATE(FLAG) EventMask &use##FLAG() noexcept {mask |= (FLAG); return *this;}

namespace X11App {
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
    struct EventMask {
        long mask = 0;

        /**
         * Special Implementation for NoEvent, removes any other active masks
         * @return Returns itself to allow method chaining
         */
        EventMask &useNoEvent() {
            mask = NoEventMask;
            return *this;
        }

        USE_FUNC_TEMPLATE(KeyPressMask)
        USE_FUNC_TEMPLATE(KeyReleaseMask)
        USE_FUNC_TEMPLATE(ButtonPressMask)
        USE_FUNC_TEMPLATE(ButtonReleaseMask)
        USE_FUNC_TEMPLATE(EnterWindowMask)
        USE_FUNC_TEMPLATE(LeaveWindowMask)
        USE_FUNC_TEMPLATE(PointerMotionMask)
        USE_FUNC_TEMPLATE(PointerMotionHintMask)
        USE_FUNC_TEMPLATE(Button1MotionMask)
        USE_FUNC_TEMPLATE(Button2MotionMask)
        USE_FUNC_TEMPLATE(Button3MotionMask)
        USE_FUNC_TEMPLATE(Button4MotionMask)
        USE_FUNC_TEMPLATE(Button5MotionMask)
        USE_FUNC_TEMPLATE(ButtonMotionMask)
        USE_FUNC_TEMPLATE(KeymapStateMask)
        USE_FUNC_TEMPLATE(ExposureMask)
        USE_FUNC_TEMPLATE(VisibilityChangeMask)
        USE_FUNC_TEMPLATE(StructureNotifyMask)
        USE_FUNC_TEMPLATE(ResizeRedirectMask)
        USE_FUNC_TEMPLATE(SubstructureNotifyMask)
        USE_FUNC_TEMPLATE(SubstructureRedirectMask)
        USE_FUNC_TEMPLATE(FocusChangeMask)
        USE_FUNC_TEMPLATE(PropertyChangeMask)
        USE_FUNC_TEMPLATE(ColormapChangeMask)
        USE_FUNC_TEMPLATE(OwnerGrabButtonMask)
    };
}

#undef USE_FUNC_TEMPLATE
#endif //X11TEST_EVENTMASK_H
