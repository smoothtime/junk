#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
void main()
{
    gl_Position = vec4(position.x - 0.3, position.y, position.z, 2.0);
}