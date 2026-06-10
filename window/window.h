#pragma once
#include <X11/Xlib.h>
#include <EGL/egl.h>

class GameWindow
{
public:
	GameWindow() = default;
	~GameWindow() = default;

	int Init(const int width, const int height, const char* title);
	int Vsync(const bool yes);
	void SwapBuffers() const;
	void GetSize(int& width, int& height) const;

	// X11
	Window xWin = 0;
	Display* xDisplay;

	// EGL
	EGLDisplay eglDisplay = nullptr;
	EGLSurface eglSurface = nullptr;
	EGLContext eglContext = nullptr;

	bool open = false;
};