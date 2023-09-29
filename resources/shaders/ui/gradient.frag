#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform vec4 start;
uniform vec4 end;
uniform float offset;

void main()
{
    float amount = smoothstep(0.0, 1.0, 1-TexCoords.y);
    FragColor = mix(start, end, clamp(amount-offset, 0.0, 1.0));
}