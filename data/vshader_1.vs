#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    //TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
    float r = 0.0f;
    float g = 0.0f;
    float z = position.z + 2.25;
    TexCoord = vec2((z + position.x + position.y) / 3, (position.y + z) / 2);
    
}

