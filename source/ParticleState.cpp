#include "ParticleState.hpp"
#include <random>
#include "easylogging++.h"

namespace application
{

ParticleState::ParticleState()
{
}

ParticleState::ParticleState(
    const glm::dvec3& position,
    const glm::dvec3& momentum,
    double invMass
):
    position{position},
    momentum{momentum},
    invMass{invMass}
{
}

SpringConstraint::SpringConstraint():
    springLength{},
    springConstant{1.0},
    attenuationFactor{0.2},
    a{},
    b{}
{
}

glm::dvec3 SpringConstraint::getForce(const ParticleSystem& system) const
{
    ParticleState A;
    ParticleState B;

    if (a >= 0)
    {
        A = system.getParticleStates()[a];
    }
    else
    {
        A = system.getStaticParticles()[-a-1];
    }

    if (b >= 0)
    {
        B = system.getParticleStates()[b];
    }
    else
    {
        B = system.getStaticParticles()[-b-1];
    }

    auto relation = B.position - A.position;
    auto relationDirection = glm::length(relation) > 10e-4
        ? glm::normalize(relation)
        : glm::dvec3{1.0, 0.0, 0.0};

    auto springCurrentLength = glm::length(relation);
    auto springForce =
        - (springCurrentLength - springLength) * springConstant;

    return -relationDirection * springForce;
}

ParticleSystem::ParticleSystem():
    _roomSize{10.0, 5.0, 10.0}
{
}

ParticleSystem::~ParticleSystem()
{
}

std::vector<double> ParticleSystem::storePhysicsState() const
{
    std::vector<double> output;

    for (const auto& particle: _particleState)
    {
        output.push_back(particle.position.x);
        output.push_back(particle.position.y);
        output.push_back(particle.position.z);

        output.push_back(particle.momentum.x);
        output.push_back(particle.momentum.y);
        output.push_back(particle.momentum.z);
    }

    return output;
}

void ParticleSystem::applyPhysicsState(const std::vector<double>& state)
{
    const int fieldsPerParticle = 6;
    auto appliedFields = 0;
    auto appliedParticles = 0;
    while (appliedFields + fieldsPerParticle - 1 < state.size())
    {
        _particleState[appliedParticles].position.x = state[appliedFields++];
        _particleState[appliedParticles].position.y = state[appliedFields++];
        _particleState[appliedParticles].position.z = state[appliedFields++];
        _particleState[appliedParticles].momentum.x = state[appliedFields++];
        _particleState[appliedParticles].momentum.y = state[appliedFields++];
        _particleState[appliedParticles].momentum.z = state[appliedFields++];
        ++appliedParticles;
    }
}

void ParticleSystem::update(double dt)
{
    auto physicsStep = std::min(dt, 0.01);

    auto availableTime = physicsStep;
    while (availableTime > 10e-6)
    {
        availableTime -= singleStep(availableTime);
    }
}

double ParticleSystem::singleStep(double maxDt)
{
    auto physicsState = storePhysicsState();
    auto newPhysicsState = step(physicsState, 0.0, maxDt);
    applyPhysicsState(newPhysicsState);

    auto interpenetration = checkInterpenetration();
    if (interpenetration)
    {
        const double cTimeTolerance = 10e-3;
        auto stepLowerLimit = 0.001;
        auto stepUpperLimit = maxDt;
        std::vector<double> lastPhysicsState;

        while ((stepUpperLimit - stepLowerLimit) > cTimeTolerance)
        {
            auto midpointStep = (stepUpperLimit + stepLowerLimit) / 2.0;
            lastPhysicsState = step(physicsState, 0.0, midpointStep);
            applyPhysicsState(lastPhysicsState);
            if (checkInterpenetration())
            {
                stepUpperLimit = midpointStep;
            }
            else
            {
                stepLowerLimit = midpointStep;
            }
        }

        auto chosenTouchTime = stepLowerLimit;
        auto touchPhysicsState = step(physicsState, 0.0, chosenTouchTime);

        applyPhysicsState(touchPhysicsState);
        applyImpulsesToCollidingContacts();

        return chosenTouchTime;
    }

    return maxDt;
}

void ParticleSystem::clear()
{
    _particleState.clear();
}

void ParticleSystem::addParticle(const ParticleState& particle)
{
    _particleState.push_back(particle);
}

void ParticleSystem::addConstraint(const SpringConstraint& constraint)
{
    _constraints.push_back(constraint);
}

const std::vector<ParticleState>& ParticleSystem::getParticleStates() const
{
    return _particleState;
}

void ParticleSystem::setStaticParticles(
    const std::vector<ParticleState>& particles
)
{
    _staticParticles = particles;
}

const std::vector<ParticleState>& ParticleSystem::getStaticParticles() const
{
    return _staticParticles;
}

std::vector<double> ParticleSystem::evaluateDerivative(
    const std::vector<double>& state,
    const double& time
)
{
    applyPhysicsState(state);
    clearForces();
    calculateForces();
    updateParticles();
    return storePhysicsStateDerivative();
}

void ParticleSystem::clearForces()
{
    for (auto& particle: _particleState)
    {
        particle.netForce = {0, 0, 0};
    }
}

void ParticleSystem::calculateForces()
{
    for (const auto& constraint: _constraints)
    {
        auto force = constraint.getForce(*this);

        if (constraint.a >= 0)
        {
            _particleState[constraint.a].netForce += force;
        }

        if (constraint.b >= 0)
        {
            _particleState[constraint.b].netForce -= force;
        }
    }
}

void ParticleSystem::updateParticles()
{
    for (auto& particle: _particleState)
    {
        particle.velocity = particle.invMass * particle.momentum;
        particle.netForce += -_movementAttenuationFactor * particle.velocity;
    }
}

std::vector<double> ParticleSystem::storePhysicsStateDerivative() const
{
    std::vector<double> output;

    for (const auto& particle: _particleState)
    {
        output.push_back(particle.velocity.x);
        output.push_back(particle.velocity.y);
        output.push_back(particle.velocity.z);

        output.push_back(particle.netForce.x);
        output.push_back(particle.netForce.y);
        output.push_back(particle.netForce.z);
    }

    return output;
}

void ParticleSystem::applyRandomDisturbance()
{
    std::random_device randomDevice;
    std::default_random_engine randomEngine(randomDevice());
    std::uniform_int_distribution<double> uniformDist(-1, 1);
    for (auto& particle: _particleState)
    {
        particle.momentum.x = uniformDist(randomEngine);
        particle.momentum.y = uniformDist(randomEngine);
        particle.momentum.z = uniformDist(randomEngine);
    }
}

void ParticleSystem::updateSoftBoxParticlesMass(double particleMass)
{
    for (auto& particle: _particleState)
    {
        particle.invMass = 1.0 / particleMass;
    }
}

void ParticleSystem::updateSoftBoxConstraints(
    double springConstant,
    double springAttenuation
)
{
    for (auto& constraint: _constraints)
    {
        if (constraint.a < 0 || constraint.b < 0)
        {
            continue;
        }

        constraint.springConstant = springConstant;
        constraint.attenuationFactor = springAttenuation;
    }
}

void ParticleSystem::updateFrameConstraints(
    double springConstant,
    double springAttenuation
)
{
    for (auto& constraint: _constraints)
    {
        if (constraint.a >= 0 && constraint.b >= 0)
        {
            continue;
        }

        constraint.springConstant = springConstant;
        constraint.attenuationFactor = springAttenuation;
    }
}

void ParticleSystem::updateEnvironmentConstant(
    double movementAttenuationFactor,
    double elasticCollisionFactor
)
{
    _movementAttenuationFactor = movementAttenuationFactor;
    _elasticCollisionFactor = elasticCollisionFactor;
}

bool ParticleSystem::checkInterpenetration()
{
    auto minPosition = -0.5 * _roomSize;
    auto maxPosition = +0.5 * _roomSize;
    for (const auto& particle: _particleState)
    {
        if (glm::any(glm::lessThan(particle.position, minPosition)))
        {
            return true;
        }

        if (glm::any(glm::greaterThan(particle.position, maxPosition)))
        {
            return true;
        }
    }

    return false;
}

void ParticleSystem::applyImpulsesToCollidingContacts()
{
    auto epsilon = 10e-5;
    for (auto& particle: _particleState)
    {
        bool applyPentalty = false;
        if (particle.position.x < -_roomSize.x / 2 + epsilon)
        {
            if (particle.momentum.x < 0.0f)
            {
                particle.momentum.x = -particle.momentum.x;
            }
            applyPentalty = true;
        } else if (particle.position.x > _roomSize.x / 2 - epsilon)
        {
            if (particle.momentum.x > 0.0f)
            {
                particle.momentum.x = -particle.momentum.x;
            }
            applyPentalty = true;
        }

        if (particle.position.y < -_roomSize.y / 2 + epsilon)
        {
            if (particle.momentum.y < 0.0f)
            {
                particle.momentum.y = -particle.momentum.y;
            }

            applyPentalty = true;
        } else if (particle.position.y > _roomSize.y / 2 - epsilon)
        {
            if (particle.momentum.y > 0.0f)
            {
                particle.momentum.y = -particle.momentum.y;
            }

            applyPentalty = true;
        }

        if (particle.position.z < -_roomSize.z / 2 + epsilon)
        {
            if (particle.momentum.z < 0.0f)
            {
                particle.momentum.z = -particle.momentum.z;
            }

            applyPentalty = true;
        } else if (particle.position.z > _roomSize.z / 2 - epsilon)
        {
            if (particle.momentum.z > 0.0f)
            {
                particle.momentum.z = -particle.momentum.z;
            }

            applyPentalty = true;
        }

        if (applyPentalty)
        {
            particle.momentum = _elasticCollisionFactor * particle.momentum;
        }
    }
}

}
