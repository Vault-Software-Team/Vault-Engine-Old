#shader vertex
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}

#shader fragment
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{   
    float opacity = 1;
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    if (sampled.a == 0.0)
        discard;
    vec4 bgColor = vec4(0, 0, 0, 1);
    color = mix(bgColor, vec4(textColor, 1.0) * sampled, opacity);
}  