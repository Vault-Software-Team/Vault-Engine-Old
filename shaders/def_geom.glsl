#shader vertex
#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTex;


out DATA
{
    vec3 Normal;
	vec3 color;
	vec2 texCoord;
    mat4 projection;
} data_out;



// Imports the camera matrix
uniform mat4 camMatrix;
// Imports the transformation matrices
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;


void main()
{
	gl_Position = translation * rotation * scale * vec4(aPos, 1.0f);
	data_out.Normal = aNormal;
	data_out.color = aColor;
	data_out.texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
	data_out.projection = camMatrix;
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

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 Normal;
out vec3 color;
out vec2 texCoord;

in DATA
{
    vec3 Normal;
	vec3 color;
	vec2 texCoord;
    mat4 projection;
} data_in[];


// Default main function
void main()
{
    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
    Normal = data_in[0].Normal;
    color = data_in[0].color;
    texCoord = data_in[0].texCoord;
    EmitVertex();

    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    Normal = data_in[1].Normal;
    color = data_in[1].color;
    texCoord = data_in[1].texCoord;
    EmitVertex();

    gl_Position = data_in[2].projection * gl_in[2].gl_Position;
    Normal = data_in[2].Normal;
    color = data_in[2].color;
    texCoord = data_in[2].texCoord;
    EmitVertex();

    EndPrimitive();
}