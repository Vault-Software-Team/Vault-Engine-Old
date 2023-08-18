#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 3) in vec2 g_texCoords;

out vec2 texCoords;

uniform mat4 lightProjection;
uniform mat4 model;

void main()
{
    texCoords = g_texCoords;
    gl_Position = lightProjection * model * vec4(aPos, 1.0);
}  

#shader fragment
#version 330 core

in vec2 texCoords;

uniform sampler2D texture_diffuse0;
uniform int isTex;

void main() {
    if(isTex == 1) {
        vec4 tex = texture(texture_diffuse0, texCoords);
        if(tex.a < 1) discard;
    }
}