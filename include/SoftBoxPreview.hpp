#pragma once

#include <vector>

#include "fw/GeometryChunk.hpp"
#include "fw/Mesh.hpp"
#include "fw/Vertices.hpp"

#include "LineSetPreview.hpp"
#include "SoftBox.hpp"

namespace application
{

class SoftBoxPreview
{
public:
    SoftBoxPreview();
    ~SoftBoxPreview();

    std::vector<fw::GeometryChunk> render(const SoftBox& softBox) const;

private:
    std::shared_ptr<fw::Mesh<fw::VertexNormalTexCoords>> _box;
    std::shared_ptr<LineSetPreview> _lineSetPreview;
    float _particleMarkerSize;
};

}
