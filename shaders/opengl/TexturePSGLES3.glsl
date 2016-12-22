#version 300 es
precision mediump float;
uniform lowp vec4 color;
uniform lowp sampler2D texture0;
in lowp vec4 ex_Color;
in vec2 ex_TexCoord;
out vec4 out_Color;
void main(void)
{
    out_Color = texture(texture0, ex_TexCoord) * ex_Color * color;
}