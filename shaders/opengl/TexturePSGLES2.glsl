precision mediump float;
uniform lowp vec4 color;
uniform lowp sampler2D texture0;
varying lowp vec4 ex_Color;
varying vec2 ex_TexCoord;
void main()
{
    gl_FragColor = texture2D(texture0, ex_TexCoord) * ex_Color * color;
}
