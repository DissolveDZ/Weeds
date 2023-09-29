#version 460 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D tex;
uniform vec4 text_color;

void main()
{   
    float d = texture(tex, TexCoords).r;
    float aaf = fwidth(d);
    float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d); 
    FragColor = vec4(text_color.rgb, text_color.a * alpha);
}