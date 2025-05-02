
#pragma once

#include "core/basic_types.hpp"
#include "gfx/glbuffer.hpp"
#include "gfx/glshader.hpp"

class DisplayWindow;

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

class SpaceDomeRenderer
{
    constexpr static const int SPHERE_TESSELATION_LEVEL = 3;

    GLShader                  _shader;
    GLShader::UniformLocation _shaderTransformLocation;

    GLBuffer _sphereVertexBuffer;
    GLsizei  _sphereVertexCount{};

public:
    SpaceDomeRenderer();
    void draw(const mat4& mvpMat);

private:
    static std::string_view  vertexShader();
    static std::string_view  fragmentShader();
    static std::vector<vec3> makeSphereVertices(int tesselationLevel);
};

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

class StarRenderer
{
    GLShader                  _shader;
    GLShader::UniformLocation _shaderViewTransformLocation;
    GLShader::UniformLocation _shaderProjectionTransformLocation;
    // GLShader::UniformLocation _shaderColorLocation;

    GLBuffer _quadVertexBuffer;
    GLBuffer _particlePosBuffer;
    GLBuffer _particleSizeBuffer;
    GLBuffer _particleColorBuffer;

    int _particlePositionsUpdated = 0;
    int _particleSizesUpdated     = 0;
    int _particleColorsUpdated    = 0;

public:
    StarRenderer();
    void updatePositions(const vector<vec3>& particlePositions);
    void updateSizes(const vector<float>& particleSizes);
    void updateColors(const vector<vec3>& particleColors);
    void draw(const mat4& viewMat, const mat4& projMat);

private:
    static std::string_view vertexShader();
    static std::string_view fragmentShader();
};

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

class SonarRenderer
{
    constexpr static const int COORD_SYSTEM_VERTEX_COUNT = 6;
    constexpr static const int CIRCLE_VERTEX_COUNT = 32;

    GLShader                  _shader;
    GLShader::UniformLocation _shaderTransformLocation;
    // GLShader::UniformLocation _shaderColorLocation;

    GLBuffer _coordSystemVertexBuffer;
    GLBuffer _circleVertexBuffer;
    float _sonarRadius{};

public:
    SonarRenderer();
    void setSonarRadius(float radius) { _sonarRadius = radius; }
    void draw(const mat4& mvpMat);

private:
    static std::string_view vertexShader();
    static std::string_view fragmentShader();
};

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

class GalaxyRenderer
{
public:
    DisplayWindow& _displayWindow;

    vec3              _galaxyCenter{0.0f, 0.0f, 0.0f};
    SpaceDomeRenderer _spaceDomeRenderer;
    StarRenderer      _starRenderer;
    SonarRenderer     _sonarRenderer;

public:
    GalaxyRenderer(DisplayWindow& renderer);
    ~GalaxyRenderer();

    void setSonarRadius(float radius);
    void updateParticlePositions(const vector<vec3>& particlePositions);
    void updateParticleSizes(const vector<float>& particleSizes);
    void updateParticleColors(const vector<vec3>& particleColors);
    void draw();

private:
};

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
