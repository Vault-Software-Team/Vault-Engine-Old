#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 g_texCoords;
layout(location = 4) in ivec4 boneIds;
layout(location = 5) in vec4 weights;
layout(location = 6) in vec3 tangent;
layout(location = 7) in vec3 bitangent;

uniform mat4 camera;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform vec3 cameraPosition;
uniform vec2 texUvOffset;

out DATA {
    vec2 texCoords;
    vec3 Color;
    vec3 Normal;
    vec3 currentPosition;
    vec3 reflectedVector;
    vec4 fragPosLight;
    mat4 projection;
    mat4 model;
    vec3 T;
    vec3 B;
    vec3 N;
} data_out;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main() {
    vec4 totalPosition = vec4(0);

    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) {
            continue;
        }

        if(boneIds[i] >=MAX_BONES)
        {
            totalPosition = vec4(position, 1.0f);
            break;
        }

        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(position, 1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
    }
    if(totalPosition == vec4(0))
    {
        totalPosition = vec4(position, 1.0f);
    }

    vec4 worldPosition = model * translation * rotation * scale * totalPosition;
    data_out.currentPosition = vec3(model * translation * rotation * scale * totalPosition);
    data_out.projection = camera;
    gl_Position = vec4(data_out.currentPosition, 1.0);
    data_out.model = model * translation * rotation * scale;

    vec2 finalCoords = g_texCoords;

    if(finalCoords.x > 0) {
        finalCoords.x = finalCoords.x + texUvOffset.x;
    }

    if(finalCoords.y > 0) {
        finalCoords.y = finalCoords.y + texUvOffset.y;
    }

    data_out.texCoords = finalCoords;
    data_out.Color = color;
    // make normal apply rotation
    data_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    // Normal = aNormal;

    data_out.fragPosLight = lightSpaceMatrix * vec4(data_out.currentPosition, 1.0);

    vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);
    data_out.reflectedVector = reflect(viewVector, data_out.Normal);

    vec3 T   = normalize(mat3(model) * tangent);
    vec3 B   = normalize(mat3(model) * bitangent);
    vec3 N   = normalize(mat3(model) * data_out.Normal);

    data_out.T = T;
    data_out.B = B;
    data_out.N = N;
}
#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;
layout(location = 2) out uint EntityID;

in vec2 texCoords;
in vec3 Color;
in vec3 Normal;
in vec3 currentPosition;
in vec3 reflectedVector;
in vec4 fragPosLight;
in mat3 m_TBN;

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
#define MAX_LIGHTS 60
uniform PointLight pointLights[MAX_LIGHTS];
uniform Light2D light2ds[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];
uniform DirectionalLight dirLights[MAX_LIGHTS];

uniform int isTex;
uniform samplerCube cubeMap;
uniform vec3 cameraPosition;

//Material properties
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_normal0;
uniform sampler2D texture_height0;
uniform sampler2D texture_emission0;
uniform vec4 baseColor;
uniform vec3 u_BloomColor;
uniform float metallic;
uniform float roughness;

//texture setters
uniform int hasNormalMap;
uniform int hasHeightMap;

//global textures
uniform sampler2D shadowMap;

vec4 reflectedColor = texture(cubeMap, reflectedVector);
float specularTexture = texture(texture_specular0, texCoords).r;

float near = 0.1;
float far = 100.0;

uniform uint u_EntityID;

// Post Processing uniforms
uniform int globalBloom;
uniform float bloomThreshold;

uniform float DeltaTime;
vec2 uv = texCoords;
vec2 fragCoord = gl_FragCoord.xy;
vec2 iResolution = textureSize(texture_diffuse0, 0);
#define iTime DeltaTime
#define iChannel0 texture_diffuse0
#define fragColor FragColor
#define mainImage main

const float PI = 3.14159265;

float wobble_intensity = 0.002;
float grade_intensity = 0.5;
float line_intensity = 2.;
float vignette_intensity = 0.2;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float sample_noise(vec2 fragCoord)
{
    return pow(0.1, 7.); //  sharper ramp
}

void mainImage()
{
    //  wobble
    vec2 wobbl = vec2(wobble_intensity * rand(vec2(iTime, fragCoord.y)), 0.);
    
    //  band distortion
    float t_val = tan(0.25 * iTime + uv.y * PI * .67);
    vec2 tan_off = vec2(wobbl.x * min(0., t_val), 0.);
    
    //  chromab
    vec4 color1 = texture(iChannel0, uv + wobbl + tan_off);
    vec4 color2 = texture(iChannel0, (uv + (wobbl * 1.5) + (tan_off * 1.3)) * 1.005);
    //  combine + grade
    vec4 color = vec4(color2.rg, pow(color1.b, .67), 1.);
    color.rgb = mix(texture(iChannel0, uv + tan_off).rgb, color.rgb, grade_intensity);
    
    //  scanline sim
    float s_val = ((sin(2. * PI * uv.y + iTime * 20.) + sin(2. * PI * uv.y)) / 2.) * .015 * sin(iTime);
    color += s_val;
    
    //  noise lines
    float ival = iResolution.y / 4.;
    float r = rand(vec2(iTime, fragCoord.y));
    //  dirty hack to avoid conditional
    float on = floor(float(int(fragCoord.y + (iTime * r * 1000.)) % int(ival + line_intensity)) / ival);
    float wh = sample_noise(fragCoord) * on;
    color = vec4(min(1., color.r + wh), 
                 min(1., color.g + wh),
                 min(1., color.b + wh), 1.);
    
    float vig = 1. - sin(PI * uv.x) * sin(PI * uv.y);
    
    fragColor = color - (vig * vignette_intensity);
    BloomColor = vec4(0);
}

#shader geometry
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec2 texCoords;
out vec3 Color;
out vec3 Normal;
out vec3 currentPosition;
out vec3 reflectedVector;
out vec4 fragPosLight;
out mat3 m_TBN;

in DATA {
    vec2 texCoords;
    vec3 Color;
    vec3 Normal;
    vec3 currentPosition;
    vec3 reflectedVector;
    vec4 fragPosLight;
    mat4 projection;
    mat4 model;
    vec3 T;
    vec3 B;
    vec3 N;
} data_in[];

void main() {
    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
    Normal = data_in[0].Normal;
    Color = data_in[0].Color;
    currentPosition = gl_in[0].gl_Position.xyz;
    reflectedVector = data_in[0].reflectedVector;
    fragPosLight = data_in[0].fragPosLight;
    texCoords = data_in[0].texCoords;
    m_TBN = mat3(1.0f);
    EmitVertex();

    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    Normal = data_in[1].Normal;
    Color = data_in[1].Color;
    currentPosition = gl_in[1].gl_Position.xyz;
    reflectedVector = data_in[1].reflectedVector;
    fragPosLight = data_in[1].fragPosLight;
    texCoords = data_in[1].texCoords;
    m_TBN = mat3(1.0f);
    EmitVertex();

    gl_Position = data_in[2].projection * gl_in[2].gl_Position;
    Normal = data_in[2].Normal;
    Color = data_in[2].Color;
    currentPosition = gl_in[2].gl_Position.xyz;
    reflectedVector = data_in[2].reflectedVector;
    fragPosLight = data_in[2].fragPosLight;
    texCoords = data_in[2].texCoords;
    m_TBN = mat3(1.0f);
    EmitVertex();

    EndPrimitive();
}
                                
