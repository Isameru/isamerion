
#pragma once

#include "core/basic_types.hpp"

class GLShader
{
public:
    using UniformLocation = GLuint;

private:
    GLuint _glProgram;

public:
    GLShader(std::string_view vertexShader, std::string_view fragmentShader);
    GLShader(GLShader&& other) noexcept;
    ~GLShader();
    void            use();
    UniformLocation getUniformLocation(std::string_view name);
    void            setUniform(UniformLocation location, const mat4& value);
};
