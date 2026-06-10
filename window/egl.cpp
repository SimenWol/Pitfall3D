#include "egl.h"

#include <cstdio>
#include <X11/Xlib.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

int InitEgl(Display* display, Window& window, EGLDisplay& eglDisplay, EGLSurface& eglSurface, EGLContext& eglContext)
{
	// Get EGLDisplay
	eglDisplay = eglGetDisplay((EGLNativeDisplayType)display);
	if (eglDisplay == EGL_NO_DISPLAY)
	{
		printf("Unable to get an EGL Display.\n");
		return 0;
	}

	// Initialize EGL.
	EGLint eglVersionMajor, eglVersionMinor;
	if (!eglInitialize(eglDisplay, &eglVersionMajor, &eglVersionMinor))
	{
		printf("Unable to initialize EGL.\n");
		return 0;
	}

	// Define what kind of EGL config we want
	const EGLint attributeList[] =
	{
		EGL_RED_SIZE, 8,	// 8-bit
		EGL_GREEN_SIZE, 8,	// 8-bit
		EGL_BLUE_SIZE, 8,	// 8-bit
		EGL_ALPHA_SIZE, 8,	// 8-bit (possibly change to 0?)
		EGL_DEPTH_SIZE, 8,	// 8-bit
		EGL_SURFACE_TYPE,
		EGL_WINDOW_BIT,
	#ifdef GLES3
		EGL_CONFORMANT,
		EGL_OPENGL_ES3_BIT_KHR,
	#endif // GLES3

		EGL_NONE,
	};

	EGLConfig eglConfig;
	EGLint numConfigs;

	// Choose config
	if (!eglChooseConfig(eglDisplay, attributeList, &eglConfig, 1, &numConfigs))
	{
		printf("Failed to choose config. (%d)\n", eglGetError());
		return 0;
	}

	if (numConfigs != 1)
	{
		printf("We got %i configs.\n", numConfigs);
		return 0;
	}

	// Create a surface
	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, window, nullptr);
	if (eglSurface == EGL_NO_SURFACE)
	{
		printf("Unable to create EGL surface. (%d)\n", eglGetError());
		return 0;
	}

	// Create a context
	const EGLint ctxAttr[] = { EGL_CONTEXT_CLIENT_VERSION, 3, /**/ EGL_CONTEXT_MINOR_VERSION, 1, /**/ EGL_NONE};
	eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, ctxAttr);
	if (eglContext == EGL_NO_CONTEXT)
	{
		printf("Unable to create EGL context. (%d)\n", eglGetError());
		return 0;
	}

	// Make context current
	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

#ifdef _DEBUG
	printf("EGL %s ~ %s\n", eglQueryString(eglDisplay, EGL_VERSION), glGetString(GL_VERSION)); // Print versions
#endif // DEBUG

	return 1;
}

int VsyncEgl(EGLDisplay& eglDisplay, const bool yes)
{
	// V-sync
	if (!eglSwapInterval(eglDisplay, yes ? 1 : 0))
	{
		printf("Warning: eglSwapInterval failed: %d\n", eglGetError());
		return 0;
	}
	return 1;
}

void CleanEgl(EGLDisplay& display, EGLContext& context, EGLSurface& surface)
{
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(display, context);
	eglDestroySurface(display, surface);
	eglTerminate(display);
}