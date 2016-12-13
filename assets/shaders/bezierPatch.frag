#version 400 core

out vec4 color;

in vec3 teNormal;
in vec3 teTangent;
in vec3 teBinormal;
in vec3 tePosition;
in vec3 teLightDirection;
in vec2 teTexcoords;
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

vec3 phongModelViewSpace(
  vec3 viewNormal,
  vec3 viewDirection,
  vec3 viewLightDirection,
  float ambientStrength,
  float diffuseStrength,
  float specularStrength,
  float specularExponent,
  vec3 ambientLightColor,
  vec3 diffuseLightColor,
  vec3 specularLightColor
) {
  vec3 ambient = ambientStrength * ambientLightColor;

  float diffuseFactor = clamp(dot(viewNormal, viewLightDirection), 0.0, 1.0);
  vec3 diffuse = diffuseStrength * diffuseFactor * diffuseLightColor;

  vec3 reflectDir = reflect(viewLightDirection, viewNormal);
  float specularFactor = pow(
      max(dot(viewDirection, reflectDir), 0.0),
      specularExponent
  );
  vec3 specular = specularStrength * specularFactor * specularLightColor;

  return ambient + diffuse + specular;
}

void main() {
  vec3 normalColor = texture(normalTexture, teTexcoords).rgb;
  vec3 tbnNormal = normalize((normalColor-0.5)*2.0);
  vec3 transformedNormal = normalize(tbnNormal.x * teTangent + tbnNormal.y * teBinormal
    + tbnNormal.z * teNormal);

  vec3 lighting = phongModelViewSpace(
    transformedNormal,
    normalize(tePosition),
    normalize(teLightDirection),
    0.1,
    1.0,
    0.4,
    32.0,
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, 1.0, 1.0)
  );

  color = texture(diffuseTexture, teTexcoords) * vec4(lighting, 1);
}
