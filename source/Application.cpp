#include "Application.hpp"

#include <iostream>

#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "easylogging++.h"

#include "fw/Common.hpp"
#include "fw/DebugShapes.hpp"
#include "fw/Resources.hpp"
#include "fw/TextureUtils.hpp"

#include "Config.hpp"

namespace application
{

Application::Application():
    _roomSize{10.0f, 5.0f, 10.0f},
    _updatePhysicsEnabled{false},
    _enableGridPreview{false},
    _enableConstraintsPreview{false},
    _enableSoftBoxRendering{false},
    _enableRoomRendering{true},
    _enableObjectRendering{true},
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

    _softbodyTexture = std::make_shared<fw::Texture>(
        std::string(cApplicationResourcesDir) + "textures/normal.png"
    );

    _bezierDistortionEffect = std::make_shared<BezierDistortionEffect>();

    restartSimulation();

    _softBoxPreview = std::make_shared<SoftBoxPreview>();

    _phongEffect = std::make_shared<fw::TexturedPhongEffect>();
    _phongEffect->create();

    _universalPhongEffect = std::make_shared<fw::UniversalPhongEffect>();

    _cube = fw::createBox({1.0, 1.0, 1.0}, true);
    _sphere = std::make_shared<fw::Mesh<fw::VertexNormalTexCoords>>(
        fw::createSphere(0.5f, 64, 64)
    );

    _grid = std::make_shared<fw::Grid>(
        glm::ivec2{32, 32},
        glm::vec2{0.5f, 0.5f}
    );

    _roomMaterial = std::make_shared<fw::Material>();
    _roomMaterial->setBaseAlbedoColor({0.8f, 0.3f, 0.3f, 1.0f});

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

    loadSoftModel();
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
            ImGui::Checkbox("Mesh rendering", &_enableObjectRendering);
            ImGui::Checkbox("Room rendering", &_enableRoomRendering);
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

    auto lightDirection = glm::normalize(glm::vec3{-1.0f, 1.0f, 2.0f});
    _universalPhongEffect->setLightDirection(lightDirection);

    if (_enableConstraintsPreview)
    {
        for (const auto& chunk: _softBoxPreview->render(*_softBox.get()))
        {
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

    if (_enableRoomRendering)
    {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        _universalPhongEffect->setMaterial(*_roomMaterial.get());
        _universalPhongEffect->begin();
        _universalPhongEffect->setProjectionMatrix(_projectionMatrix);
        _universalPhongEffect->setViewMatrix(_camera.getViewMatrix());
        _universalPhongEffect->setModelMatrix(
            glm::scale(glm::mat4{}, _roomSize)
        );
        _cube->render();
        _universalPhongEffect->end();

        glFrontFace(GL_CCW);
    }

    if (_enableSoftBoxRendering)
    {
        glDisable(GL_CULL_FACE);
        //glDisable(GL_DEPTH_TEST);

        drawSoftBoxSide([](int i, int j) { return glm::ivec3{i, j, 0}; });
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{3-i, j, 3}; });
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{0, i, j}; });
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{3, 3-i, j}; });
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{3-i, 0, j}; });
        drawSoftBoxSide([](int i, int j) { return glm::ivec3{i, 3, j}; });

        //glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    if (_enableObjectRendering)
    {
        auto particles = _softBox->getSoftBoxParticles();
        std::vector<glm::vec3> controlPoints;
        std::transform(
            std::begin(particles),
            std::end(particles),
            std::back_inserter(controlPoints),
            [](const ParticleState& particle) { return particle.position; }
        );

        _bezierDistortionEffect->setDistortionControlPoints(controlPoints);
        _bezierDistortionEffect->begin();
        _bezierDistortionEffect->setProjectionMatrix(_projectionMatrix);
        _bezierDistortionEffect->setViewMatrix(_camera.getViewMatrix());
        _bezierDistortionEffect->setModelMatrix(
            glm::translate(glm::mat4{}, glm::vec3{0.5f, 0.5f, 0.5f})
        );
        _softModel->render();
        _bezierDistortionEffect->end();
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
    _bezierEffect->setTessellationLevelBump(0);
    _bezierEffect->setPatchU(0);
    _bezierEffect->setPatchV(0);
    _bezierEffect->setPatchesNumU(1);
    _bezierEffect->setPatchesNumV(1);

    _bezierEffect->setModelMatrix({});
    _bezierEffect->setViewMatrix(_camera.getViewMatrix());
    _bezierEffect->setProjectionMatrix(_projectionMatrix);
    _bezierEffect->setNormalTexture(_softbodyTexture->getTextureId());
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

void Application::loadSoftModel()
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        std::string(cApplicationResourcesDir) + "/models/bunny.obj",
        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs
    );

    if (!scene
        || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE
        || !scene->mRootNode)
    {
        LOG(ERROR)
            << "Assimp cannot load the scene. "
            << importer.GetErrorString();
        return;
    }

    if (scene->mNumMeshes == 0)
    {
        LOG(ERROR) << "No meshes found in file.";
        return;
    }

    aiMesh *mesh = scene->mMeshes[0];

    std::vector<fw::VertexNormalTexCoords> vertices;
    std::vector<GLuint> indices;

    for (auto i = 0; i < mesh->mNumVertices; ++i)
    {
        fw::VertexNormalTexCoords vertex;

        vertex.position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        vertex.normal = glm::vec3(
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z
        );

        vertices.push_back(vertex);
    }

    for (auto i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (auto j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    _softModel = std::make_shared<fw::Mesh<fw::VertexNormalTexCoords>>(
        vertices,
        indices
    );
}

}
