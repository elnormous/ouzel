#version 120
uniform vec4 color;
varying vec4 exColor;
void main()
{
    gl_FragColor = exColor * color;
}
