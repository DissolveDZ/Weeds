#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;
uniform sampler2D tex;

void main()
{    
    FragColor = vec4(texture(tex, TexCoords).rgb, 1.0);
}