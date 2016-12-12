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

struct SpringContraint
{
public:
    SpringContraint();

    glm::dvec3 getForce() const;

    double springLength;
    double springConstant;
    double attenuationFactor;
    ParticleState* a;
    ParticleState* b;
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
    const std::vector<ParticleState>& getParticleStates() const;

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

    std::vector<ParticleState> _particleState;
    std::vector<SpringContraint> _contraints;
};

}
