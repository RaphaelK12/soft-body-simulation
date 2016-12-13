#version 400 core

layout (location = 0) in vec3 position;
out vec3 vPosition;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main() {
  vPosition = (viewMatrix * modelMatrix * vec4(position, 1)).xyz;
}
