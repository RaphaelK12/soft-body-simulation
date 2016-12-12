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
    auto relationDirection = glm::normalize(relation);

    auto relativeVelocityA = A.invMass * glm::dot(
        relationDirection,
        A.momentum
    );

    auto relativeVelocityB = B.invMass * glm::dot(
        relationDirection,
        B.momentum
    );

    auto springExtensionVelocity = relativeVelocityB - relativeVelocityA;
    auto springCurrentLength = glm::length(relation);
    auto springForce = -springExtensionVelocity * attenuationFactor
        - (springCurrentLength - springLength) * springConstant;

    return -relationDirection * springForce;
}

ParticleSystem::ParticleSystem()
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
    auto physicsState = storePhysicsState();

    auto newPhysicsState = step(
        physicsState,
        0.0,
        physicsStep
    );

    applyPhysicsState(newPhysicsState);
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

}
