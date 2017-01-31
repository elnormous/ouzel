#version 120
uniform vec4 color;
uniform sampler2D texture0;
varying vec4 exColor;
varying vec2 exTexCoord;
void main()
{
    gl_FragColor = texture2D(texture0, exTexCoord) * exColor * color;
}
