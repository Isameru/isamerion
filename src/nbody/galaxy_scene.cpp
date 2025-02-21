
#include "nbody/galaxy_scene.hpp"

#include "gfx/display_window.hpp"

GalaxyScene::GalaxyScene(DisplayWindow& displayWindow)
    : _displayWindow{displayWindow}
    , _galaxyRenderer{_displayWindow}
    , _sim{}
{
    vector<float> particleSizes;
    particleSizes.reserve(_sim._bodies.size());
    for (const auto& body : _sim._bodies) {
        const float r = std::cbrt(3.0f * body.mass / (4.0f * glm::pi<float>()));
        particleSizes.push_back(r);
    }
    _galaxyRenderer.updateParticleSizes(particleSizes);
}

GalaxyScene::~GalaxyScene() {}

void GalaxyScene::onTick(float dt)
{
    if (dt > 0) {
        const int ic = 10;
        for (int i = 0; i < ic; ++i) {
            _sim.step(dt / (float)ic);
        }
    }

    {
        static vector<vec3> particlePositions;
        particlePositions.clear();
        particlePositions.reserve(_sim._bodies.size());
        for (const auto& body : _sim._bodies) {
            particlePositions.push_back(body.pos);
        }
        _galaxyRenderer.updateParticlePositions(particlePositions);
    }

    _displayWindow.startFrame();

    _galaxyRenderer.draw();

    _displayWindow.endFrame();
}

void GalaxyScene::handleEvent(const SDL_Event& generalEvent)
{
    switch (generalEvent.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            handleKeyboardEvent(reinterpret_cast<const SDL_KeyboardEvent&>(generalEvent));
            break;
    }
}

void GalaxyScene::handleKeyboardEvent(const SDL_KeyboardEvent& keyboardEvent)
{
    //_pressedKeyScancodes.set(static_cast<size_t>(keyboardEvent.keysym.scancode), keyboardEvent.type == SDL_KEYDOWN);
}
