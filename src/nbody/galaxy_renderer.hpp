
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
    GLShader::UniformLocation _shaderColorLocation;

    GLBuffer _quadVertexBuffer;
    GLBuffer _particlePosBuffer;
    GLBuffer _particleSizeBuffer;

    int _particlePositionsUpdated = 0;
    int _particleSizesUpdated     = 0;

public:
    StarRenderer();
    void updatePositions(const vector<vec3>& particlePositions);
    void updateSizes(const vector<float>& particleSizes);
    void draw(const mat4& viewMat, const mat4& projMat);

private:
    static std::string_view vertexShader();
    static std::string_view fragmentShader();
};

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

class GalaxyRenderer
{
public:
    DisplayWindow& _displayWindow;

    SpaceDomeRenderer _spaceDomeRenderer;
    StarRenderer      _starRenderer;

public:
    GalaxyRenderer(DisplayWindow& renderer);
    ~GalaxyRenderer();

    void updateParticlePositions(const vector<vec3>& particlePositions);
    void updateParticleSizes(const vector<float>& particleSizes);
    void draw();

private:
};

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
