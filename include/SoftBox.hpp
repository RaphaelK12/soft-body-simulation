#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "fw/AABB.hpp"
#include "ParticleState.hpp"

namespace application
{

class SoftBox
{
public:
    SoftBox();
    ~SoftBox();

    void distributeUniformly(const fw::AABB<glm::dvec3>& box);

    glm::ivec3 getParticleMatrixSize() const;
    void setParticleMatrixSize(glm::ivec3 particleMatrixSize);

    const std::vector<ParticleState> getSoftBoxParticles() const;
    const ParticleState& getSoftBoxParticle(glm::ivec3 index) const;
    int getParticleIndex(glm::ivec3 coordinate) const;

private:
    std::vector<ParticleState> _particles;
    glm::ivec3 _particleMatrixSize;
};

}
