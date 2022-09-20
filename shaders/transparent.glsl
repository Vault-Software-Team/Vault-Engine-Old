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

uniform mat4 camera;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 model;

void main() {
    currentPosition = vec3(translation * rotation * scale * vec4(position, 1.0));
    gl_Position = camera * vec4(currentPosition, 1.0);
    texCoords = g_texCoords;
    Color = color;
    Normal = aNormal;
}

#shader fragment
#version 330 core
in vec2 texCoords;
in vec3 Color;
in vec3 Normal;
in vec3 currentPosition;

uniform int isTex;
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

vec4 pointLight() {
    float ambient = 0.2;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - currentPosition);

    vec3 num1 = vec3(0.2, 0.2, 0.2);
    vec3 num2 = vec3(0.2, 0.2, 0.2);

    float diffuse = max(dot(normal, lightDir), 0.0);
    
    float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - currentPosition);
	vec3 reflectionDirection = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

    if(isTex == 1) {
        return (texture(diffuse0, texCoords) * lightColor * (diffuse + ambient)  + texture(specular0, texCoords).r * specular);
    } else {
        return (vec4(Color, 1) * lightColor * (diffuse + ambient) + texture(specular0, texCoords).r * specular);
    }
}

vec4 directionalLight() {
    float ambient = 0.2;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(vec3(1, 1, 0));

    vec3 num1 = vec3(0.2, 0.2, 0.2);
    vec3 num2 = vec3(0.2, 0.2, 0.2);

    float diffuse = max(dot(normal, lightDir), 0.0);
    
    float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - currentPosition);
	vec3 reflectionDirection = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

    if(isTex == 1) {
        return (texture(diffuse0, texCoords) * lightColor * (diffuse + ambient)  + texture(specular0, texCoords).r * specular);
    } else {
        return (vec4(Color, 1) * lightColor * (diffuse + ambient) + texture(specular0, texCoords).r * specular);
    }
}

vec4 spotLight() {
    float outerCone = 0.90;
    float innerCone = 0.95;

    float ambient = 0.2;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - currentPosition);

    vec3 num1 = vec3(0.2, 0.2, 0.2);
    vec3 num2 = vec3(0.2, 0.2, 0.2);

    float diffuse = max(dot(normal, lightDir), 0.0);
    
    float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - currentPosition);
	vec3 reflectionDirection = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

    float angle = dot(vec3(0, -1, 0), -lightDir);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0, 1);

    

    if(isTex == 1) {
        if(texture(diffuse0, texCoords).a < 0.1) 
            discard;
        return (texture(diffuse0, texCoords) * lightColor * (diffuse * inten + ambient)  + texture(specular0, texCoords).r * specular * inten) * lightColor;
    } else {
        return (vec4(Color, 1) * lightColor * (diffuse * inten + ambient) + texture(specular0, texCoords).r * specular * inten) * lightColor;
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
    gl_FragColor = directionalLight();
}

#shader geometry
#version 330 core

void main() {
    
}