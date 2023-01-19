#shader vertex
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 camera;
uniform mat4 model;

void main()
{
    gl_Position = camera * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}  

#shader fragment
#version 330 core
in vec2 TexCoords;
out vec4 color;
out vec4 BloomColor;

uniform sampler2D text;
uniform vec3 textColor;
uniform vec3 bloomColor;

void main()
{    
    if(texture(text, TexCoords).r < 0.1) discard;
    vec4 result = vec4(textColor, texture(text, TexCoords).r);
    color = result;

    if((bloomColor.r > 0.5 || bloomColor.g > 0.5 || bloomColor.b > 0.5)
    && (bloomColor.r < 0.7 || bloomColor.g < 0.7 || bloomColor.b < 0.7)) {
        color = result * (bloomColor.r * 20);
        BloomColor = vec4(bloomColor * 2, 1);
    } else if(bloomColor.r > 0.7 || bloomColor.g > 0.7 || bloomColor.b > 0.7) {
        color = result * (bloomColor.r * 30);
        BloomColor = vec4(bloomColor * 3, 1);
    } else {
        BloomColor = vec4(bloomColor, 1);
    }
    color.a = texture(text, TexCoords).r;
}  