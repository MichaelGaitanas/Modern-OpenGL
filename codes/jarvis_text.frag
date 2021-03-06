#version 330 core

in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D text;
uniform vec3 textCol;

void main()
{    
    vec4 sampled = vec4(1.0f,1.0f,1.0f, texture(text, texCoords).r);
    fragColor = vec4(textCol, 1.0f)*sampled;
}  