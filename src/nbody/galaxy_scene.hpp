
#pragma once

#include "core/basic_types.hpp"
#include "nbody/galaxy_renderer.hpp"
#include "nbody/nbody_sim.hpp"

class DisplayWindow;

class GalaxyScene
{
    DisplayWindow& _displayWindow;
    GalaxyRenderer _galaxyRenderer;
    NBodySim       _sim;

public:
    GalaxyScene(DisplayWindow& displayWindow);
    ~GalaxyScene();

    void onTick(float dt);
    void handleEvent(const SDL_Event& generalEvent);

private:
    void handleKeyboardEvent(const SDL_KeyboardEvent& keyboardEvent);
};
