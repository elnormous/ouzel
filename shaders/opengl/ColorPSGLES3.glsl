#version 300 es
precision mediump float;
uniform lowp vec4 color;
in lowp vec4 exColor;
out vec4 outColor;
void main()
{
    outColor = exColor * color;
}
