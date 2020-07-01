#version 330 core

layout (location = 0) in vec4 pos;

out vec2 texCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection*vec4(pos.xy, 0.0f, 1.0f);
    texCoords = pos.zw;
}  