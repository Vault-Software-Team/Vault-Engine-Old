#shader vertex
#version 300 es

layout(location = 0) in vec3 aPosition;

uniform mat4 camera;
uniform mat4 model;

void main() {
    gl_Position = camera * model * vec4(aPosition, 1.0);
}

#shader fragment
#version 300 es
precision mediump float;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}