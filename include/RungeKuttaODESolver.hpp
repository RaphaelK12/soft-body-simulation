#pragma once

#include <cassert>
#include <vector>

namespace application
{

template <typename TPrecision>
class RungeKuttaODESolver
{
public:
    RungeKuttaODESolver();
    virtual ~RungeKuttaODESolver() = default;

protected:
    std::vector<TPrecision> step(
        const std::vector<TPrecision>& state,
        const TPrecision& t,
        const TPrecision& step
    );

    virtual std::vector<TPrecision> evaluateDerivative(
        const std::vector<TPrecision>& state,
        const TPrecision& time
    ) = 0;

private:
    std::vector<TPrecision> addVectors(
        const std::vector<TPrecision> &lhs,
        const std::vector<TPrecision> &rhs,
        TPrecision lhsFactor,
        TPrecision rhsFactor
    );

    int _dimension;
};

template <typename TPrecision>
RungeKuttaODESolver<TPrecision>::RungeKuttaODESolver()
{
}

template <typename TPrecision>
std::vector<TPrecision> RungeKuttaODESolver<TPrecision>::step(
    const std::vector<TPrecision>& input,
    const TPrecision& t,
    const TPrecision& step
)
{
    _dimension = input.size();
    auto halfstep = step / 2;

    auto k1 = evaluateDerivative(input, t);

    auto k2 = evaluateDerivative(
        addVectors(input, k1, 1.0, halfstep),
        t + halfstep
    );

    auto k3 = evaluateDerivative(
        addVectors(input, k2, 1.0, halfstep),
        t + halfstep
    );

    auto k4 = evaluateDerivative(
        addVectors(input, k3, 1.0, step),
        t + step
    );

    std::vector<TPrecision> output(_dimension);
    for (auto i = 0; i < _dimension; ++i)
    {
        output[i] = input[i]
            + (step/6.0) * (k1[i] + 2.0*k2[i] + 2.0*k3[i] + k4[i]);
    }

    return output;
}

template <typename TPrecision>
std::vector<TPrecision> RungeKuttaODESolver<TPrecision>::addVectors(
    const std::vector<TPrecision> &lhs,
    const std::vector<TPrecision> &rhs,
    TPrecision lhsFactor,
    TPrecision rhsFactor
)
{
    assert(lhs.size() == rhs.size());
    assert(lhs.size() == _dimension);

    std::vector<TPrecision> output(_dimension);
    for (auto i = 0; i < _dimension; ++i)
    {
        output[i] = lhsFactor * lhs[i] + rhs[i] * rhsFactor;
    }

    return output;
}

}
