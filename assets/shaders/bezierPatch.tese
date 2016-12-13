#version 400 core
layout(quads) in;

in vec3 tcPosition[];

out vec3 tePosition;
out vec4 tePatchDistance;
out vec3 teNormal;
out vec3 teTangent;
out vec3 teBinormal;
out vec3 teLightDirection;
out vec2 teTexcoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightPosition;
uniform int patchesNumU;
uniform int patchesNumV;
uniform int uPatch;
uniform int vPatch;
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

vec4 bernsteinBasis(float u) {
  float u2 = u*u;
  float u3 = u*u2;
  return vec4(
    1 - 3*u + 3*u2 - u3,
    3*u - 6*u2 + 3*u3,
    3*u2 - 3*u3,
    u3
  );
}

vec4 bernsteinDerivativeBasis(float u) {
  float u2 = u*u;
  return 3*vec4(
    2*u - u2 - 1,
    1 - 4*u + 3*u2,
    2*u - 3*u2,
    u2
  );
}

const float invLog10 = 1.0 / log(10);

float factor(float z) {
  return max(2, -16 * log(z * 0.01) * invLog10);
}

float getMipLevel(float z) {
  return 6 - log(factor(z));
}

void main() {
  float u = gl_TessCoord.x, v = gl_TessCoord.y;
  tePatchDistance = vec4(u, v, 1-u, 1-v);

  float patchPartSizeU = 1.0 / patchesNumU;
  float patchPartSizeV = 1.0 / patchesNumV;
  float texU = uPatch*patchPartSizeU + u*patchPartSizeU;
  float texV = vPatch*patchPartSizeU + v*patchPartSizeV;
  teTexcoords = vec2(texU, texV);

  mat4 Px = mat4(
    tcPosition[0].x, tcPosition[1].x, tcPosition[2].x, tcPosition[3].x,
    tcPosition[4].x, tcPosition[5].x, tcPosition[6].x, tcPosition[7].x,
    tcPosition[8].x, tcPosition[9].x, tcPosition[10].x, tcPosition[11].x,
    tcPosition[12].x, tcPosition[13].x, tcPosition[14].x, tcPosition[15].x);

  mat4 Py = mat4(
    tcPosition[0].y, tcPosition[1].y, tcPosition[2].y, tcPosition[3].y,
    tcPosition[4].y, tcPosition[5].y, tcPosition[6].y, tcPosition[7].y,
    tcPosition[8].y, tcPosition[9].y, tcPosition[10].y, tcPosition[11].y,
    tcPosition[12].y, tcPosition[13].y, tcPosition[14].y, tcPosition[15].y);

  mat4 Pz = mat4(
    tcPosition[0].z, tcPosition[1].z, tcPosition[2].z, tcPosition[3].z,
    tcPosition[4].z, tcPosition[5].z, tcPosition[6].z, tcPosition[7].z,
    tcPosition[8].z, tcPosition[9].z, tcPosition[10].z, tcPosition[11].z,
    tcPosition[12].z, tcPosition[13].z, tcPosition[14].z, tcPosition[15].z);

  vec4 uBasis = bernsteinBasis(u);
  vec4 vBasis = bernsteinBasis(v);
  vec4 uDerivativeBasis = bernsteinDerivativeBasis(u);
  vec4 vDerivativeBasis = bernsteinDerivativeBasis(v);

  float x = dot(uBasis, Px * vBasis);
  float y = dot(uBasis, Py * vBasis);
  float z = dot(uBasis, Pz * vBasis);
  vec3 viewPosition = vec3(x, y, z);

  float dux = dot(uDerivativeBasis, Px * vBasis);
  float duy = dot(uDerivativeBasis, Py * vBasis);
  float duz = dot(uDerivativeBasis, Pz * vBasis);
  teTangent = normalize(vec3(dux, duy, duz));

  float dvx = dot(uBasis, Px * vDerivativeBasis);
  float dvy = dot(uBasis, Py * vDerivativeBasis);
  float dvz = dot(uBasis, Pz * vDerivativeBasis);
  teBinormal = normalize(vec3(dvx, dvy, dvz));

  teNormal = normalize(cross(teBinormal, teTangent));

  tePosition = viewPosition;

  vec3 viewLightPosition = (viewMatrix * vec4(lightPosition, 1)).xyz;
  teLightDirection = normalize(viewLightPosition - viewPosition);

  gl_Position = projectionMatrix * vec4(viewPosition, 1);
}
