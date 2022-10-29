#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 model;
uniform mat4 camera;

void main()
{
    vec3 currentPosition = vec3(model * translation * rotation * scale * vec4(aPos, 1.0));
    gl_Position = camera * vec4(currentPosition, 1.0);
}  

#shader fragment
#version 330 core

void main() {
    gl_FragDepth = gl_FragCoord.z;
}