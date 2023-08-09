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
layout(location = 8) in float transform_index;

uniform mat4 camera;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform vec3 cameraPosition;
uniform vec2 texUvOffset;
uniform mat4 cam_view;
uniform bool isBatch;
#define MAX_TRANSFORMS 32
uniform mat4 transforms[MAX_TRANSFORMS];

uniform float time;

// Shadow Mapping
uniform mat4 lightProjection;

// out DATA {
//     vec2 texCoords;
//     vec3 Color;
//     vec3 Normal;
//     vec3 currentPosition;
//     vec3 reflectedVector;
//     vec4 fragPosLight;
//     mat4 projection;
//     mat4 model;
//     vec3 T;
//     vec3 B;
//     vec3 N;
// } data_out;
out vec2 texCoords;
out vec3 Color;
out vec3 Normal;
out vec3 currentPosition;
out vec3 reflectedVector;
out vec4 fragPosLight;
out mat4 projection;
out mat3 m_TBN;
out float visibility;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

const float density = 0.007;
const float gradient = 1.5;

void main() {
    vec4 totalPosition = vec4(0);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE; i++)
    {
        if(boneIds[i] == -1) 
            continue;
            
        if(boneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(position, 1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(position, 1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
    }
    if(totalPosition.xyz == vec3(0,0,0)) {
        totalPosition = vec4(position, 1.0f);
    }
    // for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    // {
    //     if(boneIds[i] == -1) continue;

    //     if(boneIds[i] >= MAX_BONES) 
    //     {
    //         totalPosition = vec4(position, 1.0f);
    //         break;
    //     }

    //     vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(position, 1.0f);
    //     totalPosition += localPosition * weights[i];
    //     vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
    // }
    // totalPosition = vec4(position, 1.0f);

    vec4 worldPosition = vec4(0);
    currentPosition = vec3(0);
    if(isBatch) {
        mat4 m_trans = transforms[int(transform_index)];
        worldPosition = m_trans * translation * rotation * scale * totalPosition;
        currentPosition = vec3(m_trans * translation * rotation * scale * totalPosition);
    } else {
        worldPosition = model * translation * rotation * scale * totalPosition;
        currentPosition = vec3(model * translation * rotation * scale * totalPosition);
    }
    projection = camera;
    
    gl_Position = camera * vec4(currentPosition, 1.0);


    // vec2 finalCoords = g_texCoords;

    // if(finalCoords.x > 0) {
    //     finalCoords.x = finalCoords.x + texUvOffset.x;
    // }

    // if(finalCoords.y > 0) {
    //     finalCoords.y = finalCoords.y + texUvOffset.y;
    // }Normal = mat3(transpose(inverse(model))) * aNormal;

    texCoords = g_texCoords;
    if(texUvOffset.x > 0) {
        texCoords.x *= texUvOffset.x;
    } 
    if(texUvOffset.y > 0) {
        texCoords.y *= texUvOffset.y;
    }

    Color = color;
    // make normal apply rotation
    // Normal = mat3(transpose(inverse(model))) * aNormal;
    Normal = mat3(transpose(inverse(model))) * aNormal;


    vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);
    reflectedVector = reflect(viewVector, normalize(Normal));

    // calculate tangent/bitangent of the current vertex
    mat4 fullM = model * translation * rotation * scale;
    vec3 T = normalize(vec3(fullM * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(fullM * vec4(aNormal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    // vec3 B =  normalize(vec3(fullM * vec4(bitangent, 0.0)));
    vec3 B = cross(N, T);

    m_TBN = mat3(T, B, N);

    fragPosLight = lightProjection * vec4(currentPosition, 1.0);

    // Fog
    vec4 positionRelativeToCam = cam_view * worldPosition;
    float distance = length(positionRelativeToCam.xyz);
    visibility = exp(-pow((distance * density), gradient));
    visibility = clamp(visibility, 0.0, 1.0);
}

#shader fragment
#version 330 core


layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;
// entity data that is unsigned int
layout(location = 2) out uint EntityID;

// in vec2 texCoords;
// in vec3 Color;
// in vec3 Normal;
// in vec3 currentPosition;
// in vec3 reflectedVector;
// in vec4 fragPosLight;
in mat3 m_TBN;

in vec2 texCoords;
in vec3 Color;
in vec3 Normal;
in vec3 currentPosition;
in vec3 reflectedVector;
in mat4 projection;
in vec4 fragPosLight;
in float visibility;
// out mat4 model;

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
uniform float shininess;

//texture setters
uniform int hasNormalMap;
uniform int hasHeightMap;

//global textures
uniform sampler2D shadowMap;
uniform sampler2D shadow_map_buffer;
uniform samplerCube shadow_cubemap_buffer;
uniform bool shadow_cubemap_set;
uniform float farPlane;
uniform vec3 pointLightPos;

vec4 reflectedColor = texture(cubeMap, reflectedVector);
float specularTexture = texture(texture_specular0, texCoords).r;

vec4 pointLight(PointLight light) {
    float specular = 0;
    // temp
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    vec2 UVs = texCoords;

    vec3 lightVec = (light.lightPos - currentPosition);
    vec3 viewDirection = normalize(cameraPosition - currentPosition);
    // float height_scale = 0.05;
    // const float minLayers = 8.0;
    // const float maxLayers = 64.0 * 2;
    // float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0,0,1), viewDirection)));
    // float layerDepth = 1.0 / numLayers;
    // float currentLayerDepth = 0.0;

    // vec2 S = viewDirection.xy / viewDirection.z * height_scale;
    // vec2 deltaUVs = S / numLayers;

    // vec2 UVs = texCoords;
    // float currentDepthMapValue = 1.0 - texture(texture_emission0, UVs).r;

    // while(currentLayerDepth < currentDepthMapValue) {
    //     UVs -= deltaUVs;
    //     currentDepthMapValue = 1.0 - texture(texture_emission0, UVs).r;
    //     currentLayerDepth += layerDepth;
    // }

    // vec2 prevTexCoords = UVs + deltaUVs;
    // float afterDepth = currentDepthMapValue - currentLayerDepth;
    // float beforeDepth = 1.0 - texture(texture_emission0, texCoords).r - currentLayerDepth + layerDepth;
    // float weight = afterDepth / (afterDepth - beforeDepth);
    // UVs = prevTexCoords * weight + UVs * (1.0 - weight);

    // if(UVs.x > 1.0 || UVs.y > 1.0 || UVs.x < 0.0 || UVs.y < 0.0)
    //     discard;

    vec3 normal;
    if(isTex == 1 && hasNormalMap == 1) {
        vec4 normalTex = texture(texture_normal0, UVs);
        normal = normalTex.rgb * 2.0 - 1.0;
        normal = normalize(m_TBN * normal);
    } else {
        normal = normalize(Normal);
    }

    vec3 lightDir = normalize(lightVec);
    float diffuse = max(dot(normal, lightDir), 0.0);

    float dist = length(lightVec);
    // float a = 1.00;
    // float b = 0.04;
    float inten = light.intensity / (constant + linear * dist + quadratic * (dist * dist));
    // float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);
    // inten *= light.intensity;

    if(diffuse != 0.0f) {
        float specularLight = 0.5;
        vec3 viewDirection = normalize(cameraPosition - currentPosition);

        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayVec = normalize(viewDirection + lightDir);

        float specAmount = pow(max(dot(normal, halfwayVec), 0.0), shininess);
        specular = specAmount * specularLight;
    }

    float _smoothness = 1 - roughness;

    if(_smoothness == 0.0) {
        specular = 0.0;
    }

    // add smoothness to the specular
    specular = specular * _smoothness;

    float shadow = 0.0f;
    if(shadow_cubemap_set) {
        vec3 fragToLight = currentPosition - pointLightPos;
        float currentDepth = length(fragToLight);
        float bias = max(0.5f * (1.0f - dot(normal, lightDir)), 0.0005f);

        int sampleRadius = 2;
        float pixelSize = 1.0f / 1024.0f;

        for(int z = -sampleRadius; z < sampleRadius; z++) {
        for(int y = -sampleRadius; y < sampleRadius; y++) {
                for(int x = -sampleRadius; x < sampleRadius; x++) {
                    float closestDepth = texture(shadow_cubemap_buffer, fragToLight + vec3(x,y,z)  * pixelSize).r;
                    closestDepth *= farPlane;
                    if(currentDepth > closestDepth + bias) {
                        shadow += 1.0f;
                    }
                }
            } 
        }
        shadow /= pow((sampleRadius *2 + 1), 3);
    }

    if(isTex == 1) {
        return (mix(texture(texture_diffuse0, UVs), reflectedColor, metallic) * baseColor * ((diffuse * inten + ambient)  * (1.0f - shadow)) + specularTexture * ((specular * (1.0f - shadow) * inten) ))  * vec4(light.color, 1);
    } else {
        // return vec4(shadow, shadow, shadow, 1);
        return (mix(baseColor, reflectedColor, metallic) * (diffuse * (1.0f - shadow) * inten + ambient) * ((specular * (1.0f - shadow) * inten + ambient))) * vec4(light.color, 1);
    }
}

vec4 directionalLight(DirectionalLight light) {
    vec3 lightDir = normalize(light.lightPos);
    vec3 viewDirection = normalize(cameraPosition - currentPosition);
    vec2 UVs = texCoords;

    vec3 normal;
    if(isTex == 1 && hasNormalMap == 1) {
        normal = texture(texture_normal0, texCoords).rgb;
        normal = normal * 2.0 - 1.0;
        normal = normalize(m_TBN * normal);   
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

        float specAmount = pow(max(dot(normal, halfwayVec), 0.0), shininess);
        specular = specAmount * specularLight;
    }
    float _smoothness = 1 - roughness;
    if(_smoothness == 0) {
        specular = 0;
    }
    specular = specular * _smoothness;

    float shadow = 0.0f;
    vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
    if(lightCoords.z <= 1.0f) {
        lightCoords = (lightCoords + 1.0f) / 2.0f;

        float closestDepth = texture(shadow_map_buffer, lightCoords.xy).r;
        float currentDepth = lightCoords.z;
        float bias = max(0.025f * (1.0f - dot(normal, lightDir)), 0.0005f);

        int sampleRadius = 2;
        vec2 pixelSize  = 1.0 / textureSize(shadow_map_buffer, 0);
        for(int y = -sampleRadius; y <= sampleRadius; y++) {
            for(int x = -sampleRadius; x <= sampleRadius; x++) {
                float closestDepth = texture(shadow_map_buffer, lightCoords.xy + vec2(x,y) * pixelSize).r;
                if(currentDepth > closestDepth + bias)
                    shadow += 1.0f;
            }
        }

        shadow /= pow((sampleRadius * 2 + 1), 2);
    }

    if(isTex == 1) {
        return (mix(texture(texture_diffuse0, UVs), reflectedColor, metallic) * vec4(light.color, 1) * (diffuse * (1.0f - shadow) + ambient) + specularTexture * (((specular  * (1.0f - shadow)) * vec4(light.color, 1)) * light.intensity)) + texture(texture_emission0, UVs).r;
    } else {
        // return (mix(baseColor, reflectedColor, metallic) * vec4(light.color, 1) * (diffuse) + vec4(1,1,1,1)  * (((specular) * vec4(light.color, 1)) * light.intensity));
        return (mix(baseColor, reflectedColor, metallic) * vec4(light.color, 1) * (diffuse * (1.0f - shadow) + ambient) + vec4(1,1,1,1)  * (((specular  * (1.0f - shadow)) * vec4(light.color, 1)) * light.intensity));
    }
}

vec4 spotLight(SpotLight light) {
    float outerCone = 0.9;
    float innerCone = 0.95;

    float specular = 0;
    vec3 lightVec = light.lightPos - currentPosition;;

    vec3 normal;
    if(isTex == 1 && hasNormalMap == 1) {
        vec4 normalTex = texture(texture_normal0, texCoords);
        normal = normalTex.rgb * 2.0 - 1.0;
        normal = normalize(m_TBN * normal);
    } else {
        normal = normalize(Normal);
    }

    vec3 lightDir = normalize(lightVec);
    float diffuse = max(dot(normal, lightDir), 0.0);

    if(diffuse != 0.0f) {
        float specularLight = 0.5;
        vec3 viewDirection = normalize(cameraPosition - currentPosition);

        vec3 reflectDir = reflect(-lightDir, normal);

        vec3 halfwayVec = normalize(viewDirection + lightDir);

        float specAmount = pow(max(dot(normal, halfwayVec), 0.0), shininess);
        specular = specAmount * specularLight;
    }
    float _smoothness = 1 - roughness;

    if(_smoothness == 0.0) {
        specular = 0.0;
    }

    float angle = dot(light.angle, -normalize(light.lightPos - currentPosition));
    // make angles y axis the same when lightPos changes
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0, 1.0);
    // make intensity smaller the further away the light is
    float dist = length(lightVec);
    float a = 1.00;
    float b = 0.04;
    inten = 1;

    // add smoothness to the specular
    specular = specular * _smoothness;

    float theta = dot(lightDir, normalize(-vec3(light.angle)));
    float cutOff = cos(radians(12.5));
    float outerCutOff = cos(radians(17.5));
    float epsilon = cutOff - outerCutOff;
    inten = clamp((theta - cutOff) / epsilon, 0.0, 1.0);

    if(theta < cutOff) {
        inten = 0;
    }

    float shadow = 0.0f;
    vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
    if(lightCoords.z <= 1.0f) {
        lightCoords = (lightCoords + 1.0f) / 2.0f;

        float closestDepth = texture(shadow_map_buffer, lightCoords.xy).r;
        float currentDepth = lightCoords.z;
        float bias = max(0.00025f * (1.0f - dot(normal, lightDir)), 0.000005f);

        int sampleRadius = 2;
        vec2 pixelSize  = 1.0 / textureSize(shadow_map_buffer, 0);
        for(int y = -sampleRadius; y <= sampleRadius; y++) {
            for(int x = -sampleRadius; x <= sampleRadius; x++) {
                float closestDepth = texture(shadow_map_buffer, lightCoords.xy + vec2(x,y) * pixelSize).r;
                if(currentDepth > closestDepth + bias)
                    shadow += 1.0f;
            }
        }

        shadow /= pow((sampleRadius * 2 + 1), 2);
    }

    if(isTex == 1) {
        return (mix(texture(texture_diffuse0, texCoords), reflectedColor, metallic) * baseColor * (diffuse * (1.0f - shadow) * inten + ambient) + specularTexture * ((specular * (1.0f - shadow) * inten) * vec4(light.color, 1)))  * vec4(light.color, 1);
    } else {
        return (mix(baseColor, reflectedColor, metallic) * (diffuse * (1.0f - shadow) * inten) * ((specular * (1.0f - shadow) * inten  + ambient) * vec4(light.color, 1))) * vec4(light.color, 1);
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

        return vec4(min(frag_color.rgb * ((light.color * diffuse)), frag_color.rgb), texture(texture_diffuse0, texCoords).a * baseColor.a);
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

// health bar thing

float near = 0.1;
float far = 100.0;

uniform uint u_EntityID;

// Post Processing uniforms
uniform bool dynamic_bloom;
uniform float bloom_threshold;

void main() {
    if(specularTexture == 0) {
        specularTexture = texture(texture_specular0, texCoords).r;
    }

    vec4 result = vec4(0);
    vec4 noAmbient = vec4(0);

    vec4 tex = texture(texture_diffuse0, texCoords);
    float alpha = tex.a;
    // if(isTex == 1) {
    //     noAmbient = mix(tex, reflectedColor, metallic) * baseColor;
    //     result = noAmbient * ambient;
    //     result.a = tex.a;
    //     result.a *= baseColor.a;
    //     alpha = result.a;
    // } else {
    //     noAmbient = mix(baseColor, reflectedColor, metallic);
    //     result = noAmbient * ambient;
    //     result.a = baseColor.a;
    //     alpha = result.a;
    // }

    for(int i = 0; i < MAX_LIGHTS; i++) {
        if(pointLights[i].intensity > 0) {
            vec4 light = pointLight(pointLights[i]);
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
    
    if(result.r == 0 && result.g == 0 && result.b == 0) {
        if(isTex == 1) {
            noAmbient = mix(tex, reflectedColor, metallic) * baseColor;
            result = noAmbient * ambient;
            result.a = tex.a;
            result.a *= baseColor.a;
            alpha = result.a;
        } else {
            noAmbient = mix(baseColor, reflectedColor, metallic);
            result = noAmbient * ambient;
            result.a = baseColor.a;
            alpha = result.a;
        }
    }
    
    result.a = alpha;
    FragColor = result;
    if(result.a < 0.1) {
        discard;
    }

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > bloom_threshold && dynamic_bloom) {
        BloomColor = FragColor;
    } else if((u_BloomColor.r > 0.5 || u_BloomColor.g > 0.5 || u_BloomColor.b > 0.5)
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
    FragColor.a = alpha;
    BloomColor.a = alpha;

    EntityID = u_EntityID;
}

// #shader geometry
// #version 330 core
// layout(triangles) in;
// layout(triangle_strip, max_vertices = 3) out;

// out vec2 texCoords;
// out vec3 Color;
// out vec3 Normal;
// out vec3 currentPosition;
// out vec3 reflectedVector;
// out vec4 fragPosLight;
// out mat3 m_TBN;

// in DATA {
//     vec2 texCoords;
//     vec3 Color;
//     vec3 Normal;
//     vec3 currentPosition;
//     vec3 reflectedVector;
//     vec4 fragPosLight;
//     mat4 projection;
//     mat4 model;
//     vec3 T;
//     vec3 B;
//     vec3 N;
// } data_in[];

// void main() {
//     vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
//     vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
//     vec2 deltaUV0 = data_in[1].texCoords - data_in[0].texCoords;
//     vec2 deltaUV1 = data_in[2].texCoords - data_in[0].texCoords;

//     float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

//     vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
//     vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

//     vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0f)));
//     vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0f)));
//     vec3 N = normalize(vec3(data_in[0].model * vec4(cross(edge1, edge0), 0.0f)));

//     mat3 TBN = mat3(T, B, N);
//     TBN = transpose(TBN);

//     gl_Position = data_in[0].projection * gl_in[0].gl_Position;
//     Normal = data_in[0].Normal;
//     Color = data_in[0].Color;
//     currentPosition = gl_in[0].gl_Position.xyz;
//     reflectedVector = data_in[0].reflectedVector;
//     fragPosLight = data_in[0].fragPosLight;
//     texCoords = data_in[0].texCoords;
//     m_TBN = TBN;
//     EmitVertex();

//     gl_Position = data_in[1].projection * gl_in[1].gl_Position;
//     Normal = data_in[1].Normal;
//     Color = data_in[1].Color;
//     currentPosition = gl_in[1].gl_Position.xyz;
//     reflectedVector = data_in[1].reflectedVector;
//     fragPosLight = data_in[1].fragPosLight;
//     texCoords = data_in[1].texCoords;
//     m_TBN = TBN;
//     EmitVertex();

//     gl_Position = data_in[2].projection * gl_in[2].gl_Position;
//     Normal = data_in[2].Normal;
//     Color = data_in[2].Color;
//     currentPosition = gl_in[2].gl_Position.xyz;
//     reflectedVector = data_in[2].reflectedVector;
//     fragPosLight = data_in[2].fragPosLight;
//     texCoords = data_in[2].texCoords;
//     m_TBN = TBN;
//     EmitVertex();

//     EndPrimitive();
// }