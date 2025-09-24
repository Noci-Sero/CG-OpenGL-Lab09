#version 330 core

out vec4 colour;
in vec4 vCol;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec4 tex1 = texture(texture1, TexCoord);
    vec4 tex2 = texture(texture2, TexCoord);
    
    colour = mix(tex1, tex2, tex2.a);
}