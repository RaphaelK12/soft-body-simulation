#include "ControlFrame.hpp"
#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/euler_angles.hpp"

namespace application
{

ControlFrame::ControlFrame():
    _frameSize{2.0f},
    _frameSpringConstant{10.0},
    _frameSpringAttenuation{0.0f}
{
}

void ControlFrame::updateUserInterface()
{
    if (ImGui::CollapsingHeader("Control frame"))
    {
        ImGui::DragFloat3("Position", glm::value_ptr(_framePosition), 0.1f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(_frameOrientation), 0.01f);

        ImGui::SliderFloat(
            "Frame spring constant",
            &_frameSpringConstant,
            0.1f,
            100.0f
        );

        ImGui::SliderFloat(
            "Frame spring attenuation",
            &_frameSpringAttenuation,
            0.0f,
            20.0f
        );
    }
}

glm::mat4 ControlFrame::getModelMatrix() const
{
    auto scaling = glm::scale(
        glm::mat4(),
        {_frameSize, _frameSize, _frameSize}
    );

    auto translation = glm::translate(glm::mat4(), _framePosition);
    auto rotation = glm::orientate4(_frameOrientation);

    return translation * rotation * scaling;
}

}
