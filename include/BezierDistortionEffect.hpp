#pragma once

#include <memory>
#include "glm/glm.hpp"

#include "fw/Effect.hpp"
#include "fw/Material.hpp"
#include "fw/Texture.hpp"

namespace application
{

class BezierDistortionEffect:
    public fw::EffectBase
{
public:
    BezierDistortionEffect();
    virtual ~BezierDistortionEffect();

    virtual void destroy() override;
    virtual void begin() override;
    virtual void end() override;

    void setLightDirection(glm::vec3 lightDirection);
    void setMaterial(const fw::Material& material);

    void setEmissionColor(glm::vec3 color);
    void setSolidColor(glm::vec3 color);
    void setSolidColor(glm::vec4 color);
    void setDistortionControlPoints(const std::vector<glm::vec3>& points);

private:
    void createShaders();

    GLuint _controlPointsLocation;
    GLuint _lightDirectionLocation;
    GLuint _emissionColorLocation;
    GLuint _solidColorLocation;

    glm::vec3 _emissionColor;
    glm::vec4 _solidColor;
    glm::vec3 _lightDirection;

    std::vector<glm::vec3> _points;
};

}
