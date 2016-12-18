#version 330 core

in VSOut
{
    vec3 Normal;
} vsOut;

out vec4 color;

uniform vec3 LightColor;
uniform vec3 LightDirection;
uniform vec3 EmissionColor;
uniform vec4 SolidColor;

void main(void)
{
    vec3 lightDirection = normalize(LightDirection);
    float diffuse = dot(lightDirection, normalize(vsOut.Normal));

    vec3 albedo = SolidColor.rgb;

    vec3 ambientLight = vec3(0.2, 0.2, 0.2);
    vec3 ambientPart = clamp(albedo * ambientLight, 0, 1);
    vec3 diffusePart = clamp(albedo * diffuse, 0, 1);

    color = vec4(EmissionColor + ambientPart + diffusePart, SolidColor.a);
}

