#version 330 core
layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 view_pos;
uniform vec4 color;
uniform bool use_color;
uniform bool flip;
uniform sampler2D tex;

void main()
{    
    // and the diffuse per-fragment color
    g_albedo = vec4(1);
    if (!use_color)
    {
        if (flip)
            g_albedo = texture(tex, vec2(1.0 - TexCoords.x, TexCoords.y));
        else
            g_albedo = texture(tex, TexCoords);
        if (g_albedo.a == 0.0)
            discard;
        g_albedo.rgb = pow(g_albedo.rgb, vec3(2.2));
    }
    else
    {
        g_albedo.rgb = color.rgb;
    }
    // store the fragment position vector in the first gbuffer texture
    g_position = FragPos;
    // also store the per-fragment normals into the gbuffer
    g_normal = normalize(Normal);
    // store specular intensity in g_albedo's alpha component
}  