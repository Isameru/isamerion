
#include "app.hpp"

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

SdlInitializer::SdlInitializer()
{
    if (SDL_Init(SDL_INIT_EVERYTHING & ~SDL_INIT_HAPTIC) != 0) {
        throwSdlError();
    }

    const int  desiredImgInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    const auto actualImgInitFlags  = IMG_Init(desiredImgInitFlags);
    if (actualImgInitFlags != desiredImgInitFlags) {
        throw std::runtime_error{"Failed to initialize SDL IMG subsystem"};
    }

    if (TTF_Init() != 0) {
        throwTtfError();
    }
}

SdlInitializer::~SdlInitializer()
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

App::App()
    : _sdlInitializer{}
    , _displayWindow{DisplayWindow::Options{.windowTitle = "Isamerion"}}
    , _galaxyScene{_displayWindow}
{
}

App::~App() {}

void App::run()
{
    _startTime    = Clock::now();
    _lastTickTime = _startTime;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(
        [](void* ctx) {
            App& app = *reinterpret_cast<App*>(ctx);
            app.onTick();
            if (app._quitRequested) {
                emscripten_cancel_main_loop();
            }
        },
        this, 0, 1);
#else
    while (!_quitRequested) {
        onTick();
    }
#endif
}

void App::onTick()
{
    const auto nowTime         = Clock::now();
    const auto elapsedDuration = nowTime - _lastTickTime;

    if (_tickCount % 60 == 0) {
        const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - _startTime).count();
        std::cout << "FPS: " << 1000.0 * static_cast<double>(_tickCount) / (static_cast<double>(elapsedMs) + 1) << std::endl;
    }

    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
        _galaxyScene.handleEvent(event);
        if (event.type == SDL_QUIT) {
            _quitRequested = true;
            return;
        }
    }

    _galaxyScene.onTick(std::chrono::duration_cast<std::chrono::microseconds>(elapsedDuration).count() / 1e+6);

    _lastTickTime = nowTime;
    ++_tickCount;
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
