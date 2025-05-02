
#include "nbody/galaxy_scene.hpp"

#include "core/clock.hpp"
#include "gfx/display_window.hpp"

GalaxyScene::GalaxyScene(DisplayWindow& displayWindow)
    : _displayWindow{displayWindow}
    , _galaxyRenderer{_displayWindow}
    , _sim{}
{
    vector<float> particleSizes;
    vector<vec3> particleColors;
    particleSizes.reserve(_sim._bodies.size());
    for (const auto& body : _sim._bodies) {
        const float r = std::cbrt(3.0f * body.mass / (4.0f * glm::pi<float>()));
        particleSizes.push_back(r);
        //particleColors.push_back(vec3{1.0f, 0.0f, 0.0f});
        // FC6A03

        const vec3 bluePoint{111 / 255.0f, 140 / 255.0f, 199 / 255.0f};
        const vec3 redPoint{255 / 255.0f, 255 / 255.0f, 0 / 255.0f};
        const vec3 yellowPoint{189 / 255.0f, 57 / 255.0f, 54 / 255.0f};


        static std::mt19937                   re(0);
        std::uniform_real_distribution<float> uniformDis(0.0f, 1.0f);

        const float alpha = uniformDis(re);
        const float beta = uniformDis(re);

        const vec3 starColor = (bluePoint * (1.0f - alpha) + redPoint * alpha) * (1.0f - beta) + yellowPoint * beta;

        particleColors.push_back(starColor);
    }
    _galaxyRenderer.updateParticleSizes(particleSizes);
    _galaxyRenderer.updateParticleColors(particleColors);
}

GalaxyScene::~GalaxyScene() {}

void GalaxyScene::onTick(uint64_t tickCount, float dt)
{
    if (tickCount % 30 == 0) {
        std::cout << "dt: " << dt << std::endl;
    }

    const auto simStartTime = Clock::now();

    if (tickCount == 0) {
        _sim.step(0.0f);
    } else {
        const int ic = 2;
        for (int i = 0; i < ic; ++i) {
            _sim.step(dt / (float)ic);
        }
    }

    const auto simEndTime = Clock::now();
    if (tickCount % 30 == 0) {
        std::cout << "Sim time: " << std::chrono::duration_cast<std::chrono::microseconds>(simEndTime - simStartTime).count() << " us" << std::endl;
    }

    static vector<vec3> particlePositions;
    particlePositions.clear();
    particlePositions.reserve(_sim._bodies.size());
    for (const auto& body : _sim._bodies) {
        particlePositions.push_back(body.pos);
    }

    _galaxyRenderer.updateParticlePositions(particlePositions);

    {
        constexpr float sonarPulseTimeWrap = 10.0f;
        const float     ltd                = std::fmod(_sim.simTime(), sonarPulseTimeWrap) * _sim.LightSpeed;
        _galaxyRenderer.setSonarRadius(ltd);
    }

    const auto updateTime = Clock::now();
    if (tickCount % 30 == 0) {
        std::cout << "Update time: " << std::chrono::duration_cast<std::chrono::microseconds>(updateTime - simEndTime).count() << " us" << std::endl;
    }

    {
        const auto renderStartTime = Clock::now();

        _displayWindow.startFrame();

        _galaxyRenderer.draw();

        _displayWindow.endFrame();

        const auto renderEndTime = Clock::now();

        if (tickCount % 30 == 0) {
            std::cout << "Render time: " << std::chrono::duration_cast<std::chrono::microseconds>(renderEndTime - renderStartTime).count() << " us"
                      << std::endl;
        }
    }
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
