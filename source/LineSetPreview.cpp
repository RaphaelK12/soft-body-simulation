#include "LineSetPreview.hpp"
#include "easylogging++.h"

namespace application
{

const int LineSetPreview::cMaxVertices = 4*4*4+10;
const int LineSetPreview::cMaxIndices = 24*2*3*3*3+10;

LineSetPreview::LineSetPreview():
    _vao{},
    _vbo{},
    _ebo{},
    _numElements{}
{
    createBuffers();
}

LineSetPreview::~LineSetPreview()
{
}

void LineSetPreview::destroy()
{
}

void LineSetPreview::render() const
{
    glBindVertexArray(_vao);
    glDrawElements(GL_LINES, _numElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void LineSetPreview::setVertices(const std::vector<fw::VertexColor>& vertices)
{
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(fw::VertexColor) * vertices.size(),
        vertices.data()
    );

    fw::VertexColor::setupAttribPointers();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void LineSetPreview::setIndices(const std::vector<GLuint>& indices)
{
    glBindVertexArray(_vao);

    glBufferSubData(
        GL_ELEMENT_ARRAY_BUFFER,
        0,
        sizeof(GLuint) * indices.size(),
        indices.data()
    );

    glBindVertexArray(0);

    _numElements = indices.size();
}

void LineSetPreview::createBuffers()
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(fw::VertexColor) * cMaxVertices,
        nullptr,
        GL_DYNAMIC_DRAW
    );

    fw::VertexColor::setupAttribPointers();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * cMaxIndices,
        nullptr,
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);

    LOG(DEBUG) << "Created buffers: vao=" << _vao
        << " containing " << _numElements << " elements.";
}

}
