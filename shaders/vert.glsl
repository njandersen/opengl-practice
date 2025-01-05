#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColors;

uniform float uOffset = 0;

out vec3 v_vertexColors;

void main()
{
   v_vertexColors = vertexColors;

   gl_Position = vec4(position.x, position.y + uOffset, position.z, 1.0f);
}


