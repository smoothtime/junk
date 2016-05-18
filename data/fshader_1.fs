#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
    //color = texture(ourTexture1, TexCoord);
    color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
}

