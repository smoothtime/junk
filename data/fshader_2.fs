#version 330 core
in vec4 vertColor;
in vec2 fTexCoord;

out vec4 color;

uniform vec4  uniColor;
uniform sampler2D  uniTexture;

void main()
{
   vec4 texColor = texture(uniTexture, fTexCoord);
   color = vec4((texColor.rgb * uniColor.rgb), uniColor.a);
}