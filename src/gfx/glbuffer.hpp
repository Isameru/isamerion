
#pragma once

#include "core/basic_types.hpp"

class GLBuffer
{
    const GLenum _target;
    GLuint       _glBuffer = 0;

public:
    GLBuffer(GLenum target);
    GLBuffer(GLBuffer&& other) noexcept;
    ~GLBuffer();

    GLuint id() const { return _glBuffer; }
    void   bind();
    void   setData(std::span<const std::byte> dataView);

    template<typename T> void setData(const std::vector<T>& data) { setData(std::as_bytes(std::span{data})); }
};
