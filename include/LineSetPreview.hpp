#pragma once

#include "fw/Vertices.hpp"
#include "fw/Mesh.hpp"

namespace application
{

class LineSetPreview:
    public fw::IMesh
{
public:
    LineSetPreview();
    ~LineSetPreview();

    virtual void destroy();
    virtual void render() const;

    void setVertices(const std::vector<fw::VertexColor>& vertices);
    void setIndices(const std::vector<GLuint>& indices);

private:
    void createBuffers();

    static const int cMaxVertices;
    static const int cMaxIndices;

    GLuint _vao, _vbo, _ebo;
    GLuint _numElements;
};

}

