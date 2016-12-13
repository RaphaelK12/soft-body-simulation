#include "Application.hpp"

#include <iostream>

#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"

#include "fw/Common.hpp"
#include "fw/DebugShapes.hpp"
#include "fw/Resources.hpp"
#include "fw/TextureUtils.hpp"

namespace application
{

Application::Application():
    _updatePhysicsEnabled{false},
    _enableGridPreview{true},
    _enableConstraintsPreview{false},
    _enableSoftBoxRendering{true},
    _enableCameraRotations{false},
    _cameraRotationSensitivity{0.2, 0.2}
{
    setWindowSize({1920, 1080});
}

Application::~Application()
{
}

void Application::onCreate()
{
    ImGuiApplication::onCreate();

    _cubeOutline = fw::createBoxOutline({1.0f, 1.0f, 1.0f});
    _cubeOutlineMaterial = std::make_shared<fw::Material>();
    _cubeOutlineMaterial->setEmissionColor({0.0f, 1.0f, 0.0f});

    restartSimulation();

    _softBoxPreview = std::make_shared<SoftBoxPreview>();

    _phongEffect = std::make_shared<fw::TexturedPhongEffect>();
    _phongEffect->create();

    _universalPhongEffect = std::make_shared<fw::UniversalPhongEffect>();

    _cube = fw::createBox({1.0, 1.0, 1.0});
    _grid = std::make_shared<fw::Grid>(
        glm::ivec2{32, 32},
        glm::vec2{0.5f, 0.5f}
    );

    _testTexture = fw::loadTextureFromFile(
        fw::getFrameworkResourcePath("textures/checker-base.png")
    );

    _camera.rotate(fw::pi()/4, -3.0*fw::pi()/4);
    _camera.setDist(3.0f);

    updateProjectionMatrix();

    _bezierPatch = std::make_shared<BezierPatch>();
    _bezierPatch->createFlatGrid(3.0f, 3.0f);
    _bezierEffect = std::make_shared<BezierPatchEffect>();
    _bezierEffect->initialize("bezierPatch");
}

void Application::onDestroy()
{
    ImGuiApplication::onDestroy();
}

void Application::onUpdate(
    const std::chrono::high_resolution_clock::duration& deltaTime
)
{
    ImGuiApplication::onUpdate(deltaTime);

    if (ImGui::Begin("Soft Body Simulation"))
    {
        ImGui::Checkbox("Enable physics", &_updatePhysicsEnabled);

        if (ImGui::Button("Restart"))
        {
            restartSimulation();
        }

        if (ImGui::Button("Apply random disturbance"))
        {
            _softBox->applyRandomDisturbance();
        }

        _softBox->updateUserInterface();

        if (ImGui::CollapsingHeader("Visual"))
        {
            ImGui::Checkbox("Grid preview", &_enableGridPreview);
            ImGui::Checkbox("Constraints preview", &_enableConstraintsPreview);
            ImGui::Checkbox("Soft-box preview", &_enableSoftBoxRendering);
        }
    }
    ImGui::End();

    if (_updatePhysicsEnabled)
    {
        _softBox->update(std::chrono::duration<double>(deltaTime).count());
    }
}

void Application::onRender()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (_enableGridPreview)
    {
        _phongEffect->begin();
        _phongEffect->setProjectionMatrix(_projectionMatrix);
        _phongEffect->setViewMatrix(_camera.getViewMatrix());
        _phongEffect->setModelMatrix({});
        _phongEffect->setTexture(_testTexture);
        _grid->render();
        _phongEffect->end();
    }

    auto lightDirection = glm::normalize(glm::vec3{-1.0f, 1.0f, 0.0f});

    if (_enableConstraintsPreview)
    {
        for (const auto& chunk: _softBoxPreview->render(*_softBox.get()))
        {
            _universalPhongEffect->setLightDirection(lightDirection);
            _universalPhongEffect->setMaterial(*chunk.getMaterial().get());

            _universalPhongEffect->begin();
            _universalPhongEffect->setProjectionMatrix(_projectionMatrix);
            _universalPhongEffect->setViewMatrix(_camera.getViewMatrix());
            _universalPhongEffect->setModelMatrix(chunk.getModelMatrix());
            chunk.getMesh()->render();
            _universalPhongEffect->end();
        }
    }

    _universalPhongEffect->setMaterial(*_cubeOutlineMaterial.get());
    _universalPhongEffect->begin();
    _universalPhongEffect->setProjectionMatrix(_projectionMatrix);
    _universalPhongEffect->setViewMatrix(_camera.getViewMatrix());
    _universalPhongEffect->setModelMatrix(
        _softBox->getControlFrame().getModelMatrix()
    );
    _cubeOutline->render();
    _universalPhongEffect->end();

    if (_enableSoftBoxRendering)
    {
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{i, j, 0}; });
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{i, j, 3}; });
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{0, i, j}; });
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{3, i, j}; });
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{i, 0, j}; });
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{i, 3, j}; });
    }

    ImGuiApplication::onRender();
}

bool Application::onMouseButton(int button, int action, int mods)
{
    if (ImGuiApplication::onMouseButton(button, action, mods)) { return true; }

    if (GLFW_MOUSE_BUTTON_LEFT == button)
    {
        _enableCameraRotations = GLFW_PRESS == action;
    }

    return true;
}

bool Application::onMouseMove(glm::dvec2 newPosition)
{
    if (ImGuiApplication::onMouseMove(newPosition)) { return true; }

    if (_enableCameraRotations)
    {
        auto movement = getMouseMovement() * _cameraRotationSensitivity;
        _camera.rotate(movement.y, movement.x);
    }

    return true;
}

bool Application::onScroll(double xoffset, double yoffset)
{
    if (fw::ImGuiApplication::onScroll(xoffset, yoffset))
        return true;

    const double cMinimumDistance = 1.0;
    const double cMaximumDistance = 10.0;
    const double cZoomStep = 0.5;
    auto currentDistance = _camera.getDist();
    _camera.setDist(
        std::min(
            std::max(currentDistance + cZoomStep * yoffset, cMinimumDistance),
            cMaximumDistance
        )
    );

    return true;
}

bool Application::onResize()
{
    updateProjectionMatrix();
    return true;
}

void Application::updateProjectionMatrix()
{
    auto windowSize = getWindowSize();
    auto aspectRatio = static_cast<float>(windowSize.x) / windowSize.y;
    _projectionMatrix = glm::perspective(45.0f, aspectRatio, 0.5f, 100.0f);
}

void Application::drawSoftBoxSide(
    std::function<glm::ivec3(int,int)> coordTransformation
)
{
    std::vector<glm::vec3> controlPoints;
    for (auto i = 0; i < 4; ++i)
    {
        for (auto j = 0; j < 4; ++j)
        {
            glm::ivec3 coord = coordTransformation(i, j);
            controlPoints.push_back(
                _softBox->getSoftBoxParticle(coord).position
            );
        }
    }

    _bezierPatch->setControlPoints(controlPoints);
    _bezierEffect->begin();
    _bezierEffect->setModelMatrix({});
    _bezierEffect->setViewMatrix(_camera.getViewMatrix());
    _bezierEffect->setProjectionMatrix(_projectionMatrix);
    _bezierPatch->drawPatch();
    _bezierEffect->end();
}

void Application::restartSimulation()
{
    _softBox = std::make_shared<SoftBox>();
    _softBox->distributeUniformly({
        {-1.0, -1.0, -1.0},
        {+1.0, +1.0, +1.0}
    });
}

}
