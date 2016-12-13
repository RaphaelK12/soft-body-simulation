#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "fw/AABB.hpp"
#include "ParticleState.hpp"
#include "ControlFrame.hpp"

namespace application
{

class SoftBox
{
public:
    SoftBox();
    ~SoftBox();

    void distributeUniformly(const fw::AABB<glm::dvec3>& box);

    glm::ivec3 getParticleMatrixSize() const;

    const std::vector<ParticleState> getSoftBoxParticles() const;
    const ParticleState& getSoftBoxParticle(glm::ivec3 index) const;
    int getParticleIndex(glm::ivec3 coordinate) const;

    void updateUserInterface();
    void update(double dt);

    const ControlFrame& getControlFrame() { return _controlFrame; }

    void applyRandomDisturbance();

private:
    void fixCurrentBoxPositionUsingSprings();
    void connectBoxToFrame();

    float _particleMass;
    float _springsConstant;
    float _springsAttenuation;

    ParticleSystem _particleSystem;
    ControlFrame _controlFrame;
    glm::ivec3 _particleMatrixSize;
};

}
