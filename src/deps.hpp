
#pragma once

// Emscripten: A complete compiler toolchain to WebAssembly, using LLVM
// https://emscripten.org/
//
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// SDL2: Simple DirectMedia Layer (v2) with special handling for Emscripten
// https://www.libsdl.org/
//
#if defined _MSC_VER || defined __linux__
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#elif defined __EMSCRIPTEN__
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#else
#error Unsupported build config
#endif

// GLM: OpenGL Mathematics
// https://glm.g-truc.net/
//
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>

// GLEW: OpenGL Extension Wrangler Library
// http://glew.sourceforge.net/
//
#include <GL/glew.h>

// C++ Standard Library aka "STL"
// https://en.cppreference.com/w/cpp/header
//
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <span>
#include <utility>
#include <vector>
