#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 g_texCoords;

out vec2 texCoords;
out vec3 Color;
out vec3 Normal;
out vec3 currentPosition;
out vec3 reflectedVector;

uniform mat4 camera;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 model;
uniform vec3 cameraPosition;

void main() {
    vec4 worldPosition = model * vec4(position, 1.0);
    currentPosition = vec3(model * translation * rotation * scale * vec4(position, 1.0));
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

struct PointLight {
    vec3 lightPos;
    vec3 color;
    float intensity;
};

struct SpotLight {
    vec3 lightPos;
    vec3 color;
    float intensity;
};

uniform float ambient;
#define MAX_POINT_LIGHTS 100
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_POINT_LIGHTS];

uniform int isTex;
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D shadowMap;
uniform samplerCube cubeMap;
uniform vec3 cameraPosition;
uniform vec3 DefaultColor;

// //pbr stuff
// uniform vec3 albedo;
// uniform float metallic;
// uniform float roughness;
// uniform vec3 emissivity;
// uniform vec3 baseReflectance;

// math garbage poopoo

// end of math garbage

vec4 pointLight(PointLight light) {
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.lightPos - currentPosition);
    float diffuse = max(dot(normal, lightDir), 0.0);

    float specular = 0;

    if(diffuse != 0.0f) {
        float specularLight = 0.5;
        vec3 viewDirection = normalize(cameraPosition - currentPosition);
        vec3 reflectDir = reflect(-lightDir, normal);
        
        vec3 halfwayVec = normalize(viewDirection + lightDir);
        
        float specAmount = pow(max(dot(normal, halfwayVec), 0.0), 16);
        specular = specAmount * specularLight;
    }

	if(isTex == 1) {
        return (texture(diffuse0, texCoords) * vec4(light.color, 1) * (diffuse) + texture(specular0, texCoords).r * (specular * vec4(light.color, 1)) * light.intensity);
    } else {
        return (vec4(DefaultColor, 1) * vec4(light.color, 1) * (diffuse) * (specular * vec4(light.color, 1)) * light.intensity);
    }
}

vec4 directionalLight() {
    // float ambient = 0.4;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(vec3(1, 1, 0));
    float diffuse = max(dot(normal, lightDir), 0.0);

    float specular = 0;

    if(diffuse != 0.0f) {
        float specularLight = 0.5;
        vec3 viewDirection = normalize(cameraPosition - currentPosition);
        vec3 reflectDir = reflect(-lightDir, normal);
        
        vec3 halfwayVec = normalize(viewDirection + lightDir);
        
        float specAmount = pow(max(dot(normal, halfwayVec), 0.0), 16);
        specular = specAmount * specularLight;
    }

	if(isTex == 1) {
        return (texture(diffuse0, texCoords) * (diffuse + ambient) + texture(specular0, texCoords).r * specular);
    } else {
        return (vec4(DefaultColor, 1) * (diffuse + ambient) * specular);
    }
}

vec4 spotLight(SpotLight light) {
    float outerCone = 0.90;
    float innerCone = 0.95;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.lightPos - currentPosition);

    float diffuse = max(dot(normal, lightDir), 0.0);
    
    float specularLight = 0.50f;
	vec3 viewDirection = normalize(cameraPosition - currentPosition);
	vec3 reflectionDirection = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

    float angle = dot(vec3(0, -1, 0), -lightDir);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0, 1) * light.intensity;

    if(isTex == 1) {
        return (texture(diffuse0, texCoords)  * vec4(light.color, 1) * (diffuse * inten)  + texture(specular0, texCoords).r * specular * inten);
    } else {
        return (vec4(DefaultColor, 1)  * vec4(light.color, 1) * (diffuse * inten) + texture(specular0, texCoords).r * specular * inten);
    }
}

float near = 0.1;
float far = 100.0;
float linearizeDepth(float depth) {
    return (2 * near * far) / (far + near - (depth * 2 - 1) * (far - near));
}

float logisticDepth(float depth, float steepness = 0.5, float offset = 0.5) {
    float zVal = linearizeDepth(depth);
    return (1 / (1 + exp(-steepness * (zVal - offset))));
}

vec4 fog() {
    float depth = logisticDepth(gl_FragCoord.z);
    return (directionalLight() * (1 - depth) + vec4(depth * vec3(0.85, 0.85, 0.90), 1));
}

void main() {
    gl_FragColor = vec4(DefaultColor, 1);
    // gl_FragColor = result;
}