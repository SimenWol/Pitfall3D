#include "x11.h"

#include <cstdio>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


Window InitX11Window(Display* display, int x, int y, const int width, const int height, const char* title)
{
	// Get root window
	const Window rootWin = DefaultRootWindow(display);

	XSetWindowAttributes swa;
	// Input events we'd like to receive
	swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask; // FocusChange?

	// Create window
	const Window xWin = XCreateWindow(display, rootWin, 0, 0, width, height, 0, CopyFromParent, InputOutput, nullptr, CWEventMask, &swa);

	if (xWin == 0)
	{
		printf("Unable to create window.\n");
	}

	// Window hints
	XWMHints hints;
	hints.input = True;
	hints.flags = InputHint;
	XSetWMHints(display, xWin, &hints);

	// Update window title
	XStoreName(display, xWin, title);

	// Show window
	XMapWindow(display, xWin);
	return xWin;
}

void CleanX11(Display* display, Window& window)
{
	XDestroyWindow(display, window);
	XCloseDisplay(display);
}