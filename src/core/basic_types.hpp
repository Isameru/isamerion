
#pragma once

#include "deps.hpp"

using vec2  = glm::vec2;
using ivec2 = glm::ivec2;
using vec3  = glm::vec3;
using vec4  = glm::vec4;
using mat3  = glm::mat3;
using mat4  = glm::mat4;

[[noreturn]] inline void throwSdlError() { throw std::runtime_error{SDL_GetError()}; }
[[noreturn]] inline void throwTtfError() { throw std::runtime_error{TTF_GetError()}; }

// using TexturePtr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;

template<typename... TX> using uptr   = std::unique_ptr<TX...>;
template<typename... TX> using vector = std::vector<TX...>;

template<typename T> class Singleton
{
protected:
    Singleton() noexcept
    {
        assert(objPtr() == nullptr);
        objPtr() = reinterpret_cast<T*>(this);
    }

    ~Singleton() noexcept
    {
        assert(objPtr() == reinterpret_cast<T*>(this));
        objPtr() = nullptr;
    }

public:
    static T& get() noexcept
    {
        assert(objPtr() != nullptr);
        return *objPtr();
    }

private:
    static T*& objPtr() noexcept
    {
        static T* ptr = nullptr;
        return ptr;
    }
};
