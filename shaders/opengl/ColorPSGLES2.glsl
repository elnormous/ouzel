precision mediump float;
uniform lowp vec4 color;
varying lowp vec4 ex_Color;
void main()
{
    gl_FragColor = ex_Color * color;
}
