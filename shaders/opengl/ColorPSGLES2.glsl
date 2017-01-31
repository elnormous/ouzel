precision mediump float;
uniform lowp vec4 color;
varying lowp vec4 exColor;
void main()
{
    gl_FragColor = exColor * color;
}
