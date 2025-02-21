
#pragma once

#include "core/basic_types.hpp"
#include "core/clock.hpp"
#include "gfx/display_window.hpp"
#include "nbody/galaxy_scene.hpp"

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

class SdlInitializer
{
public:
    SdlInitializer();
    ~SdlInitializer();
};

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

class App : Singleton<App>
{
    SdlInitializer _sdlInitializer;
    DisplayWindow  _displayWindow;
    GalaxyScene    _galaxyScene;  // Cannot outlive _renderer

    Clock::time_point _startTime{};
    Clock::time_point _lastTickTime{};
    uint64_t          _tickCount     = 0;
    bool              _quitRequested = false;
    // std::bitset<SDL_NUM_SCANCODES> _pressedKeyScancodes;

public:
    App();
    ~App();
    void run();

private:
    void onTick();
};

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
