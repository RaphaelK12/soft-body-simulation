#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "RungeKuttaODESolver.hpp"

namespace application
{

struct ParticleState
{
public:
    ParticleState();
    ParticleState(
        const glm::dvec3& position,
        const glm::dvec3& momentum,
        double invMass = 1.0
    );

    double invMass;

    glm::dvec3 position;
    glm::dvec3 momentum;

    glm::dvec3 velocity;
    glm::dvec3 netForce;
};

class ParticleSystem;

struct SpringConstraint
{
public:
    SpringConstraint();

    glm::dvec3 getForce(const ParticleSystem& system) const;

    double springLength;
    double springConstant;
    double attenuationFactor;
    int a;
    int b;
};

class ParticleSystem:
    public RungeKuttaODESolver<double>
{
public:
    ParticleSystem();
    ~ParticleSystem();

    std::vector<double> storePhysicsState() const;
    void applyPhysicsState(const std::vector<double>& state);

    void update(double dt);

    void clear();
    void addParticle(const ParticleState& particle);
    void addConstraint(const SpringConstraint& constraint);

    const std::vector<ParticleState>& getParticleStates() const;

    void setStaticParticles(const std::vector<ParticleState>& particles);
    const std::vector<ParticleState>& getStaticParticles() const;

    void applyRandomDisturbance();

    void updateSoftBoxParticlesMass(double particleMass);

    void updateSoftBoxConstraints(
        double springConstant,
        double springAttenuation
    );

protected:
    virtual std::vector<double> evaluateDerivative(
        const std::vector<double>& state,
        const double& time
    );

private:
    void clearForces();
    void calculateForces();
    void updateParticles();
    std::vector<double> storePhysicsStateDerivative() const;

    std::vector<ParticleState> _staticParticles;
    std::vector<ParticleState> _particleState;
    std::vector<SpringConstraint> _constraints;
};

}
