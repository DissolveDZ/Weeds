#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform bool use_normals;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewpos = view * model * vec4(aPos, 1.0);
    FragPos = viewpos.xyz; 
    TexCoords = aTexCoords;
    if (!use_normals)
    {
        Normal = vec3(0, 0, 1);
    }
    else
    {
        mat3 normalMatrix = transpose(inverse(mat3(model)));
        Normal = normalMatrix * aNormal;
    }

    gl_Position = projection * viewpos;
}