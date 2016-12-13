#version 400 core
layout(vertices = 16) out;

in vec3 vPosition[];
out vec3 tcPosition[];

uniform int tessellationLevelBump;

const float invLog10 = 1.0 / log(10);

float factor(float z) {
  return max(2, -16 * log(z * 0.01) * invLog10);
}

float estimateTessalationLevel(vec3 v0, vec3 v1, vec3 v2, vec3 v3) {
  float closest = min(
    length(v0),
    min(
      length(v1),
      min(
        length(v2),
        length(v3)
      )
    )
  );

  return factor(closest);
}

void main() {
  tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];

  if (gl_InvocationID == 0) {
    gl_TessLevelOuter[1] = tessellationLevelBump + estimateTessalationLevel(
      vPosition[0], vPosition[1], vPosition[2], vPosition[3]
    );

    gl_TessLevelOuter[0] = tessellationLevelBump + estimateTessalationLevel(
      vPosition[0], vPosition[4], vPosition[8], vPosition[12]
    );

    gl_TessLevelOuter[3] = tessellationLevelBump + estimateTessalationLevel(
      vPosition[12], vPosition[13], vPosition[15], vPosition[15]
    );

    gl_TessLevelOuter[2] = tessellationLevelBump + estimateTessalationLevel(
      vPosition[3], vPosition[7], vPosition[11], vPosition[15]
    );

    float outerMaximum = max(
      gl_TessLevelOuter[0],
      max(
        gl_TessLevelOuter[1],
        max(
          gl_TessLevelOuter[2],
          gl_TessLevelOuter[3]
        )
      )
    );

    gl_TessLevelInner[0] = outerMaximum;
    gl_TessLevelInner[1] = outerMaximum;
  }
}

