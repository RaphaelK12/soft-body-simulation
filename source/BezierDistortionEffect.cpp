#include "BezierDistortionEffect.hpp"
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include "Config.hpp"

namespace application
{

BezierDistortionEffect::BezierDistortionEffect():
    _solidColor{1.0, 0.0, 0.0, 1.0},
    _lightDirection{0.0, 1.0, 0.0}
{
    createShaders();

    _controlPointsLocation = glGetUniformLocation(
        _shaderProgram->getId(),
        "BezierCubeControlPoints"
    );

    _lightDirectionLocation = glGetUniformLocation(
        _shaderProgram->getId(),
        "LightDirection"
    );

    _emissionColorLocation = glGetUniformLocation(
        _shaderProgram->getId(),
        "EmissionColor"
    );

    _solidColorLocation = glGetUniformLocation(
        _shaderProgram->getId(),
        "SolidColor"
    );
}

BezierDistortionEffect::~BezierDistortionEffect()
{
}

void BezierDistortionEffect::destroy()
{
}

void BezierDistortionEffect::begin()
{
    _shaderProgram->use();

    glUniform3fv(_controlPointsLocation, 64, glm::value_ptr(_points[0]));
    glUniform3fv(_lightDirectionLocation, 1, glm::value_ptr(_lightDirection));
    glUniform3fv(_emissionColorLocation, 1, glm::value_ptr(_emissionColor));
    glUniform4fv(_solidColorLocation, 1, glm::value_ptr(_solidColor));
}

void BezierDistortionEffect::end()
{
}

void BezierDistortionEffect::setLightDirection(glm::vec3 lightDirection)
{
    _lightDirection = lightDirection;
}

void BezierDistortionEffect::setMaterial(const fw::Material& material)
{
    setEmissionColor(material.getEmissionColor());
    setSolidColor(material.getBaseAlbedoColor());
}

void BezierDistortionEffect::setEmissionColor(glm::vec3 color)
{
    _emissionColor = color;
}

void BezierDistortionEffect::setSolidColor(glm::vec3 color)
{
    _solidColor = glm::vec4{color, 1.0};
}

void BezierDistortionEffect::setSolidColor(glm::vec4 color)
{
    _solidColor = color;
}

void BezierDistortionEffect::setDistortionControlPoints(
    const std::vector<glm::vec3>& points
)
{
    _points = points;
}

void BezierDistortionEffect::createShaders()
{
    std::string vertName = std::string(cApplicationResourcesDir) + "shaders/"
      + "BezierCubeDistortion.vert";

    std::string fragName = std::string(cApplicationResourcesDir) + "shaders/"
      + "BezierCubeDistortion.frag";

    std::shared_ptr<fw::Shader> vs = std::make_shared<fw::Shader>();
    vs->addSourceFromFile(vertName);
    vs->compile(GL_VERTEX_SHADER);

    std::shared_ptr<fw::Shader> fs = std::make_shared<fw::Shader>();
    fs->addSourceFromFile(fragName);
    fs->compile(GL_FRAGMENT_SHADER);

    _shaderProgram = std::make_shared<fw::ShaderProgram>();
    _shaderProgram->attach(vs.get());
    _shaderProgram->attach(fs.get());
    _shaderProgram->link();
}

}
