#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 g_texCoords;
layout(location = 4) in float g_texID;
layout(location = 5) in mat4 gModel;

out vec2 texCoords;
out vec3 Color;
out vec3 Normal;
out vec3 currentPosition;
out vec3 reflectedVector;
out float texID;

uniform mat4 camera;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 test[2];
uniform vec3 cameraPosition;

void main() {
    texID = g_texID;
    vec4 worldPosition = gModel * vec4(position, 1.0);
    currentPosition = vec3(gModel * vec4(position, 1.0));
    gl_Position = camera * vec4(currentPosition, 1.0);
    texCoords = g_texCoords;
    Color = color;
    Normal = aNormal;

    vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);
    reflectedVector = reflect(viewVector, Normal);
}

#shader fragment
#version 330 core

in vec2 texCoords;
in vec3 Color;
in vec3 Normal;
in vec3 currentPosition;
in vec3 reflectedVector;
in float texID;

struct PointLight {
    vec2 lightPos;
    vec3 color;
    float range;
};
uniform float ambient;
#define MAX_POINT_LIGHTS 100
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform int isTex;
uniform sampler2D shadowMap;
uniform samplerCube cubeMap;
uniform vec3 cameraPosition;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_normal0;
uniform vec4 baseColor;
uniform float metallic;
uniform float roughness;

uniform sampler2D textures[2];

vec4 pointLight(PointLight light) {
    if(isTex == 1) {
        vec4 frag_color = texture(texture_diffuse0, texCoords);
        if(frag_color.a < 1.0)
            discard;

        float distance = distance(light.lightPos, currentPosition.xy);
        float diffuse = 0.0;

        if (distance <= light.range)
            diffuse =  1.0 - abs(distance / light.range);

        return vec4(min(frag_color.rgb * ((light.color * diffuse)), frag_color.rgb), 1.0);
    } else {
        vec4 frag_color = baseColor;
        if(frag_color.a < 1.0)
            discard;

        float distance = distance(light.lightPos, currentPosition.xy);
        float diffuse = 0.0;

        if (distance <= light.range)
            diffuse =  1.0 - abs(distance / light.range);

        return vec4(frag_color.rgb * ((light.color * diffuse)), frag_color.rgb);
    }
}

void main() {
    // bool lights = false;
    vec4 result = vec4(0.0);
    // if(isTex == 1) {
    //     result = texture(texture_diffuse0, texCoords) * ambient;
    // } else {
    //     result = baseColor * ambient;
    // }

    // for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
    //     if(pointLights[i].range > 0) {
    //         result += pointLight(pointLights[i]);
    //     }
    // }

    // // check alpha
    // if(result.a < 0.1)
    //     discard;

    int TexID = int(texID);

    result = texture(textures[TexID], texCoords) * ambient;

    if(result.a < 0.1)
        discard;

    gl_FragColor = result;
}