
#pragma once

#include "core/basic_types.hpp"

class DisplayWindow
{
public:
    struct Options {
        std::string windowTitle;
        ivec2       screenSize = ivec2{1024, 768};
    };

private:
    const Options _options;
    SDL_Window*   _window = nullptr;
    SDL_GLContext _glContext{};
    ivec2         _screenSize;

public:
    DisplayWindow(Options options);
    ~DisplayWindow();

    SDL_GLContext glContext() const { return _glContext; }
    ivec2         screenSize() const { return _screenSize; }

    void startFrame();
    void endFrame();

private:
    // ...
};
