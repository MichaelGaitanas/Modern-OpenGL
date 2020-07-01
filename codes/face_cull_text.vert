#version 330 core

layout (location = 0) in vec4 pos;

out vec2 texCoords;

uniform mat4 textProjection;
uniform mat4 textModel;

void main()
{
    gl_Position = textProjection*textModel*vec4(pos.xy, 0.0f, 1.0f);
    texCoords = pos.zw;
}  