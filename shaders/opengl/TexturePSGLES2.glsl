precision mediump float;
uniform lowp vec4 color;
uniform lowp sampler2D texture0;
varying lowp vec4 exColor;
varying vec2 exTexCoord;
void main()
{
    gl_FragColor = texture2D(texture0, exTexCoord) * exColor * color;
}
