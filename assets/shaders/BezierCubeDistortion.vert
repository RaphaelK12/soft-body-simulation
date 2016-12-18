#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out VSOut
{
    vec3 Normal;
} vsOut;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 BezierCubeControlPoints[64];

vec4 BernsteinBasis(float u)
{
    float u2 = u*u;
    float u3 = u*u2;
    return vec4(
        1 - 3*u + 3*u2 - u3,
        3*u - 6*u2 + 3*u3,
        3*u2 - 3*u3,
        u3
    );
}

vec4 BernsteinDerivativeBasis(float u)
{
    float u2 = u*u;
    return 3*vec4(
        2*u - u2 - 1,
        1 - 4*u + 3*u2,
        2*u - 3*u2,
        u2
    );
}

vec3 GetControlPoint(int x, int y, int z)
{
    return BezierCubeControlPoints[16 * z + 4 * y + x];
}

vec3 EvaluateBernsteinDistortion(vec3 p)
{
    vec4 basisx = BernsteinBasis(p.x);
    vec4 basisy = BernsteinBasis(p.y);
    vec4 basisz = BernsteinBasis(p.z);

    vec3 sum = vec3(0, 0, 0);

    for (int z = 0; z < 4; ++z)
    {
        for (int y = 0; y < 4; ++y)
        {
            for (int x = 0; x < 4; ++x)
            {
                float basisCoefficient = basisx[x] * basisy[y] * basisz[z];
                sum += GetControlPoint(x, y, z) * basisCoefficient;
            }
        }
    }

    return sum;
}

void main()
{
    vec3 modelPosition = (model* vec4(position, 1)).xyz;
    vec3 distortedPosition = EvaluateBernsteinDistortion(modelPosition);
    gl_Position = projection * view * vec4(distortedPosition, 1.0);
    vsOut.Normal = (transpose(inverse(model)) * vec4(normal, 0)).xyz;
}
