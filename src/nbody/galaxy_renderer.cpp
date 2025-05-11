/*
    MIT License
    Copyright (c) 2025 Mariusz Łapiński

      ▄█     ▄████████    ▄████████    ▄▄▄▄███▄▄▄▄      ▄████████    ▄████████  ▄█   ▄██████▄  ███▄▄▄▄
      ███    ███    ███   ███    ███  ▄██▀▀▀███▀▀▀██▄   ███    ███   ███    ███ ███  ███    ███ ███▀▀▀██▄
      ███▌   ███    █▀    ███    ███  ███   ███   ███   ███    █▀    ███    ███ ███▌ ███    ███ ███   ███
      ███▌   ███          ███    ███  ███   ███   ███  ▄███▄▄▄      ▄███▄▄▄▄██▀ ███▌ ███    ███ ███   ███
      ███▌ ▀███████████ ▀███████████  ███   ███   ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   ███▌ ███    ███ ███   ███
      ███           ███   ███    ███  ███   ███   ███   ███    █▄  ▀███████████ ███  ███    ███ ███   ███
      ███     ▄█    ███   ███    ███  ███   ███   ███   ███    ███   ███    ███ ███  ███    ███ ███   ███
      █▀    ▄████████▀    ███    █▀    ▀█   ███   █▀    ██████████   ███    ███ █▀    ▀██████▀   ▀█   █▀
                                                                    ███    ███
*/

#include "nbody/galaxy_renderer.hpp"

#include "gfx/display_window.hpp"

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

SpaceDomeRenderer::SpaceDomeRenderer()
    : _shader{vertexShader(), fragmentShader()}
    , _shaderTransformLocation{_shader.getUniformLocation("transform")}
    , _sphereVertexBuffer{GL_ARRAY_BUFFER}
{
    const auto sphereVertices = makeSphereVertices(SPHERE_TESSELATION_LEVEL);
    _sphereVertexCount        = (GLsizei)sphereVertices.size();
    _sphereVertexBuffer.setData(sphereVertices);
}

void SpaceDomeRenderer::draw(const mat4& mvpMat)
{
    _shader.use();

    glEnableVertexAttribArray(0);
    _sphereVertexBuffer.bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    _shader.setUniform(_shaderTransformLocation, mvpMat);

    glDrawArrays(GL_TRIANGLES, 0, _sphereVertexCount);

    glDisableVertexAttribArray(0);
}

std::string_view SpaceDomeRenderer::vertexShader()
{
    return "#version 300 es\n"
           "uniform highp mat4 transform;\n"
           "layout(location = 0) in highp vec3 aPos;\n"
           "out highp float pos_y;\n"
           "void main() {\n"
           "    gl_Position = transform * vec4(aPos, 1.0);\n"
           "    pos_y = aPos.y;\n"
           "}";
}

std::string_view SpaceDomeRenderer::fragmentShader()
{
    return "#version 300 es\n"
           "in highp float pos_y;\n"
           "out highp vec4 out_color;\n"
           "void main() {\n"
           "    out_color = vec4(0.0f, 0.0f, 0.25f - abs(pos_y / 2.5f), 1.0f);\n"
           "}";
}

