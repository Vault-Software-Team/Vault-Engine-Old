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

void main() {
    vec4 worldPosition = model * translation * rotation * scale * vec4(position, 1.0);
    currentPosition = vec3(model * translation * rotation * scale * vec4(position, 1.0));
    gl_Position = camera * vec4(currentPosition, 1.0);
    texCoords = g_texCoords;
    Color = color;
    Normal = aNormal;

    fragPosLight = lightProjection * vec4(currentPosition, 1.0);

    vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);
    reflectedVector = reflect(viewVector, Normal);
}

#shader fragment
#version 330 core

in vec2 texCoords;
in vec3 Color;
in vec3 Normal;
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

//Material properties
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_normal0;

uniform vec4 baseColor;
uniform float metallic;
uniform float smoothness;

//pbr requirements
uniform vec3 cameraPosition;
in vec3 currentPosition;
vec3 fragmentPosition = gl_FragCoord.xyz;
uniform vec3 emissiveColor;
uniform float roughness;
uniform vec3 baseReflectance;

vec3 N = normalize(Normal);
vec3 V = normalize(cameraPosition - currentPosition);

//dir light
vec3 L = normalize(vec3(2, 0, 0) - currentPosition);
vec3 H = normalize(V + L);
vec3 F0 = vec3(0.04);

//PI
const float PI = 3.14159265359;

float D(float alpha, vec3 N, vec3 H) {
    float numerator = pow(alpha, 2);

    float NdotH = max(dot(N, H), 0.0);
    float denominator = PI * pow(NdotH, 2) * (pow(alpha, 2) - 1.0) + 1.0;
    denominator = max(denominator, 0.0001);

    return numerator / denominator;
}

float G1(float alpha, vec3 N, vec3 X) {
    float numerator = max(dot(N, X), 0.0);

    float k = alpha / 2.0;
    float denominator = max(dot(N, X), 0.0001) * (1.0 - k) + k;
    denominator = max(denominator, 0.0001);

    return numerator / denominator;
}

float G(float alpha, vec3 N, vec3 V, vec3 L) {
    return G1(alpha, N, V) * G1(alpha, N, L);
}

vec3 F(vec3 F0, vec3 V, vec3 H) {
    return F0 + (vec3(1.0) - F0) * pow(1.0 - max(dot(V, H), 0.0), 5.0); 
}

vec3 PBR() {
    // alpha
    float alpha = roughness * roughness;
    vec3 Ks = F(F0, V, H);
    vec3 Kd = vec3(1.0) - Ks;

    vec3 lambert = baseColor.rgb / PI;

    vec3 cookTorrenceNumerator = D(alpha, N, H) * G(alpha, N, V, L) * F(F0, V, H);
    float cookTorrenceDenominator = 4.0 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0);
    cookTorrenceDenominator = max(cookTorrenceDenominator, 0.0001);

    vec3 cookTorrence = cookTorrenceNumerator / cookTorrenceDenominator;

    vec3 BRDF = Kd * lambert + cookTorrence;
    vec3 outgoingLight = emissiveColor + BRDF * vec3(1.0, 1.0, 1.0) * max(dot(L, N), 0.0);

    return outgoingLight;
}

//texture setters
uniform int hasNormalMap;

vec4 pointLight(PointLight light) {
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.lightPos - currentPosition);
    float diffuse = max(dot(normal, lightDir), 0.0);

    float specular = 0;

    vec3 lightVec = light.lightPos - currentPosition;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
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
    float _smoothness = smoothness;

	if(smoothness == 0.0) {
        specular = 0.0;
    }

    // add smoothness to the specular
    specular = specular * _smoothness;

    if(isTex == 1) {
        return (texture(texture_diffuse0, texCoords) * baseColor * vec4(light.color, 1) * (diffuse) + texture(texture_specular0, texCoords).r * (specular * vec4(light.color, 1)));
    } else {
        return (baseColor * vec4(light.color, 1) * (diffuse) * (specular * vec4(light.color, 1)));
    }
}

vec4 directionalLight() {
    // float ambient = 0.4;

    vec3 normal;
    if(isTex == 1 && hasNormalMap == 1) {
        normal = normalize(Normal);
        // normal = normalize(texture(texture_normal0, texCoords).rgb * 2.0 - 1.0);
    } else {
        normal = normalize(Normal);
    }
    vec3 lightDir = normalize(vec3(0.5f, 0.5f, 0.5f));
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

    float shadow = 0.0f;
    vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		float bias = max(0.025f * (1.0f - dot(normal, lightDir)), 0.0005f);

		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);

	}

    if(smoothness == 0.0 || smoothness == 0) {
        specular = 0;
    }

    specular = specular * smoothness;

	if(isTex == 1) {
        return (texture(texture_diffuse0, texCoords) * baseColor * vec4(1, 1, 1, 1) * (diffuse ) + texture(texture_specular0, texCoords).r * specular);
    } else {
        return (baseColor * vec4(1, 1, 1, 1) * (diffuse) + vec4(1,1,1,1) * specular);
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
        return (texture(texture_diffuse0, texCoords)  * vec4(light.color, 1) * baseColor * (diffuse * inten)  + texture(texture_specular0, texCoords).r * specular * inten);
    } else {
        return (baseColor  * vec4(light.color, 1) * (diffuse * inten) + texture(texture_specular0, texCoords).r * specular * inten);
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
    // vec4 result = vec4(0);

    // if(isTex == 1) {
    //     result = texture(texture_diffuse0, texCoords) * ambient;
    // } else {
    //     result = baseColor * ambient;
    // }

    // // discard if the alpha is 0
    // if(result.a < 0.1) {
    //     discard;
    // }

    // result += directionalLight();

    // for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
    //     if(pointLights[i].intensity > 0) {
    //         result += pointLight(pointLights[i]);
    //     }
    // }

    // for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
    //     if(spotLights[i].intensity > 0) {
    //         result += spotLight(spotLights[i]);
    //     }
    // }

    // vec4 reflectedColor = texture(cubeMap, reflectedVector);
    
    // gl_FragColor = mix(result, reflectedColor, metallic);
    gl_FragColor = vec4(PBR().rgb, 1);
}