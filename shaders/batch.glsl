#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 g_texCoords;
layout(location = 4) in float diffuseTexture;
layout(location = 5) in float specularTexture;
layout(location = 6) in float normalMapTexture;
layout(location = 7) in float g_metallic;
layout(location = 8) in float g_roughness;
layout(location = 9) in vec2 texUvOffset;
layout(location = 10) in vec3 transformPosition;
layout(location = 11) in vec3 transformRotation;
layout(location = 12) in vec3 transformScale;

out vec4 gColor;
out float texture_diffuse0;
out float texture_specular0;
out float texture_normal0;

out vec2 g_g_texCoords;
out vec3 Normal;
out vec3 currentPosition;
out vec4 fragPosLight;
out vec3 reflectedVector;

out float metallic;
out float roughness;

uniform mat4 camera;
uniform vec3 cameraPosition;

// translate function
mat4 translate(vec3 v)
{
    mat4 m = mat4(1.0);
    m[3][0] = v.x;
    m[3][1] = v.y;
    m[3][2] = v.z;
    return m;
}

// rotate function (xyz are in degrees)
mat4 rotate(vec3 v)
{
    mat4 x = mat4(1.0);
    mat4 y = mat4(1.0);
    mat4 z = mat4(1.0);
    x[1][1] = cos(radians(v.x));
    x[1][2] = -sin(radians(v.x));
    x[2][1] = sin(radians(v.x));
    x[2][2] = cos(radians(v.x));
    y[0][0] = cos(radians(v.y));
    y[0][2] = sin(radians(v.y));
    y[2][0] = -sin(radians(v.y));
    y[2][2] = cos(radians(v.y));
    z[0][0] = cos(radians(v.z));
    z[0][1] = -sin(radians(v.z));
    z[1][0] = sin(radians(v.z));
    z[1][1] = cos(radians(v.z));
    return x * y * z;
}

// scale function
mat4 scale(vec3 v)
{
    mat4 m = mat4(1.0);
    m[0][0] = v.x;
    m[1][1] = v.y;
    m[2][2] = v.z;
    return m;
}

void main() {
    vec3 ggcolor = color;

    baseColor = vec4(ggcolor, 1.0f);
    mat4 model = mat4(1.0f);
    model = translate(transformPosition);
    model = model * rotate(transformRotation);
    model = model * scale(transformScale);

    vec4 worldPosition = model * vec4(position, 1.0);
    currentPosition = vec3(model * vec4(position, 1.0));

    vec2 finalCoords = texCoords;

    if(finalCoords.x > 0) {
        finalCoords.x = finalCoords.x + texUvOffset.x;
    }

    if(finalCoords.y > 0) {
        finalCoords.y = finalCoords.y + texUvOffset.y;
    }
    g_texCoords = finalCoords;

    texture_diffuse0 = diffuseTexture;
    texture_specular0 = specularTexture;
    texture_normal0 = normalMapTexture;
    metallic = g_metallic;
    roughness = g_roughness;
    Normal = mat3(transpose(inverse(model))) * aNormal;

    fragPosLight = mat4(1.0f) * vec4(currentPosition, 1.0);

    vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);
    reflectedVector = reflect(viewVector, Normal);

    gl_Position = camera * vec4(currentPosition, 1.0);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec4 baseColor;
in float texture_diffuse0;
in float texture_specular0;
in float texture_normal0;

in vec2 g_g_texCoords;
in vec3 Normal;
in vec3 reflectedVector;
in vec3 currentPosition;
in vec4 fragPosLight;

in float metallic;
in float roughness;

uniform sampler2D textures[32];

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

uniform samplerCube cubeMap;
uniform vec3 cameraPosition;

vec4 reflectedColor = texture(cubeMap, reflectedVector);
float specularTexture = texture(texture_specular0, g_texCoords).r;

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

    if(int(texture_diffuse0) > -1) {
        return (mix(texture(texture_diffuse0, g_texCoords), reflectedColor, metallic) * baseColor * (diffuse * inten) + specularTexture * ((specular * inten) * vec4(light.color, 1)))  * vec4(light.color, 1);
    } else {
        return (mix(baseColor, reflectedColor, metallic) * (diffuse * inten) * ((specular * inten) * vec4(light.color, 1))) * vec4(light.color, 1);
    }
}

vec4 directionalLight(DirectionalLight light) {
    // float ambient = 0.4;

    vec3 normal;
    normal = normalize(Normal);
    // if(int(texture_diffuse0) > -1 && hasNormalMap == 1) {
        // normal = normalize(texture(texture_normal0, g_texCoords).rgb * 2.0 - 1.0);
    // } else {
        // normal = normalize(Normal);
    // 
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
    
    float _smoothness = 1 - roughness;
    if(_smoothness == 0) {
        specular = 0;
    }


    specular = specular * _smoothness;

	if(int(texture_diffuse0) > -1) {
        return (mix(texture(texture_diffuse0, g_texCoords), reflectedColor, metallic) * baseColor * vec4(light.color, 1) * ((diffuse)) + specularTexture * (((specular) * vec4(light.color, 1)) * light.intensity));
    } else {
        return (mix(baseColor, reflectedColor, metallic) * vec4(light.color, 1) * ((diffuse)) + vec4(1,1,1,1)  * (((specular) * vec4(light.color, 1)) * light.intensity));
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

    if(int(texture_diffuse0) > -1) {
        return (mix(texture(texture_diffuse0, g_texCoords), reflectedColor, metallic) * baseColor * (diffuse * inten) + specularTexture * ((specular * inten) * vec4(light.color, 1)))  * vec4(light.color, 1);
    } else {
        return (mix(baseColor, reflectedColor, metallic) * (diffuse * inten) * ((specular * inten) * vec4(light.color, 1))) * vec4(light.color, 1);
    }
}

void main() {
    // sampler2D to int
    vec4 result = vec4(1.0f);
    
    if(texture_diffuse0 > -1) {
        // result = vec4(texture_diffuse0, texture_diffuse0, texture_diffuse0, 1.0f);
        result = texture(textures[int(texture_diffuse0)], g_g_texCoords) * ambient;
        // result = vec4(result.rgb, 1.0f) * gColor;
        // result = gColor;
    } else {
        result = gColor * ambient;
    }

    if(result.a < 0.1f) {
        discard;
    }
    
    FragColor = result;
}