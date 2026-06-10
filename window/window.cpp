#include "window.h"

#include "x11.h"
#include "egl.h"

#include <cstdio>
#include <X11/Xlib.h>
#include <EGL/egl.h>

int GameWindow::Init(const int width, const int height, const char* title)
{
	xDisplay = XOpenDisplay(nullptr);
	if (xDisplay == nullptr)
	{
		printf("Unable to open an XDisplay.\n");
		return 0;
	}

	xWin = InitX11Window(xDisplay, 0, 0, width, height, title);

	// Initialize EGL context
	if (!InitEgl(xDisplay, xWin, eglDisplay, eglSurface, eglContext))
	{
		printf("Error creating EGL context.\n");
		return 0;
	}

	open = true;
	return 1;
}

int GameWindow::Vsync(const bool yes)
{
	if (open == false) { return 0; }
	return VsyncEgl(eglDisplay, yes);
}

void GameWindow::SwapBuffers() const
{
	if (open == false) { return; }
	eglSwapBuffers(eglDisplay, eglSurface);
}

void GameWindow::GetSize(int& width, int& height) const
{
	if (open == false) { return; }
	XWindowAttributes gwa;
	XGetWindowAttributes(xDisplay, xWin, &gwa);

	width = gwa.width;
	height = gwa.height;
}