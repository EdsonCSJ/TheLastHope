#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform float layer_z;
uniform mat4 transform;
void main()
{
    gl_Position = transform * vec4(aPos, layer_z, 1.0);
    TexCoord = aTexCoord;
}