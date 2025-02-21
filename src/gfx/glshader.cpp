
#include "gfx/glshader.hpp"

GLuint compileShader(GLuint shaderType, std::string_view shader)
{
    assert(shaderType == GL_VERTEX_SHADER || shaderType == GL_FRAGMENT_SHADER);

    GLuint      shaderId  = glCreateShader(shaderType);
    const char* shaderPtr = shader.data();
    glShaderSource(shaderId, 1, &shaderPtr, nullptr);
    glCompileShader(shaderId);

    GLint compilationStatus{};
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compilationStatus);
    if (compilationStatus == 0) {
        GLint logLength{};
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

        std::string errorMessage(logLength, 0);
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

        glGetShaderInfoLog(shaderId, errorMessage.size(), &logLength, errorMessage.data());

        glDeleteShader(shaderId);
        std::string_view errorMessageHeader = (shaderType == GL_VERTEX_SHADER) ? "GLSL vertex shader compilation failed: " : "GLSL fragment shader compilation failed: ";
        throw std::runtime_error{std::string{errorMessageHeader} + errorMessage};
    }

    return shaderId;
}

GLuint compileProgram(std::string_view vertexShader, std::string_view fragmentShader)
{
    const GLuint vertexShaderId   = compileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint       fragmentShaderId = 0;
    try {
        fragmentShaderId = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    } catch (...) {
        glDeleteShader(vertexShaderId);
        throw;
    }

    GLuint shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, vertexShaderId);
    glAttachShader(shaderProgramId, fragmentShaderId);
    glLinkProgram(shaderProgramId);

    glDetachShader(shaderProgramId, vertexShaderId);
    glDetachShader(shaderProgramId, fragmentShaderId);
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    GLint linkStatus{};
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == 0) {
        GLint logLength{};
        glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &logLength);

        std::string errorMessage(logLength, 0);
        glGetProgramInfoLog(shaderProgramId, errorMessage.size(), &logLength, errorMessage.data());

        glDeleteProgram(shaderProgramId);
        throw std::runtime_error{"GLSL shader program linking failed: " + errorMessage};
    }

    return shaderProgramId;
}

GLShader::GLShader(std::string_view vertexShader, std::string_view fragmentShader)
    : _glProgram{compileProgram(vertexShader, fragmentShader)}
{
}

GLShader::GLShader(GLShader&& other) noexcept
    : _glProgram{std::exchange(other._glProgram, 0)}
{
}

GLShader::~GLShader()
{
    if (_glProgram != 0) {
        glDeleteProgram(_glProgram);
    }
}

void GLShader::use() { glUseProgram(_glProgram); }

GLShader::UniformLocation GLShader::getUniformLocation(std::string_view name) { return glGetUniformLocation(_glProgram, name.data()); }

void GLShader::setUniform(UniformLocation location, const mat4& value) { glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]); }
