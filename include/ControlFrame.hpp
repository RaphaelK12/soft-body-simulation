#pragma once
#include "glm/glm.hpp"
#include "fw/GeometryChunk.hpp"
#include "LineSetPreview.hpp"

namespace application
{

class ControlFrame
{
public:
    ControlFrame();

    void updateUserInterface();
    glm::mat4 getModelMatrix() const;

    inline float getFrameSize() { return _frameSize; }

private:
    float _frameSize;
    glm::vec3 _framePosition;
    glm::vec3 _frameOrientation;
};

}
