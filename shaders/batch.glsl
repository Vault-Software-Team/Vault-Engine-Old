#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec4 gColor;

uniform mat4 camera;

void main() {
    gColor = vec4(color, 1.0f);
    gl_Position = vec4(position, 1.0);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec4 gColor;

void main() {
    FragColor = vec4(1,1,1,1);
}