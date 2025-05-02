
#include "core/clock.hpp"

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

// EM_JS(double, high_res_now, (), { return performance.now(); });

Clock::time_point Clock::now() noexcept
{
#ifdef __EMSCRIPTEN__
    return time_point{std::chrono::microseconds((uint64_t)(emscripten_get_now() * 1000.0))};
//     // return time_point{std::chrono::microseconds((uint64_t)(high_res_now() * 1000.0))};
#else
    return std::chrono::high_resolution_clock::now();
#endif
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
