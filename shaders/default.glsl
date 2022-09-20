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
out vec4 fragPosLight;

uniform mat4 camera;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 model;
uniform mat4 lightProjection;
uniform vec3 cameraPosition;
uniform vec2 texUvOffset;

void main() {
    vec4 worldPosition = model * translation * rotation * scale * vec4(position, 1.0);
    currentPosition = vec3(model * translation * rotation * scale * vec4(position, 1.0));
    gl_Position = camera * vec4(currentPosition, 1.0);

    vec2 finalCoords = g_texCoords;

    if(finalCoords.x > 0) {
        finalCoords.x = finalCoords.x + texUvOffset.x;
    }

    if(finalCoords.y > 0) {
        finalCoords.y = finalCoords.y + texUvOffset.y;
    }

    texCoords = finalCoords;
    Color = color;
    // make normal apply rotation
    Normal = mat3(transpose(inverse(model))) * aNormal;
    // Normal = aNormal;

    fragPosLight = lightProjection * vec4(currentPosition, 1.0);

    vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);
    reflectedVector = reflect(viewVector, Normal);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 texCoords;
in vec3 Color;
in vec3 Normal;
in vec3 currentPosition;
in vec3 reflectedVector;
in vec4 fragPosLight;

struct PointLight {
    vec3 lightPos;
    vec3 color;
    float intensity;
};

struct SpotLight {
    vec3 lightPos;
    vec3 color;
    vec3 angle;
};

struct DirectionalLight {
    vec3 lightPos;
    vec3 color;
    float intensity;
};

uniform float ambient;
#define MAX_LIGHTS 100
uniform PointLight pointLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];
uniform DirectionalLight dirLights[MAX_LIGHTS];

uniform int isTex;
uniform samplerCube cubeMap;
uniform vec3 cameraPosition;

//Material properties
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_normal0;
uniform vec4 baseColor;
uniform float metallic;
uniform float roughness;

//texture setters
uniform int hasNormalMap;

//global textures
uniform sampler2D shadowMap;

vec4 reflectedColor = texture(cubeMap, reflectedVector);

float specularTexture = texture(texture_specular0, texCoords).r;

vec4 pointLight(PointLight light) {
    float specular = 0;
    vec3 lightVec = light.lightPos - currentPosition;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightVec);
    float diffuse = max(dot(normal, lightDir), 0.0);

	float dist = length(lightVec);
	float a = 1.0;
	float b = 0.04;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);
    inten *= light.intensity;

    if(diffuse != 0.0f) {
        float specularLight = 0.5;
        vec3 viewDirection = normalize(cameraPosition - currentPosition);
        vec3 reflectDir = reflect(-lightDir, normal);
        
        vec3 halfwayVec = normalize(viewDirection + lightDir);
        
        float specAmount = pow(max(dot(normal, halfwayVec), 0.0), 16);
        specular = specAmount * specularLight;
    }

    float _smoothness = 1 - roughness;

	if(_smoothness == 0.0) {
        specular = 0.0;
    }

    // add smoothness to the specular
    specular = specular * _smoothness;

    if(isTex == 1) {
        return (mix(texture(texture_diffuse0, texCoords), reflectedColor, metallic) * baseColor * (diffuse * inten) + specularTexture * ((specular * inten) * vec4(light.color, 1)))  * vec4(light.color, 1);
    } else {
        return (mix(baseColor, reflectedColor, metallic) * (diffuse * inten) * ((specular * inten) * vec4(light.color, 1))) * vec4(light.color, 1);
    }
}

