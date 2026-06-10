#pragma once
#include <X11/Xlib.h>
#include <EGL/egl.h>

int InitEgl(Display* display, Window& window, EGLDisplay& eglDisplay, EGLSurface& eglSurface, EGLContext& eglContext);

int VsyncEgl(EGLDisplay& eglDisplay, const bool yes);

void CleanEgl(EGLDisplay& display, EGLContext& context, EGLSurface& surface);