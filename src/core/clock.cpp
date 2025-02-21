
#include "core/clock.hpp"

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

Clock::time_point Clock::now() noexcept
{
#ifdef __EMSCRIPTEN__
    return time_point{std::chrono::microseconds((uint64_t)(emscripten_get_now() * 1000.0))};
#else
    return std::chrono::high_resolution_clock::now();
#endif
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
