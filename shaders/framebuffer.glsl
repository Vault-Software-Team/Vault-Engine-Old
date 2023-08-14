#shader vertex
#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 g_texCoords;

out vec2 texCoords;

void main() {
    gl_Position = vec4(inPos.x, inPos.y, 0, 1);
    texCoords = g_texCoords;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gFragPosLight;

uniform float gamma;
uniform float exposure;

// post processing uniforms
uniform float chromaticAmount;
uniform float vignetteAmount;
uniform bool deferredShading;

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
uniform vec3 ambient_color;
#define MAX_LIGHTS 100
uniform PointLight pointLights[MAX_LIGHTS];
uniform Light2D light2ds[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];
uniform DirectionalLight dirLights[MAX_LIGHTS];
uniform vec3 cameraPosition;
uniform mat4 lightProjection;

const float shininess = 200.000;
const float roughness = 0;
vec3 normal = normalize(texture(gNormal, texCoords.st).rgb);
vec4 albedo = texture(screenTexture, texCoords.st);
float specularTexture = texture(screenTexture, texCoords.st).a;

uniform sampler2D shadow_map_buffer;
uniform samplerCube shadow_cubemap_buffer;

vec4 pointLight(PointLight light, vec3 currentPosition) {
    float specular = 0;
    // temp
    float constant = 1.0;
    // float linear = 0.09;
    // float quadratic = 0.032;
    float quadratic = 0.0003;
    float linear = 0.00002;
    vec2 UVs = texCoords;

    vec3 lightVec = (light.lightPos - currentPosition);
    vec3 viewDirection = normalize(cameraPosition - currentPosition);

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
    // if() {
        float farPlane = 100;
        vec3 fragToLight = currentPosition - light.lightPos;
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
                        shadow += 1;
                    }
                }
            } 
        }
        shadow /= pow((sampleRadius *2 + 1), 3);
    // }

    // if(isTex == 1) {
        return (albedo * ((diffuse  * (1.0f - shadow) * inten + (vec4(ambient_color, 1) * ambient))) + specularTexture * ((specular * inten) * (1.0f - shadow)))  * vec4(light.color, 1);
    // } else {
        // return vec4(shadow, shadow, shadow, 1);
    // }
}

vec4 directionalLight(DirectionalLight light, vec3 currentPosition) {
    vec3 lightDir = normalize(light.lightPos);
    vec3 viewDirection = normalize(cameraPosition - currentPosition);
    vec2 UVs = texCoords.st;

    float diffuse = max(dot(normal, lightDir), 0.0);

    float specular = 0;
    float inten = 0.3;

    vec4 fragPosLight = lightProjection * vec4(currentPosition, 1); 

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
    return (albedo * vec4(light.color, 1) * (diffuse * (1.0f - shadow) + (vec4(ambient_color, 1) * ambient)) + specularTexture * (((specular  * (1.0f - shadow)) * vec4(light.color, 1)) * light.intensity));
    // if(isTex == 1) {
    // } else {
        // return (mix(baseColor, reflectedColor, metallic) * vec4(light.color, 1) * (diffuse) + vec4(1,1,1,1)  * (((specular) * vec4(light.color, 1)) * light.intensity));
        // return (albedo * vec4(light.color, 1) * (diffuse * (1.0f - shadow) + (vec4(ambient_color, 1) * ambient)) + vec4(1,1,1,1)  * (((specular  * (1.0f - shadow)) * vec4(light.color, 1)) * light.intensity));
    // }
}

vec4 chromaticAberration(float amount)
{
    float r = texture(screenTexture, texCoords - vec2(amount, 0)).r;
    float g = texture(screenTexture, texCoords).g;
    float b = texture(screenTexture, texCoords + vec2(amount, 0)).b;

    return vec4(r, g, b, texture(screenTexture, texCoords).a);
}

vec4 vignette(float amount)
{
    vec2 tex = texCoords - vec2(0.5, 0.5);
    float dist = length(tex);
    float vignette = smoothstep(0.8, 0.0, dist * (amount + 0.5));
    return vec4(vignette, vignette, vignette, 1.0);
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 filmGrain(float amount) {
    float grain = (rand(texCoords) - 0.5) * amount;

    return vec4(grain, grain, grain, 1.0);
}

vec4 radialBlur(vec4 tex, vec2 cursorPos, float blurWidth) {
    vec4 color = tex;
	vec2 res;
	res = texCoords;
	vec2 pp = cursorPos;
	vec2 center = pp;
	float blurStart = 1.0;

    const int nsamples = 10;

	vec2 uv = texCoords.xy;

	uv -= center;
	float precompute = blurWidth * (1.0 / float(nsamples - 1));

	for(int i = 0; i < nsamples; i++)
	{
		float scale = blurStart + (float(i)* precompute);
		float r = texture2D(screenTexture, (uv * scale + center) - vec2(chromaticAmount, 0.0)).rgb.x;
		float g = texture2D(screenTexture, (uv * scale + center)).rgb.y;
		float b = texture2D(screenTexture, (uv * scale + center) + vec2(chromaticAmount, 0.0)).rgb.z;
		float a = texture2D(screenTexture, (uv * scale + center)).a;

		color += vec4(r, g, b, a);
	}

	color /= float(nsamples);
    return color;
}

vec4 bloom()
{
    // vec3 color = radialBlur(chromaticAberration(chromaticAmount), vec2(0.5, 0.5), 0.05).rgb;
    vec3 color = chromaticAberration(chromaticAmount).rgb;
    vec3 bloomColor = texture(bloomTexture, texCoords).rgb;
    return vec4(color + bloomColor, 1.0) * vignette(vignetteAmount);
}

void main() {
    vec2 resolution = vec2(1920, 1080);
    vec2 srcTexel = 1.0 / resolution;
    float x = srcTexel.x;
    float y = srcTexel.y;

    vec4 fragment = texture(screenTexture, texCoords.st);
    vec4 color = bloom();

    vec3 currentPosition = texture(gPosition, texCoords.st).rgb;
    if(deferredShading) {
        vec4 result = vec4(0);
        for(int i = 0; i < MAX_LIGHTS; i++) {
            if(dirLights[i].intensity == 1) {
                result += directionalLight(dirLights[i], currentPosition);
            }

            if(pointLights[i].intensity > 0) {
                result += pointLight(pointLights[i], currentPosition);
            }
        }
        result.a = 1;
        color = result;
        if(color.r == 0 && color.g == 0 && color.b == 0) color = texture(gNormal, texCoords.st);
    }

    vec3 toneMapped = vec3(1.0) - exp(-color.rgb * exposure);

    vec3 res = vec3(0.0);
    float Z = 0.0;

    FragColor.rgb = pow(toneMapped, vec3(1.0/2.2));
    BloomColor = vec4(0);
}
