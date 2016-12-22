#version 330
uniform vec4 color;
uniform sampler2D texture0;
in vec4 ex_Color;
in vec2 ex_TexCoord;
out vec4 out_Color;
void main()
{
    out_Color = texture(texture0, ex_TexCoord) * ex_Color * color;
}
