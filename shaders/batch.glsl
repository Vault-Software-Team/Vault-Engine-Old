#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 g_texCoords;
layout(location = 3) in int diffuseTexture;
layout(location = 4) in int specularTexture;
layout(location = 5) in int normalMapTexture;
layout(location = 6) in float g_metallic;
layout(location = 7) in float g_roughness;
layout(location = 8) in vec3 color;
layout(location = 9) in vec2 texUvOffset;
layout(location = 10) in vec3 transformPosition;
layout(location = 11) in vec3 transformRotation;
layout(location = 12) in vec3 transformScale;

out vec4 gColor;
flat out float texture_diffuse0;
flat out float texture_specular0;
flat out float texture_normal0;

out vec2 g_g_texCoords;
out vec3 Normal;
out vec3 currentPosition;
out vec4 fragPosLight;
out vec3 reflectedVector;
out vec4 baseColor;

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
    x[1][1] = cos(v.x);
    x[1][2] = -sin(v.x);
    x[2][1] = sin(v.x);
    x[2][2] = cos(v.x);
    y[0][0] = cos(v.y);
    y[0][2] = sin(v.y);
    y[2][0] = -sin(v.y);
    y[2][2] = cos(v.y);
    z[0][0] = cos(v.z);
    z[0][1] = -sin(v.z);
    z[1][0] = sin(v.z);
    z[1][1] = cos(v.z);
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

    g_g_texCoords = g_texCoords;

    texture_diffuse0 = float(diffuseTexture);
    texture_specular0 = float(specularTexture);
    texture_normal0 = float(normalMapTexture);
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
flat in float texture_diffuse0;
flat in float texture_specular0;
flat in float texture_normal0;

in vec2 g_g_texCoords;
in vec3 Normal;
in vec3 reflectedVector;
in vec3 currentPosition;
in vec4 fragPosLight;

in float metallic;
in float roughness;

uniform samplerCube cubeMap;
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

struct Light2D {
    vec2 lightPos;
    vec3 color;
    float range;
};

uniform float ambient;
#define MAX_LIGHTS 50
uniform PointLight pointLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];
uniform DirectionalLight dirLights[MAX_LIGHTS];
uniform Light2D light2ds[MAX_LIGHTS];

uniform vec3 cameraPosition;
float specularTexture_g = texture(textures[int(texture_specular0)], g_g_texCoords).r;
vec4 reflectedColor = texture(cubeMap, reflectedVector);

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

    if(texture_diffuse0 != -1) {
        return (mix(texture(textures[int(texture_diffuse0)], g_g_texCoords), reflectedColor, metallic) * baseColor * (diffuse * inten) + specularTexture_g * ((specular * inten) * vec4(light.color, 1)))  * vec4(light.color, 1);
    } else {
        return (baseColor * (diffuse * inten) * ((specular * inten) * vec4(light.color, 1))) * vec4(light.color, 1);
    }
}

vec4 directionalLight(DirectionalLight light) {
    // float ambient = 0.4;

    vec3 normal;
    if(texture_diffuse0 != -1 && texture_normal0 != -1) {
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
    float shadow = 0;

    float _smoothness = 1 - roughness;
    if(_smoothness == 0) {
        specular = 0;
    }


    specular = specular * _smoothness;
    float shadowAdder = 1.0;

    if(texture_diffuse0 != -1) {
        return (mix(texture(textures[int(texture_diffuse0)], g_g_texCoords), reflectedColor, metallic) * baseColor * vec4(light.color, 1) * ((diffuse ) * shadowAdder) + specularTexture_g * (((specular * shadowAdder) * vec4(light.color, 1)) * light.intensity));
    } else {
        return (baseColor * vec4(light.color, 1) * ((diffuse) * shadowAdder) + vec4(1,1,1,1)  * (((specular * shadowAdder) * vec4(light.color, 1)) * light.intensity));
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

    if(texture_diffuse0 != -1) {
        return (mix(texture(textures[int(texture_diffuse0)], g_g_texCoords), reflectedColor, metallic) * baseColor * (diffuse * inten) + specularTexture_g * ((specular * inten) * vec4(light.color, 1)))  * vec4(light.color, 1);
    } else {
        return (baseColor * (diffuse * inten) * ((specular * inten) * vec4(light.color, 1))) * vec4(light.color, 1);
    }
}

vec4 light2d(Light2D light) {
    if(texture_diffuse0 != -1) {
        vec4 frag_color = mix(texture(textures[int(texture_diffuse0)], g_g_texCoords), reflectedColor, metallic) * baseColor;
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
    // sampler2D to int
    vec4 result = vec4(1.0f);
    result = mix(texture(textures[int(texture_diffuse0)], g_g_texCoords), reflectedColor, metallic) * ambient;

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

        if(light2ds[i].color.r > 0 || spotLights[i].color.g > 0 || spotLights[i].color.b > 0) {
            result += light2d(light2ds[i]);
        }
    }

    if(result.a < 0.1)
        discard;

    FragColor = result;
}