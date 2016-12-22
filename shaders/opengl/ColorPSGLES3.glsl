#version 300 es
precision mediump float;
uniform lowp vec4 color;
in lowp vec4 ex_Color;
out vec4 out_Color;
void main()
{
    out_Color = ex_Color * color;
}
