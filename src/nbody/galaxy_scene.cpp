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

#include "nbody/galaxy_scene.hpp"

#include "core/clock.hpp"
#include "gfx/display_window.hpp"

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

#ifdef __EMSCRIPTEN__

void respawnScenario() { GalaxyScene::get().spawnScenario(); }

EMSCRIPTEN_BINDINGS(Isamerion) { function("respawnScenario", &respawnScenario); }

#endif

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

GalaxyScene::GalaxyScene(DisplayWindow& displayWindow)
    : _displayWindow{displayWindow}
    , _galaxyRenderer{_displayWindow}
    , _sim{}
{
    spawnScenario();
}

GalaxyScene::~GalaxyScene() {}

void GalaxyScene::spawnScenario(int scenarioId)
{
    vector<NBodySim::Body> bodies;

    switch (scenarioId) {
        case 0: {
            bodies.clear();
            bodies.reserve(128);

            static std::mt19937                   re(0);
            std::uniform_real_distribution<float> radiusDis(1.5f, 5.0f);
            std::uniform_real_distribution<float> velDis(-1.0f, 1.0f);
            std::uniform_real_distribution<float> massDis(0.01f, 0.5f);

            bodies.push_back(NBodySim::Body{
                .pos  = vec3{0.0f, 0.0f, 0.0f},
                .vel  = vec3{0.0f, 0.0f, 0.0f},
                .mass = 5.0f,
            });

            const int bc = 127;
            for (int i = 0; i < bc; ++i) {
                const float alpha  = (float)i * 2.0f * glm::pi<float>() / (float)bc;
                const float radius = radiusDis(re);
                const float mass   = massDis(re) * massDis(re) * massDis(re) / radius;

                NBodySim::Body body{
                    .pos  = radius * vec3{cos(alpha), 0.0f, sin(alpha)},
                    .vel  = 1.0f * vec3{-sin(alpha), 0.5f * velDis(re), cos(alpha)},
                    .mass = mass,
                };

                bodies.push_back(std::move(body));
            }

            break;
        }

        default: {
            throw std::runtime_error("Unsupported scenario ID:" + std::to_string(scenarioId));
        }
    }

    _sim.respawn(std::move(bodies));
    regenerateStarSizesAndColors();
}

void GalaxyScene::onTick(uint64_t tickCount, float dt)
{
    if (tickCount != 0) {
        _sim.step(dt);
    }

    static vector<vec3> particlePositions;
    particlePositions.clear();
    particlePositions.reserve(_sim._bodies.size());
    for (const auto& body : _sim._bodies) {
        particlePositions.push_back(body.pos);
    }

    _galaxyRenderer.updateParticlePositions(particlePositions);

    {
        constexpr float sonarPulseTimeWrap = 5.0f;
        const float     ltd                = std::fmod(_sim.simTime(), sonarPulseTimeWrap) * _sim.LightSpeed;
        _galaxyRenderer.setSonarRadius(ltd);
    }

    {
        _displayWindow.startFrame();
        _galaxyRenderer.draw();
        _displayWindow.endFrame();
    }
}

bool GalaxyScene::handleEvent(const SDL_Event& generalEvent)
{
    switch (generalEvent.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            handleKeyboardEvent(reinterpret_cast<const SDL_KeyboardEvent&>(generalEvent));
            return true;
    }
    return false;
}

void GalaxyScene::handleKeyboardEvent(const SDL_KeyboardEvent& keyboardEvent)
{
    if (keyboardEvent.keysym.scancode == SDL_SCANCODE_R) {
        spawnScenario();
    }
}

void GalaxyScene::regenerateStarSizesAndColors()
{
    constexpr float BodyDensity = 1.0f;

    const vec3 bluePoint{111 / 255.0f, 140 / 255.0f, 199 / 255.0f};
    const vec3 redPoint{255 / 255.0f, 255 / 255.0f, 0 / 255.0f};
    const vec3 yellowPoint{189 / 255.0f, 57 / 255.0f, 54 / 255.0f};

    vector<float> particleSizes;
    vector<vec3>  particleColors;
    particleSizes.reserve(_sim._bodies.size());
    particleColors.reserve(_sim._bodies.size());

    static std::mt19937                   re(0);
    std::uniform_real_distribution<float> uniformDis(0.0f, 1.0f);

    for (const auto& body : _sim._bodies) {
        const float volume = body.mass / BodyDensity;
        const float radius = std::cbrt(3.0f / (4.0f * glm::pi<float>()) * volume);
        particleSizes.push_back(radius);

        const float alpha     = uniformDis(re);
        const float beta      = uniformDis(re);
        const vec3  starColor = (bluePoint * (1.0f - alpha) + redPoint * alpha) * (1.0f - beta) + yellowPoint * beta;

        particleColors.push_back(starColor);
    }
    _galaxyRenderer.updateParticleSizes(particleSizes);
    _galaxyRenderer.updateParticleColors(particleColors);
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
