#include "ParticleState.hpp"

namespace application
{

ParticleState::ParticleState()
{
}

ParticleState::ParticleState(
    const glm::vec3& position,
    const glm::vec3& velocity
):
    position{position},
    velocity{velocity}
{
}

ParticleState::~ParticleState()
{
}


}

