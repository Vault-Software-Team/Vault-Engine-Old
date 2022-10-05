#shader vertex
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

uniform mat4 camera;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform float outlining;
uniform mat4 model;

void main() {
    vec3 currentPosition = vec3(model * translation * rotation * scale * vec4(position + normal * outlining, 1));
    gl_Position = camera * vec4(currentPosition, 1);
}

#shader fragment
#version 330 core

void main() {
    gl_FragColor = vec4(1.0, 0.5, 0, 1.0);
}