vec4 directionalLight(DirectionalLight light) {
    // float ambient = 0.4;

    vec3 normal;
    if(isTex == 1 && hasNormalMap == 1) {
        normal = normalize(Normal);
        // normal = normalize(texture(texture_normal0, texCoords).rgb * 2.0 - 1.0);
    } else {
        normal = normalize(Normal);
    }
    vec3 lightDir = normalize(light.lightPos);
    float diffuse = max(dot(normal, lightDir), 0.0);

    float specular = 0;
    float inten = 0.3;

    if(diffuse != 0.0f) {
        float specularLight = 0.5;
        vec3 viewDirection = normalize(cameraPosition - currentPosition);
        vec3 reflectDir = reflect(-lightDir, normal);
        
        vec3 halfwayVec = normalize(viewDirection + lightDir);
        
        float specAmount = pow(max(dot(normal, halfwayVec), 0.0), 16);
        specular = specAmount * specularLight;
    }

    // shadows
    float shadow = 0.0;
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    vec2 UVCoords;
    UVCoords.x = projCoords.x * 0.5 + 0.5;
    UVCoords.y = projCoords.y * 0.5 + 0.5;
    
    float z = 0.5 * projCoords.z + 0.5;
    float Depth = texture(shadowMap, UVCoords).r;

    float bias = 0.0025;
    if(Depth + bias < z) {
        shadow = 0.5;
    } else {
        shadow = 1.0;
    }

    float _smoothness = 1 - roughness;
    if(_smoothness == 0) {
        specular = 0;
    }


    specular = specular * _smoothness;
    float shadowAdder = 1.0;

	if(isTex == 1) {
        return (mix(texture(texture_diffuse0, texCoords), reflectedColor, metallic) * baseColor * vec4(light.color, 1) * ((diffuse) * shadowAdder) + specularTexture * (((specular * shadowAdder) * vec4(light.color, 1)) * light.intensity));
    } else {
        return (mix(baseColor, reflectedColor, metallic) * vec4(light.color, 1) * ((diffuse) * shadowAdder) + vec4(1,1,1,1)  * (((specular * shadowAdder) * vec4(light.color, 1)) * light.intensity));
    }
}

vec4 spotLight(SpotLight light) {
    float outerCone = 0.9;
    float innerCone = 0.95;
    
    float specular = 0;
    vec3 lightVec = light.lightPos - currentPosition;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightVec);
    float diffuse = max(dot(normal, lightDir), 0.0);

    if(diffuse != 0.0f) {
        float specularLight = 0.5;
        vec3 viewDirection = normalize(cameraPosition - currentPosition);
        vec3 reflectDir = reflect(-lightDir, normal);
        
        vec3 halfwayVec = normalize(viewDirection + lightDir);
        
        float specAmount = pow(max(dot(normal, halfwayVec), 0.0), 16);
        specular = specAmount * specularLight;
    }
    float _smoothness = 1 - roughness;

	if(_smoothness == 0.0) {
        specular = 0.0;
    }

    float angle = dot(light.angle, -lightDir);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0, 1.0);

    // add smoothness to the specular
    specular = specular * _smoothness;

    if(isTex == 1) {
        return (mix(texture(texture_diffuse0, texCoords), reflectedColor, metallic) * baseColor * (diffuse * inten) + specularTexture * ((specular * inten) * vec4(light.color, 1)))  * vec4(light.color, 1);
    } else {
        return (mix(baseColor, reflectedColor, metallic) * (diffuse * inten) * ((specular * inten) * vec4(light.color, 1))) * vec4(light.color, 1);
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

// vec4 fog() {
//     float depth = logisticDepth(gl_FragCoord.z);
//     return (directionalLight() * (1 - depth) + vec4(depth * vec3(0.85, 0.85, 0.90), 1));
// }

void main() {
    if(specularTexture == 0) {
        specularTexture = texture(texture_diffuse0, texCoords).r;
    }

    vec4 result = vec4(0);

    if(isTex == 1) {
        result = mix(texture(texture_diffuse0, texCoords), reflectedColor, metallic) * baseColor * ambient;
    } else {
        result = mix(baseColor, reflectedColor, metallic) * ambient;
    }

    // discard if the alpha is 0
    if(result.a < 0.1) {
        discard;
    }

    // result += directionalLight();

    for(int i = 0; i < MAX_LIGHTS; i++) {
        if(pointLights[i].intensity > 0) {
            result += pointLight(pointLights[i]);
        }

        if(spotLights[i].color.r > 0 || spotLights[i].color.g > 0 || spotLights[i].color.b > 0) {
            result += spotLight(spotLights[i]);
        }

        if(dirLights[i].intensity == 1) {
            result += directionalLight(dirLights[i]);
        }
    }
    
    FragColor = result;
    // FragColor = directionalLight(dirLights[0]);
    // FragColor = result;
    
}