#include "render/WaterRenderer.hpp"

#include <cmath>

#include <glm/glm.hpp>

#include <rw/debug.hpp>
#include <rw/types.hpp>

#include "engine/GameData.hpp"
#include "engine/GameWorld.hpp"
#include "render/GameRenderer.hpp"
#include "render/GameShaders.hpp"
#include "render/OpenGLRenderer.hpp"

WaterRenderer::WaterRenderer(GameRenderer* renderer) {
    maskDraw.setFaceType(GL_TRIANGLES);
    gridDraw.setFaceType(GL_TRIANGLES);

    waterProg = renderer->getRenderer()->createShader(
        GameShaders::WaterHQ::VertexShader,
        GameShaders::WaterHQ::FragmentShader);
    maskProg = renderer->getRenderer()->createShader(
        GameShaders::Mask3D::VertexShader, GameShaders::Mask3D::FragmentShader);

    renderer->getRenderer()->setProgramBlockBinding(waterProg.get(), "SceneData", 1);
    renderer->getRenderer()->setProgramBlockBinding(maskProg.get(), "SceneData", 1);

    renderer->getRenderer()->setUniformTexture(waterProg.get(), "data", 1);

    // Generate grid mesh
    int gridres = 60;
    std::vector<glm::vec2> grid;
    float gridresinv = 1.f / (gridres * 0.5f);
    glm::vec2 b(-1.f, -1.f);
    for (int x = 0; x < gridres; x++) {
        for (int y = 0; y < gridres; y++) {
            glm::vec2 tMin(b + glm::vec2(x, y) * gridresinv);
            glm::vec2 tMax(b + glm::vec2(x + 1, y + 1) * gridresinv);

            // Build geometry
            grid.emplace_back(tMax.x, tMax.y);
            grid.emplace_back(tMax.x, tMin.y);
            grid.emplace_back(tMin.x, tMin.y);

            grid.emplace_back(tMin.x, tMin.y);
            grid.emplace_back(tMin.x, tMax.y);
            grid.emplace_back(tMax.x, tMax.y);
        }
    }

    gridGeom.uploadVertices(static_cast<GLsizei>(grid.size()), sizeof(glm::vec2) * grid.size(),
                            grid.data());
    gridGeom.getDataAttributes().emplace_back(ATRS_Position, 2, 0, 0, GL_FLOAT);
    gridDraw.addGeometry(&gridGeom);
}

void WaterRenderer::setWaterTable(const float* waterHeights, const unsigned int nHeights,
                                  const uint8_t* tiles, const unsigned int nTiles) {
    // Determine the dimensions of the input tiles
    auto edgeNum = static_cast<unsigned int>(sqrt(nTiles));
    float tileSize = WATER_WORLD_SIZE / edgeNum;
    glm::vec2 wO{-WATER_WORLD_SIZE / 2.f, -WATER_WORLD_SIZE / 2.f};

    std::vector<glm::vec3> vertexData;

    for (auto x = 0u; x < edgeNum; x++) {
        int xi = x * WATER_HQ_DATA_SIZE;
        for (auto y = 0u; y < edgeNum; y++) {
            if (tiles[xi + y] >= nHeights) {
                continue;
            }

            // Tiles with the magic value contain no water.
            if (tiles[xi + y] >= NO_WATER_INDEX) {
                continue;
            }
            float h = waterHeights[tiles[xi + y]];
            float hMax = h + WATER_HEIGHT;
            glm::vec2 tMin(wO + glm::vec2(x, y) * tileSize);
            glm::vec2 tMax(wO + glm::vec2(x + 1, y + 1) * tileSize);

            // Build geometry
            vertexData.emplace_back(tMax.x, tMax.y, hMax);
            vertexData.emplace_back(tMax.x, tMin.y, hMax);
            vertexData.emplace_back(tMin.x, tMin.y, hMax);

            vertexData.emplace_back(tMin.x, tMin.y, hMax);
            vertexData.emplace_back(tMin.x, tMax.y, hMax);
            vertexData.emplace_back(tMax.x, tMax.y, hMax);
        }
    }

    maskGeom.uploadVertices(static_cast<GLsizei>(vertexData.size()),
                            sizeof(glm::vec3) * vertexData.size(),
                            vertexData.data());
    maskGeom.getDataAttributes().emplace_back(ATRS_Position, 3, 0, 0, GL_FLOAT);
    maskDraw.addGeometry(&maskGeom);
}

void WaterRenderer::setDataTexture(GLuint fbBinding, GLuint dataTexture) {
    fbOutput = fbBinding;
    _dataTexture = dataTexture;
}

void WaterRenderer::render(GameRenderer* renderer, GameWorld* world) {
    auto r = renderer->getRenderer();

    auto waterTex = world->data->findSlotTexture("particle", "water_old");
    RW_CHECK(waterTex != nullptr, "Water texture is null");
    if (waterTex == nullptr) {
        // Can't render water if we don't have a texture.
        return;
    }

    Renderer::DrawParameters wdp;
    wdp.start = 0;
    wdp.count = maskGeom.getCount();
    wdp.textures = {{0}};
    glm::mat4 m(1.0);

    glEnable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);

    GLenum buffers[] = {GL_COLOR_ATTACHMENT1};
    glDrawBuffers(1, buffers);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    r->useProgram(maskProg.get());

    r->drawArrays(m, &maskDraw, wdp);

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glEnable(GL_DEPTH_TEST);

    r->useProgram(waterProg.get());

    buffers[0] = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, buffers);

    r->setUniform(waterProg.get(), "time", world->getGameTime());
    r->setUniform(waterProg.get(), "waveParams",
                  glm::vec2(WATER_SCALE, WATER_HEIGHT));
    auto ivp =
        glm::inverse(r->getSceneData().projection * r->getSceneData().view);
    r->setUniform(waterProg.get(), "inverseVP", ivp);

    wdp.count = gridGeom.getCount();
    wdp.textures = {{waterTex->getName(), _dataTexture}};

    r->drawArrays(m, &gridDraw, wdp);

    glDisable(GL_STENCIL_TEST);
}
