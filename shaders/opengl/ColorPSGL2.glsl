#version 120
uniform vec4 color;
varying vec4 ex_Color;
void main()
{
    gl_FragColor = ex_Color * color;
}
