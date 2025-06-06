/*
    MIT License
    Copyright (c) 2025 Mariusz Łapiński

      ▄█     ▄████████    ▄████████    ▄▄▄▄███▄▄▄▄      ▄████████    ▄████████  ▄█   ▄██████▄  ███▄▄▄▄
      ███    ███    ███   ███    ███  ▄██▀▀▀███▀▀▀██▄   ███    ███   ███    ███ ███  ███    ███ ███▀▀▀██▄
      ███▌   ███    █▀    ███    ███  ███   ███   ███   ███    █▀    ███    ███ ███▌ ███    ███ ███   ███
      ███▌   ███          ███    ███  ███   ███   ███  ▄███▄▄▄      ▄███▄▄▄▄██▀ ███▌ ███    ███ ███   ███
      ███▌ ▀███████████ ▀███████████  ███   ███   ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   ███▌ ███    ███ ███   ███
      ███           ███   ███    ███  ███   ███   ███   ███    █▄  ▀███████████ ███  ███    ███ ███   ███
      ███     ▄█    ███   ███    ███  ███   ███   ███   ███    ███   ███    ███ ███  ███    ███ ███   ███
      █▀    ▄████████▀    ███    █▀    ▀█   ███   █▀    ██████████   ███    ███ █▀    ▀██████▀   ▀█   █▀
                                                                    ███    ███
*/

#include "gfx/display_window.hpp"

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

#ifdef __EMSCRIPTEN__
EM_JS(int, jsGetCanvasWidth, (), { return Module.canvas.width; });
EM_JS(int, jsGetCanvasHeight, (), { return Module.canvas.height; });
#endif

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

DisplayWindow::DisplayWindow(Options options)
    : _options{std::move(options)}
    , _screenSize{_options.screenSize}
{
#ifdef __EMSCRIPTEN__
    // In web browser, always maximize the window to the whole canvas.
    _screenSize = ivec2{jsGetCanvasWidth(), jsGetCanvasHeight()};
#endif

    // Create OpenGL-compatible Window; loads the library.
    //
    _window = SDL_CreateWindow(_options.windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _screenSize.x, _screenSize.y, SDL_WINDOW_OPENGL);
    if (_window == nullptr) {
        throwSdlError();
    }

    // Create OpenGL context and activate the Window as the render target.
    //
    _glContext = SDL_GL_CreateContext(_window);
    if (_glContext == nullptr) {
        throwSdlError();
    }

    // Initialize GLEW; loads OpenGL API entry points.
    //
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();

    if (glewError != GLEW_OK) {
        throw std::runtime_error(std::string{"Unable to initialize OpenGL Extension Wrangler Library: glewInit() failed: "} + reinterpret_cast<const char*>(glewGetErrorString(glewError)));
    }

    // Disable V-sync: The rendering time is neglible compared to the physics time, so it is better not to block on swapping/presenting the render target.
    SDL_GL_SetSwapInterval(0);

    // Enable Z-buffer.
    glEnable(GL_DEPTH_TEST);

    // Setup blending for transparency.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
}

DisplayWindow::~DisplayWindow()
{
    if (_glContext != nullptr) {
        SDL_GL_DeleteContext(_glContext);
    }
    if (_window != nullptr) {
        SDL_DestroyWindow(_window);
    }
}

void DisplayWindow::startFrame()
{
#ifdef __EMSCRIPTEN__
    // Update the screen size at the beginning of each frame. It is the best way to support glitchless resizing of the browser window or changing display orientation on the handheld device.
    _screenSize = ivec2{jsGetCanvasWidth(), jsGetCanvasHeight()};
    glViewport(0, 0, _screenSize.x, _screenSize.y);
#endif
}

void DisplayWindow::endFrame() { SDL_GL_SwapWindow(_window); }

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
