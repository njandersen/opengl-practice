#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColors;

uniform mat4 uModelMatrix;
uniform mat4 uProjection;
uniform mat4 uViewMatrix;

out vec3 v_vertexColors;

void main()
{
   v_vertexColors = vertexColors;

   vec4 newPosition = uProjection * uViewMatrix * uModelMatrix * vec4(position, 1.0f);

   gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
}
