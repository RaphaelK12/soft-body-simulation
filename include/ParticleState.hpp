#pragma once

#include "glm/glm.hpp"

namespace application
{

struct ParticleState
{
public:
    ParticleState();
    ParticleState(const glm::vec3& position, const glm::vec3& velocity);
    ~ParticleState();

    glm::vec3 position;
    glm::vec3 velocity;
};

}
