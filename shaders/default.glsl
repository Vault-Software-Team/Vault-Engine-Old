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

//out vec2 texCoords;
//out vec3 Color;
//out vec3 Normal;
//out vec3 currentPosition;
//out vec3 reflectedVector;
//out vec4 fragPosLight;

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
// entity data that is unsigned int
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

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

vec4 pointLight(PointLight light) {
    float specular = 0;

    vec3 lightVec = light.lightPos - currentPosition;
    if(hasNormalMap == 1) {
        lightVec = (m_TBN * light.lightPos) - (m_TBN * currentPosition);
    } else {
        lightVec = light.lightPos - currentPosition;
    }

    vec3 normal;
    if(isTex == 1 && hasNormalMap == 1) {
        vec4 normalTex = texture(texture_normal0, texCoords);
        normal = normalize(normalTex.rgb * 2.0 - 1.0);
    } else {
        normal = normalize(Normal);
    }

    vec3 lightDir = normalize(lightVec);
    float diffuse = max(dot(normal, lightDir), 0.0);

    float dist = length(lightVec);
    float a = 1.00;
    float b = 0.04;
    float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);
    inten *= light.intensity;

    if(diffuse != 0.0f) {
        float specularLight = 0.5;
        vec3 viewDirection;
        if(hasNormalMap == 1) {
            viewDirection = normalize((m_TBN * cameraPosition) - (m_TBN * currentPosition));
        } else {
            viewDirection = normalize(cameraPosition - currentPosition);
        }

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
    vec3 lightDir = normalize(light.lightPos);
    vec3 viewDirection = normalize(cameraPosition - currentPosition);
    vec2 UVs = texCoords;

    if(hasNormalMap == 1) {
        viewDirection = normalize((m_TBN * cameraPosition) - (m_TBN * currentPosition));
    } else {
        viewDirection = normalize(cameraPosition - currentPosition);
    }

    if(hasHeightMap == 1 && hasNormalMap == 1) {
        float heightScale = 0.05;
        const float minLayers = 8;
        const float maxLayers = 64;
        float numLayers = mix(minLayers, maxLayers, abs(dot(vec3(0,0,1), viewDirection)));
        float layerDepth = 1 / numLayers;
        float currentLayerDepth = 0.0;

        vec2 S = viewDirection.xy / viewDirection.z * heightScale;
        vec2 deltaUVs = S / numLayers;

        float currentDepthMapValue = 1 - texture(texture_height0, UVs).r;

        while(currentLayerDepth < currentDepthMapValue) {
            UVs -= deltaUVs;
            currentDepthMapValue = 1 - texture(texture_height0, UVs).r;
            currentLayerDepth += layerDepth;
        }

        vec2 prevTexCoords = UVs + deltaUVs;
        float afterDepth = currentDepthMapValue - currentLayerDepth;
        float beforeDepth = 1 - texture(texture_height0, prevTexCoords).r - currentLayerDepth + layerDepth;
        float weight = afterDepth / (afterDepth - beforeDepth);
        UVs = prevTexCoords * weight + UVs * (1 - weight);
    }

    vec3 normal;
    if(isTex == 1 && hasNormalMap == 1) {
        vec4 normalTex = texture(texture_normal0, UVs);
        normal = normalize(normalTex.rgb * 2.0 - 1.0);
    } else {
        normal = normalize(Normal);
    }

    float diffuse = max(dot(normal, lightDir), 0.0);

    float specular = 0;
    float inten = 0.3;

    if(diffuse != 0.0f) {
        float specularLight = 0.5;
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

    if(isTex == 1) {
        return (mix(texture(texture_diffuse0, UVs), reflectedColor, metallic) * baseColor * vec4(light.color, 1) * (diffuse) + specularTexture * (((specular) * vec4(light.color, 1)) * light.intensity));
    } else {
        return (mix(baseColor, reflectedColor, metallic) * vec4(light.color, 1) * (diffuse) + vec4(1,1,1,1)  * (((specular) * vec4(light.color, 1)) * light.intensity));
    }
}

vec4 spotLight(SpotLight light) {
    float outerCone = 0.9;
    float innerCone = 0.95;

    float specular = 0;
    vec3 lightVec;
    if(hasNormalMap == 1) {
        lightVec = (m_TBN * light.lightPos) - (m_TBN * currentPosition);
    } else {
        lightVec = light.lightPos - currentPosition;
    }

    vec3 normal;
    if(isTex == 1 && hasNormalMap == 1) {
        vec4 normalTex = texture(texture_normal0, texCoords);
        normal = normalize(normalTex.rgb * 2.0 - 1.0);
    } else {
        normal = normalize(Normal);
    }

    vec3 lightDir = normalize(lightVec);
    float diffuse = max(dot(normal, lightDir), 0.0);

    if(diffuse != 0.0f) {
        float specularLight = 0.5;
        vec3 viewDirection;
        if(hasNormalMap == 1) {
            viewDirection = normalize((m_TBN * cameraPosition) - (m_TBN * currentPosition));
        } else {
            viewDirection = normalize(cameraPosition - currentPosition);
        }

        vec3 reflectDir = reflect(-lightDir, normal);

        vec3 halfwayVec = normalize(viewDirection + lightDir);

        float specAmount = pow(max(dot(normal, halfwayVec), 0.0), 16);
        specular = specAmount * specularLight;
    }
    float _smoothness = 1 - roughness;

    if(_smoothness == 0.0) {
        specular = 0.0;
    }

    float angle = dot(light.angle, -normalize(light.lightPos - currentPosition));
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0, 1.0);

    // add smoothness to the specular
    specular = specular * _smoothness;

    if(isTex == 1) {
        return (mix(texture(texture_diffuse0, texCoords), reflectedColor, metallic) * baseColor * (diffuse * inten) + specularTexture * ((specular * inten) * vec4(light.color, 1)))  * vec4(light.color, 1);
    } else {
        return (mix(baseColor, reflectedColor, metallic) * (diffuse * inten) * ((specular * inten) * vec4(light.color, 1))) * vec4(light.color, 1);
    }
}

vec4 light2d(Light2D light) {
    if(isTex == 1) {
        vec4 frag_color = texture(texture_diffuse0, texCoords) * baseColor;
        if(frag_color.a < 0.1)
        discard;

        float distance = distance(light.lightPos, currentPosition.xy);
        float diffuse = 0.0;

        if (distance <= light.range)
        diffuse =  1.0 - abs(distance / light.range);

        return vec4(min(frag_color.rgb * ((light.color * diffuse)), frag_color.rgb), 1.0);
    } else {
        vec4 frag_color = baseColor;
        if(frag_color.a < 0.1)
        discard;

        float distance = distance(light.lightPos, currentPosition.xy);
        float diffuse = 0.0;

        if (distance <= light.range)
        diffuse =  1.0 - abs(distance / light.range);

        return vec4(frag_color.rgb * ((light.color * diffuse)), frag_color.rgb);
    }
}

float near = 0.1;
float far = 100.0;

uniform uint u_EntityID;

// vec4 fog() {
//     float depth = logisticDepth(gl_FragCoord.z);
//     return (directionalLight() * (1 - depth) + vec4(depth * vec3(0.85, 0.85, 0.90), 1));
// }

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
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec2 deltaUV0 = data_in[1].texCoords - data_in[0].texCoords;
    vec2 deltaUV1 = data_in[2].texCoords - data_in[0].texCoords;

    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(data_in[0].model * vec4(cross(edge1, edge0), 0.0f)));

    mat3 TBN = mat3(T, B, N);
    TBN = transpose(TBN);

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