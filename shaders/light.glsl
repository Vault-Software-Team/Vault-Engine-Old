#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 g_texCoords;

uniform mat4 model;
uniform mat4 camera;

void main() {
    gl_Position = camera * model * vec4(position, 1.0);
}

#shader fragment
#version 330 core

uniform vec4 lightColor;

void main() {
    gl_FragColor = lightColor;
}

#shader geometry
#version 330 core

void main() {
    
}