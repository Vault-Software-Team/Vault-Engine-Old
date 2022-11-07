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

void main() {
    vec4 worldPosition = model * translation * rotation * scale * vec4(position, 1);
    data_out.currentPosition = vec3(model * translation * rotation * scale * vec4(position, 1));
    data_out.projection = camera;
    gl_Position = vec4(data_out.currentPosition, 1.0);
    data_out.model = model * translation * rotation * scale;

    data_out.texCoords = g_texCoords;
    data_out.Color = color;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;
// entity data that is unsigned int
layout(location = 2) out uint EntityID;

in vec2 texCoords;
in vec3 Color;
in vec3 Normal;
in vec3 currentPosition;
in vec3 reflectedVector;
in vec4 fragPosLight;
in mat3 m_TBN;

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

void main() {
    vec4 normalTex = texture(texture_normal0, texCoords);

    float shadow = ShadowCalculation(fragPosLight);
    if(specularTexture == 0) {
        specularTexture = texture(texture_diffuse0, texCoords).r;
    }

    vec4 result = vec4(0);
    vec4 noAmbient = vec4(0);

    if(isTex == 1) {
        vec4 tex = texture(texture_diffuse0, texCoords);
        noAmbient = mix(tex, reflectedColor, metallic) * baseColor;
        result = noAmbient * ambient;
        result.a = tex.a;
    } else {
        noAmbient = mix(baseColor, reflectedColor, metallic);
        result = noAmbient * ambient;
        result.a = baseColor.a;
    }

    if(result.a < 0.1) {
        discard;
    }

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

    FragColor = result;

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > bloomThreshold && globalBloom == 0) {
        // BloomColor = FragColor;
    }

    if((u_BloomColor.r > 0.5 || u_BloomColor.g > 0.5 || u_BloomColor.b > 0.5)
    && (u_BloomColor.r < 0.7 || u_BloomColor.g < 0.7 || u_BloomColor.b < 0.7)) {
        FragColor = result * (u_BloomColor.r * 20);
        BloomColor = vec4(u_BloomColor * 2, 1);
    } else if(u_BloomColor.r > 0.7 || u_BloomColor.g > 0.7 || u_BloomColor.b > 0.7) {
        FragColor = result * (u_BloomColor.r * 30);
        BloomColor = vec4(u_BloomColor * 3, 1);
    } else {
        BloomColor = vec4(u_BloomColor, 1);
    }

    vec4 emission = texture(texture_emission0, texCoords);

    if(emission.r > 0 || emission.g > 0 || emission.b > 0) {
        FragColor += emission;
        if(u_BloomColor.r > 0 || u_BloomColor.g > 0 || u_BloomColor.b > 0) {
            BloomColor = emission * vec4(u_BloomColor, 1);
        } else {
            BloomColor = emission;
        }
    }

    EntityID = u_EntityID;
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
    mat3 TBN;

    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
    Normal = data_in[0].Normal;
    Color = data_in[0].Color;
    currentPosition = gl_in[0].gl_Position.xyz;
    reflectedVector = data_in[0].reflectedVector;
    fragPosLight = data_in[0].fragPosLight;
    texCoords = data_in[0].texCoords;
    m_TBN = TBN;
    EmitVertex();

    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    Normal = data_in[1].Normal;
    Color = data_in[1].Color;
    currentPosition = gl_in[1].gl_Position.xyz;
    reflectedVector = data_in[1].reflectedVector;
    fragPosLight = data_in[1].fragPosLight;
    texCoords = data_in[1].texCoords;
    m_TBN = TBN;
    EmitVertex();

    gl_Position = data_in[2].projection * gl_in[2].gl_Position;
    Normal = data_in[2].Normal;
    Color = data_in[2].Color;
    currentPosition = gl_in[2].gl_Position.xyz;
    reflectedVector = data_in[2].reflectedVector;
    fragPosLight = data_in[2].fragPosLight;
    texCoords = data_in[2].texCoords;
    m_TBN = TBN;
    EmitVertex();

    EndPrimitive();
}