#version 460 core

out vec4 FragColor;
in vec2 TexCoords;
uniform vec2 resolution;
uniform vec3 view_pos;

vec2 uv = gl_FragCoord.xy / resolution.xy;
vec4 top = vec4(0.2, 0.3, 0.6, 1.0);
vec4 bottom = vec4(0.7, 0.9, 1.0, 1.0);
void main()
{
    // mix(bottom, top, ((view_pos.y + gl_FragCoord.y) / resolution.y)).rgb, 1
    FragColor = vec4(mix(bottom, top, ((view_pos.y + gl_FragCoord.y) / resolution.y)).rgb, 1);
}