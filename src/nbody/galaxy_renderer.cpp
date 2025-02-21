
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

std::vector<vec3> SpaceDomeRenderer::makeSphereVertices(int tesselationLevel)
{
    std::vector<vec3> vertices;

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
    , _shaderColorLocation{_shader.getUniformLocation("color")}
    , _quadVertexBuffer{GL_ARRAY_BUFFER}
    , _particlePosBuffer{GL_ARRAY_BUFFER}
    , _particleSizeBuffer{GL_ARRAY_BUFFER}
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

void StarRenderer::draw(const mat4& viewMat, const mat4& projMat)
{
    if (_particlePositionsUpdated != _particleSizesUpdated) {
        throw std::runtime_error{"Mismatch in number of update particle positions and sizes"};
    }

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

    const vec4 color = {1.0f, 0.5f, 0.5f, 1.0f};
    glUniform4fv(_shaderColorLocation, 1, &color[0]);

    _shader.setUniform(_shaderViewTransformLocation, viewMat);
    _shader.setUniform(_shaderProjectionTransformLocation, projMat);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _particlePositionsUpdated);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

std::string_view StarRenderer::vertexShader()
{
    return "#version 300 es\n"
           "uniform highp mat4 viewTransform;\n"
           "uniform highp mat4 projectionTransform;\n"
           "layout(location = 0) in highp vec2 aPos;\n"
           "layout(location = 1) in highp vec3 aParticlePos;\n"
           "layout(location = 2) in highp float particleSize;\n"
           "out highp vec2 TexCoord;\n"
           "void main() {\n"
           "    gl_Position = projectionTransform * (viewTransform * vec4(aParticlePos, 1.0) + vec4(aPos * particleSize, 0.0, 0.0));\n"
           //"    TexCoord = aPos + vec2(0.5, 0.5);\n"
           "    TexCoord = 2.0 * aPos;\n"
           "}";
}

std::string_view StarRenderer::fragmentShader()
{
    return "#version 300 es\n"
           "uniform highp vec4 color;\n"
           "uniform sampler2D particleTexture;\n"
           "in highp vec2 TexCoord;\n"
           "out highp vec4 out_color;\n"
           "highp float rand(highp vec2 co) {\n"
           "    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);\n"
           "}\n"
           "void main() {\n"
           //"    highp vec4 c = texture(particleTexture, TexCoord);\n"
           //"    if (c[3] < 0.5) discard;\n"

           "    highp float r = dot(TexCoord, TexCoord);\n"
           //"    highp float rd = rand(vec2(rand(gl_FragCoord.xy), rand(gl_FragCoord.zw)));\n"
           //"    if (r*r*r > rd) discard;\n"
           "    if (r*r > 1.0) discard;\n"
           "    out_color = vec4(20.0f * gl_FragCoord.w * vec3(1.0f, 1.0f, 1.0f), 1.0f);\n"
           "}";
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---

GalaxyRenderer::GalaxyRenderer(DisplayWindow& renderer)
    : _displayWindow{renderer}
{
}

GalaxyRenderer::~GalaxyRenderer() {}

void GalaxyRenderer::updateParticlePositions(const vector<vec3>& particlePositions) { _starRenderer.updatePositions(particlePositions); }

void GalaxyRenderer::updateParticleSizes(const vector<float>& particleSizes) { _starRenderer.updateSizes(particleSizes); }

void GalaxyRenderer::draw()
{
    const vec3 cameraPos = vec3{20.0f, 10.0f, 0.0f};
    const auto viewMat   = glm::lookAt(cameraPos, vec3{0.0f, 0.0f, 0.0f}, vec3{0.0f, 1.0f, 0.0f});
    const auto projectionMat =
        glm::perspective(glm::pi<float>() / 3.0f, (float)_displayWindow.screenSize().x / (float)_displayWindow.screenSize().y, 1.0f, 10000.0f);

    glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    const float radius   = 100.0f;
    const auto  modelMat = glm::scale(mat4{1.0f}, radius * vec3(1.0f, 1.0f, 1.0f));
    _spaceDomeRenderer.draw(projectionMat * viewMat * modelMat);

    glEnable(GL_DEPTH_TEST);

    _starRenderer.draw(viewMat, projectionMat);
}

// ---―--―-――-―――-―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――-―――-――-―--―---
