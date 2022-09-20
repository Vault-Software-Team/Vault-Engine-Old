#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 g_texCoords;

out DATA {
    vec3 Normal;
    vec3 color;
    vec2 texCoords;
    mat4 projection;
    vec3 currentPosition;
    vec3 reflectedVector;
    vec4 fragPosLight;
} data_out;

uniform mat4 camera;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 model;
uniform mat4 lightProjection;
uniform vec3 cameraPosition;

void main() {
    vec4 worldPosition = model * translation * rotation * scale * vec4(position, 1.0);
    vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);
    data_out.currentPosition = vec3(model * translation * rotation * scale * vec4(position, 1.0));
    
    gl_Position = vec4(data_out.currentPosition, 1.0);
    
    data_out.texCoords = g_texCoords;
    data_out.color = aColor;
    data_out.Normal = aNormal;
    data_out.projection = camera;
    data_out.fragPosLight = lightProjection * vec4(data_out.currentPosition, 1.0);
    data_out.reflectedVector = reflect(viewVector, aNormal);
}

#shader fragment
#version 330 core

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
    float intensity;
};

uniform float ambient;
#define MAX_POINT_LIGHTS 100
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_POINT_LIGHTS];

uniform int isTex;
uniform sampler2D shadowMap;
uniform samplerCube cubeMap;
uniform vec3 cameraPosition;

//Material properties
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_normal0;
uniform vec3 baseColor;
uniform float metallic;
uniform float smoothness;

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
    float _smoothness = smoothness;

	if(smoothness == 0.0) {
        specular = 0.0;
    }

    // add smoothness to the specular
    specular = specular * _smoothness;

    if(isTex == 1) {
        return (texture(texture_diffuse0, texCoords) * vec4(baseColor, 1) * vec4(light.color, 1) * (diffuse) + texture(texture_specular0, texCoords).r * (specular * vec4(light.color, 1)) * light.intensity);
    } else {
        return (vec4(baseColor, 1) * vec4(light.color, 1) * (diffuse) * (specular * vec4(light.color, 1)) * light.intensity);
    }
}

vec4 directionalLight() {
    // float ambient = 0.4;

    vec3 normal;
    if(isTex == 1 && texture(texture_normal0, texCoords).r != 0.0) {
        normal = normalize(texture(texture_normal0, texCoords).rgb * 2.0 - 1.0);
    } else {
        normal = normalize(Normal);
    }
    vec3 lightDir = normalize(vec3(1, 1, 0.0));
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

    float shadow = 0.0;
    vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
    if(lightCoords.z <= 1.0) {
        lightCoords = (lightCoords + 1) / 2;

        float closestDepth = texture(shadowMap, lightCoords.xy).r;
        float currentDepth = lightCoords.z;

        if(currentDepth > closestDepth) {
            shadow = 1.0;
        }
    }

    if(smoothness == 0.0 || smoothness == 0) {
        specular = 0;
    }

    specular = specular * smoothness;

	if(isTex == 1) {
        return (texture(texture_diffuse0, texCoords) * vec4(baseColor, 1) * vec4(1, 1, 1, 1) * (diffuse + ambient) + texture(texture_specular0, texCoords).r * (specular));
    } else {
        return (vec4(baseColor, 1) * vec4(1, 1, 1, 1) * (diffuse + ambient) + vec4(1,1,1,1) * (specular));
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

    if(smoothness == 0.0) {
        specular = 0.0;
    }

    if(isTex == 1) {
        return (texture(texture_diffuse0, texCoords)  * vec4(light.color, 1) * vec4(baseColor, 1) * (diffuse * inten)  + texture(texture_specular0, texCoords).r * specular * inten);
    } else {
        return (vec4(baseColor, 1)  * vec4(light.color, 1) * (diffuse * inten) + texture(texture_specular0, texCoords).r * specular * inten);
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
    vec4 result = vec4(0);

    if(isTex == 1) {
        result = texture(texture_diffuse0, texCoords) * ambient;
    } else {
        result = vec4(baseColor, 1) * ambient;
    }

    result += directionalLight();

    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        if(pointLights[i].intensity > 0) {
            result += pointLight(pointLights[i]);
        }
    }

    for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        if(spotLights[i].intensity > 0) {
            result += spotLight(spotLights[i]);
        }
    }

    vec4 reflectedColor = texture(cubeMap, reflectedVector);
    
    gl_FragColor = mix(result, reflectedColor, metallic);
    // gl_FragColor = result;
}

#shader geometry
#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 Normal;
out vec3 color;
out vec2 texCoords;
out vec3 currentPosition;
out vec3 reflectedVector;
out vec4 fragPosLight;

in DATA {
    vec3 Normal;
    vec3 color;
    vec2 texCoords;
    mat4 projection;
    vec3 currentPosition;
    vec3 reflectedVector;
    vec4 fragPosLight;
} data_in[];

void main() {
    for(int i = 0; i < gl_in.length(); i++) {
        gl_Position = data_in[i].projection * gl_in[i].gl_Position;
        Normal = data_in[i].Normal;
        color = data_in[i].color;
        texCoords = data_in[i].texCoords;
        currentPosition = data_in[i].currentPosition;
        reflectedVector = data_in[i].reflectedVector;
        fragPosLight = data_in[i].fragPosLight;
        EmitVertex();
    }

    EndPrimitive();
}