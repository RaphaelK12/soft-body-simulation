#pragma once

#include <string>
#include "glm/glm.hpp"
#include "fw/OpenGLHeaders.hpp"
#include "fw/Shaders.hpp"

namespace application
{

struct BezierPatchEffectUniformLocations {
    GLuint projectionMatrix;
    GLuint viewMatrix;
    GLuint modelMatrix;
    GLuint lightPosition;
    GLuint tessellationLevelBump;
    GLuint patchesNumU;
    GLuint patchesNumV;
    GLuint uPatch;
    GLuint vPatch;
    GLuint diffuseTexture;
    GLuint normalTexture;
};

class BezierPatchEffect
{
public:
    BezierPatchEffect();
    void initialize(std::string shaderName);

    void setProjectionMatrix(const glm::mat4 &projection);
    void setViewMatrix(const glm::mat4 &view);
    virtual void setModelMatrix(const glm::mat4 &model);
    void setLightPosition(const glm::vec3 &position);
    void setTessellationLevelBump(int levelBump);
    void setPatchesNumU(int uPatches);
    void setPatchesNumV(int vPatches);
    void setPatchU(int patch);
    void setPatchV(int patch);

    void setDiffuseTexture(GLuint texture);
    void setNormalTexture(GLuint texture);

    void begin();
    void end();

private:
    void getUniformLocations();
    std::shared_ptr<fw::ShaderProgram> _shaderProgram;
    BezierPatchEffectUniformLocations _uniforms;
};

}
