#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(-aPos.x, -aPos.y, 0.0, 1.0);
    TexCoords = aTexCoords;
}