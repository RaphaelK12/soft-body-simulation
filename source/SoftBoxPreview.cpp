#include "SoftBoxPreview.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "fw/DebugShapes.hpp"

namespace application
{
SoftBoxPreview::SoftBoxPreview():
    _particleMarkerSize{0.03f}
{
    _box = fw::createBox({1.0f, 1.0f, 1.0f});
    _lineSetPreview = std::make_shared<LineSetPreview>();
}

SoftBoxPreview::~SoftBoxPreview()
{
}

std::vector<fw::GeometryChunk> SoftBoxPreview::render(
    const SoftBox& softBox
) const
{
    std::vector<fw::GeometryChunk> chunks;

    glm::mat4 softBoxParticleMarkerScaling = glm::scale(
        glm::mat4{},
        {_particleMarkerSize, _particleMarkerSize, _particleMarkerSize}
    );

    std::shared_ptr<fw::Material> markerMaterial =
        std::make_shared<fw::Material>();
    markerMaterial->setEmissionColor({0.7f, 0.0f, 0.0f});
    markerMaterial->setBaseAlbedoColor({1.0f, 0.0f, 0.0f, 1.0f});

    const auto& particles = softBox.getSoftBoxParticles();
    for (const auto& particle: particles)
    {
        glm::mat4 translationMatrix = glm::translate(
            glm::mat4{},
            particle.position
        );

        auto modelMatrix = translationMatrix * softBoxParticleMarkerScaling;
        chunks.push_back({_box, markerMaterial, modelMatrix});
    }

    std::shared_ptr<fw::Material> lineMaterial =
        std::make_shared<fw::Material>();
    lineMaterial->setEmissionColor({1.0f, 0.0f, 0.0f});

    std::vector<fw::VertexColor> vertices;
    std::vector<GLuint> indices;

    auto matrixSize = softBox.getParticleMatrixSize();
    for (auto z = 0; z < matrixSize.z; ++z)
    for (auto y = 0; y < matrixSize.y; ++y)
    for (auto x = 0; x < matrixSize.x; ++x)
    {
        auto mainParticle = softBox.getSoftBoxParticle({x, y, z});
        auto currentIndex = softBox.getParticleIndex({x, y, z});
        vertices.push_back({mainParticle.position, {1.0f, 0.0f, 0.0f}});

        for (auto i = -1; i <= 1; ++i)
        for (auto j = -1; j <= 1; ++j)
        for (auto k = 0; k <= 1; ++k)
        {
            if (i == 0 && j == 0 && k == 0) { continue; }
            if (x + i < 0 || x + i >= matrixSize.x
                || y + j < 0 || y + j >= matrixSize.y
                || z + k < 0 || z + k >= matrixSize.z)
            {
                continue;
            }

            indices.push_back(currentIndex);
            indices.push_back(softBox.getParticleIndex({x + i, y + j, z + k}));
        }
    }

    _lineSetPreview->setVertices(vertices);
    _lineSetPreview->setIndices(indices);

    chunks.push_back({_lineSetPreview, lineMaterial, {}});
    return chunks;
}

}
