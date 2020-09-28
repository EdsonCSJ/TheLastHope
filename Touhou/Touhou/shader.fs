#version 330 core
out vec4 aply;

in vec2 TexCoord;

uniform sampler2D textures;
uniform float offsetx;
uniform float offsety;

void main()
{    
    vec4 texel = texture(textures,
    vec2(TexCoord.x + offsetx, TexCoord.y + offsety));
    if (texel.a < 0.5) {
        discard;
    }

    aply = texel;
}  