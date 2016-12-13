#include "BezierPatchEffect.hpp"

#include <iostream>
#include <memory>
#include "glm/gtc/type_ptr.hpp"
#include "Config.hpp"

using namespace std;

namespace application
{

BezierPatchEffect::BezierPatchEffect() {
}

void BezierPatchEffect::initialize(string shaderName)
{
    string vertName = string(cApplicationResourcesDir) + "shaders/"
      + shaderName + ".vert";
    string tescName = string(cApplicationResourcesDir) + "shaders/"
      + shaderName + ".tesc";
    string teseName = string(cApplicationResourcesDir) + "shaders/"
      + shaderName + ".tese";
    string fragName = string(cApplicationResourcesDir) + "shaders/"
      + shaderName + ".frag";

    shared_ptr<fw::Shader> vert = make_shared<fw::Shader>();
    vert->addSourceFromFile(vertName);
    vert->compile(GL_VERTEX_SHADER);

    shared_ptr<fw::Shader> tesc = make_shared<fw::Shader>();
    tesc->addSourceFromFile(tescName);
    tesc->compile(GL_TESS_CONTROL_SHADER);

    shared_ptr<fw::Shader> tese = make_shared<fw::Shader>();
    tese->addSourceFromFile(teseName);
    tese->compile(GL_TESS_EVALUATION_SHADER);

    shared_ptr<fw::Shader> frag = make_shared<fw::Shader>();
    frag->addSourceFromFile(fragName);
    frag->compile(GL_FRAGMENT_SHADER);

    _shaderProgram = make_shared<fw::ShaderProgram>();
    _shaderProgram->attach(vert.get());
    _shaderProgram->attach(tesc.get());
    _shaderProgram->attach(tese.get());
    _shaderProgram->attach(frag.get());
    _shaderProgram->link();

    getUniformLocations();

    _shaderProgram->use();
    glUniform1i(_uniforms.diffuseTexture, 0);
    glUniform1i(_uniforms.normalTexture, 1);
}

void BezierPatchEffect::setProjectionMatrix(const glm::mat4 &projection)
{
    glUniformMatrix4fv(
        _uniforms.projectionMatrix,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );
}

void BezierPatchEffect::setViewMatrix(const glm::mat4 &view) {
    glUniformMatrix4fv(
        _uniforms.viewMatrix,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );
}

void BezierPatchEffect::setModelMatrix(const glm::mat4 &model) {
    glUniformMatrix4fv(
        _uniforms.modelMatrix,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );
}

void BezierPatchEffect::setLightPosition(const glm::vec3 &position) {
    glUniform3fv(
        _uniforms.lightPosition,
        1,
        glm::value_ptr(position)
    );
}

void BezierPatchEffect::setTessellationLevelBump(int levelBump) {
    glUniform1i(
        _uniforms.tessellationLevelBump,
        levelBump
    );
}

void BezierPatchEffect::setPatchesNumU(int uPatches) {
    glUniform1i(
        _uniforms.patchesNumU,
        uPatches
    );
}

void BezierPatchEffect::setPatchesNumV(int vPatches) {
    glUniform1i(
        _uniforms.patchesNumV,
        vPatches
    );
}

void BezierPatchEffect::setPatchU(int patch) {
    glUniform1i(
        _uniforms.uPatch,
        patch
    );
}

void BezierPatchEffect::setPatchV(int patch) {
    glUniform1i(
        _uniforms.vPatch,
        patch
    );
}

void BezierPatchEffect::setDiffuseTexture(GLuint texture) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
}

void BezierPatchEffect::setNormalTexture(GLuint texture) {
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture);
}

void BezierPatchEffect::begin() {
  glUseProgram(_shaderProgram->getId());
}

void BezierPatchEffect::end() {
}

void BezierPatchEffect::getUniformLocations() {
  memset(&_uniforms, 0, sizeof(BezierPatchEffectUniformLocations));
  _uniforms.projectionMatrix =
    glGetUniformLocation(_shaderProgram->getId(), "projectionMatrix");
  _uniforms.viewMatrix =
    glGetUniformLocation(_shaderProgram->getId(), "viewMatrix");
  _uniforms.modelMatrix =
    glGetUniformLocation(_shaderProgram->getId(), "modelMatrix");
  _uniforms.lightPosition =
    glGetUniformLocation(_shaderProgram->getId(), "lightPosition");
  _uniforms.tessellationLevelBump =
    glGetUniformLocation(_shaderProgram->getId(), "tessellationLevelBump");
  _uniforms.patchesNumU =
    glGetUniformLocation(_shaderProgram->getId(), "patchesNumU");
  _uniforms.patchesNumV =
    glGetUniformLocation(_shaderProgram->getId(), "patchesNumV");
  _uniforms.uPatch =
    glGetUniformLocation(_shaderProgram->getId(), "uPatch");
  _uniforms.vPatch =
    glGetUniformLocation(_shaderProgram->getId(), "vPatch");
  _uniforms.diffuseTexture =
    glGetUniformLocation(_shaderProgram->getId(), "diffuseTexture");
  _uniforms.normalTexture =
    glGetUniformLocation(_shaderProgram->getId(), "normalTexture");
}

}
