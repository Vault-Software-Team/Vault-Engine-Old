#shader vertex
#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 g_texCoords;

out vec2 texCoords;

void main() {
    gl_Position = vec4(inPos.x, inPos.y, 0, 1);
    texCoords = g_texCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform float gamma;
uniform float exposure;

void main() {
    vec4 fragment = texture(screenTexture, texCoords.st);
    vec4 bloom = texture(bloomTexture, texCoords.st);
    vec4 color = fragment;

    vec3 toneMapped = vec3(1.0) - exp(-color.rgb * exposure);

    vec3 res = vec3(0.0);
    float Z = 0.0;

    FragColor.rgb = pow(toneMapped, vec3(1.0/2.2));
}
