#pragma once
#include <X11/Xlib.h>

Window InitX11Window(Display* display, int x, int y, const int width, const int height, const char* title);

void CleanX11(Display* display, Window& window);