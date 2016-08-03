#version 330 core
uniform vec3 objectID;
out vec4 color;

void main()
{
   color = vec4(objectID.x, objectID.y, objectID.z, 1.0f);
}