// Creates a sphere-like polyhedron by recursively subdividing a tetrahedron's faces into smaller triangles.
// Returns a vector of vertices groupped with triples (faces).
//
std::vector<vec3> SpaceDomeRenderer::makeSphereVertices(int tesselationLevel)
{
    std::vector<vec3> vertices;

    // Primary 4 faces, each face has 3 vertices, and each face subdivided `tesselationLevel` times into 4.
    vertices.reserve(3 * 4 * (1 << (2 * tesselationLevel)));

    std::function<void(int, vec3, vec3, vec3)> makeSegmentFn;
    makeSegmentFn = [&](int level, vec3 p0, vec3 p1, vec3 p2) {
        p0 = glm::normalize(p0);
        p1 = glm::normalize(p1);
        p2 = glm::normalize(p2);
        if (level == 0) {
            vertices.push_back(p0);
            vertices.push_back(p1);
            vertices.push_back(p2);
        } else {
            const auto p01 = p0 + p1;
            const auto p02 = p0 + p2;
            const auto p12 = p1 + p2;
            makeSegmentFn(level - 1, p0, p01, p02);
            makeSegmentFn(level - 1, p1, p01, p12);
            makeSegmentFn(level - 1, p2, p02, p12);
            makeSegmentFn(level - 1, p01, p02, p12);
        }
    };

    // See: https://en.wikipedia.org/wiki/Tetrahedron#/media/File:Tetraeder_animation_with_cube.gif
    makeSegmentFn(tesselationLevel, vec3{-1.0f, +1.0f, -1.0f}, vec3{+1.0f, +1.0f, +1.0f}, vec3{+1.0f, -1.0f, -1.0f});
    makeSegmentFn(tesselationLevel, vec3{-1.0f, +1.0f, -1.0f}, vec3{+1.0f, +1.0f, +1.0f}, vec3{-1.0f, -1.0f, +1.0f});
    makeSegmentFn(tesselationLevel, vec3{+1.0f, -1.0f, -1.0f}, vec3{-1.0f, -1.0f, +1.0f}, vec3{-1.0f, +1.0f, -1.0f});
    makeSegmentFn(tesselationLevel, vec3{+1.0f, -1.0f, -1.0f}, vec3{-1.0f, -1.0f, +1.0f}, vec3{+1.0f, +1.0f, +1.0f});

    return vertices;
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

StarRenderer::StarRenderer()
    : _shader{vertexShader(), fragmentShader()}
    , _shaderViewTransformLocation{_shader.getUniformLocation("viewTransform")}
    , _shaderProjectionTransformLocation{_shader.getUniformLocation("projectionTransform")}
    , _quadVertexBuffer{GL_ARRAY_BUFFER}
    , _particlePosBuffer{GL_ARRAY_BUFFER}
    , _particleSizeBuffer{GL_ARRAY_BUFFER}
    , _particleColorBuffer{GL_ARRAY_BUFFER}
{
    const auto quadVertices = std::vector<float>{
        -0.5f, -0.5f, -0.5f, +0.5f, +0.5f, +0.5f, -0.5f, -0.5f, +0.5f, +0.5f, +0.5f, -0.5f,
    };
    _quadVertexBuffer.setData(quadVertices);
}

void StarRenderer::updatePositions(const vector<vec3>& particlePositions)
{
    static_assert(sizeof(vec3) == 3 * sizeof(GLfloat));
    _particlePosBuffer.setData(particlePositions);
    _particlePositionsUpdated = static_cast<int>(particlePositions.size());
}

void StarRenderer::updateSizes(const vector<float>& particleSizes)
{
    _particleSizeBuffer.setData(particleSizes);
    _particleSizesUpdated = static_cast<int>(particleSizes.size());
}

void StarRenderer::updateColors(const vector<vec3>& particleColors)
{
    _particleColorBuffer.setData(particleColors);
    _particleColorsUpdated = static_cast<int>(particleColors.size());
}

void StarRenderer::draw(const mat4& viewMat, const mat4& projMat)
{
    assert(_particlePositionsUpdated == _particleSizesUpdated && "Mismatch in number of update particle positions and sizes");
    assert(_particlePositionsUpdated == _particleColorsUpdated && "Mismatch in number of update particle positions and colors");

    _shader.use();

    glEnableVertexAttribArray(0);
    _quadVertexBuffer.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    _particlePosBuffer.bind();
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    _particleSizeBuffer.bind();
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    _particleColorBuffer.bind();
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glVertexAttribDivisor(3, 1);

    _shader.setUniform(_shaderViewTransformLocation, viewMat);
    _shader.setUniform(_shaderProjectionTransformLocation, projMat);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _particlePositionsUpdated);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

std::string_view StarRenderer::vertexShader()
{
    return "#version 300 es\n"
           "uniform highp mat4 viewTransform;\n"
           "uniform highp mat4 projectionTransform;\n"
           "layout(location = 0) in highp vec2 aPos;\n"
           "layout(location = 1) in highp vec3 aParticlePos;\n"
           "layout(location = 2) in highp float aParticleSize;\n"
           "layout(location = 3) in highp vec3 aParticleColor;\n"
           "out highp vec2 TexCoord;\n"
           "out highp vec3 StarColor;\n"
           "void main() {\n"
           "    gl_Position = projectionTransform * (viewTransform * vec4(aParticlePos, 1.0) + vec4(aPos * aParticleSize, 0.0, 0.0));\n"
           "    TexCoord = 2.0 * aPos;\n"
           "    StarColor = aParticleColor;\n"
           "}";
}

std::string_view StarRenderer::fragmentShader()
{
    return "#version 300 es\n"
           "in highp vec2 TexCoord;\n"
           "in highp vec3 StarColor;\n"
           "out highp vec4 out_color;\n"
           "highp float rand(highp vec2 co) {\n"
           "    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);\n"
           "}\n"
           "void main() {\n"
           "    highp float r = dot(TexCoord, TexCoord);\n"
           "    if (r > 1.0) discard;\n"
           "    highp float rd = rand(vec2(rand(gl_FragCoord.xy), rand(gl_FragCoord.zw)));\n"
           "    highp vec3 star_color = StarColor * (1.0 + 0.85 * (r + rd));"
           "    if (r > 0.9) star_color = 10.0 * (1.0 - r) * star_color;\n"
           "    out_color = vec4(star_color, 1.0);\n"
           "}";
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

SonarRenderer::SonarRenderer()
    : _shader{vertexShader(), fragmentShader()}
    , _shaderTransformLocation{_shader.getUniformLocation("transform")}
    , _coordSystemVertexBuffer{GL_ARRAY_BUFFER}
    , _circleVertexBuffer{GL_ARRAY_BUFFER}
{
    auto coordSystemVertices = std::vector<vec3>{
        {-10.0f, 0.0f, 0.0f}, {+10.0f, 0.0f, 0.0f}, {0.0f, -10.0f, 0.0f}, {0.0f, +10.0f, 0.0f}, {0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, +10.0f},
    };
    _coordSystemVertexBuffer.setData(coordSystemVertices);

    auto circleVertices = std::vector<vec3>{};
    circleVertices.reserve(CIRCLE_VERTEX_COUNT);
    for (int i = 0; i < CIRCLE_VERTEX_COUNT; ++i) {
        const float angle = 2.0f * glm::pi<float>() * i / CIRCLE_VERTEX_COUNT;
        circleVertices.push_back(vec3{cos(angle), 0.0f, sin(angle)});
    }
    _circleVertexBuffer.setData(circleVertices);
}

void SonarRenderer::draw(const mat4& mvpMat)
{
    _shader.use();

    glEnableVertexAttribArray(0);

    // Draw the coordinate system.
    //
    _coordSystemVertexBuffer.bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    _shader.setUniform(_shaderTransformLocation, mvpMat);
    glDrawArrays(GL_LINES, 0, 6);

    // Draw the sonar circle.
    //
    _circleVertexBuffer.bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    const auto modelMat = glm::scale(mat4{1.0f}, _sonarRadius * vec3(1.0f, 1.0f, 1.0f));
    _shader.setUniform(_shaderTransformLocation, mvpMat * modelMat);
    glDrawArrays(GL_LINE_LOOP, 0, CIRCLE_VERTEX_COUNT);

    glDisableVertexAttribArray(0);
}

std::string_view SonarRenderer::vertexShader()
{
    return "#version 300 es\n"
           "uniform highp mat4 transform;\n"
           "layout(location = 0) in highp vec3 aPos;\n"
           "void main() {\n"
           "    gl_Position = transform * vec4(aPos, 1.0);\n"
           "}";
}

std::string_view SonarRenderer::fragmentShader()
{
    return "#version 300 es\n"
           "out highp vec4 out_color;\n"
           "void main() {\n"
           "    out_color = vec4(0.0f, 0.0f, 0.5, 1.0f);\n"
           "}";
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

GalaxyRenderer::GalaxyRenderer(DisplayWindow& renderer)
    : _displayWindow{renderer}
{
}

GalaxyRenderer::~GalaxyRenderer() {}

void GalaxyRenderer::setSonarRadius(float radius) { _sonarRenderer.setSonarRadius(radius); }

void GalaxyRenderer::updateParticlePositions(const vector<vec3>& particlePositions)
{
    _galaxyCenter = particlePositions.empty() ? vec3{} : particlePositions[0];
    _starRenderer.updatePositions(particlePositions);
}

void GalaxyRenderer::updateParticleSizes(const vector<float>& particleSizes) { _starRenderer.updateSizes(particleSizes); }

void GalaxyRenderer::updateParticleColors(const vector<vec3>& particleColors) { _starRenderer.updateColors(particleColors); }

void GalaxyRenderer::draw()
{
    const vec3 cameraPos     = vec3{12.0f, 6.0f, 2.0f};
    const auto viewMat       = glm::lookAt(cameraPos, _galaxyCenter, vec3{0.0f, 1.0f, 0.0f});
    const auto projectionMat = glm::perspective(glm::pi<float>() / 3.0f, (float)_displayWindow.screenSize().x / (float)_displayWindow.screenSize().y, 1.0f, 10000.0f);

    glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    const float radius   = 100.0f;
    const auto  modelMat = glm::scale(mat4{1.0f}, radius * vec3(1.0f, 1.0f, 1.0f));
    _spaceDomeRenderer.draw(projectionMat * viewMat * modelMat);

    glEnable(GL_DEPTH_TEST);
    _starRenderer.draw(viewMat, projectionMat);
    _sonarRenderer.draw(projectionMat * viewMat);
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
