#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 3) in vec2 g_texCoords;

uniform mat4 model;
out vec2 m_texCoords;

void main() {
    m_texCoords = g_texCoords;
    gl_Position = model * vec4(aPos, 1);
}

#shader fragment
#version 330 core
in vec4 FragPos;
in vec2 texCoords;

uniform sampler2D texture_diffuse0;
uniform int isTex;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
    if(isTex == 1) {
        vec4 tex = texture(texture_diffuse0, texCoords);
        if(tex.a < 1) discard;
    }
    gl_FragDepth = length(FragPos.xyz - lightPos) / farPlane;
}

#shader geometry
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

in vec2 m_texCoords[];

uniform mat4 shadowMatrices[6];

out vec4 FragPos;
out vec2 texCoords;

void main() {
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face;
        for(int i = 0; i < 3; i++) {
            FragPos = gl_in[i].gl_Position;
            texCoords = m_texCoords[i];
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}