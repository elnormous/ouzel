#version 300 es
precision mediump float;
uniform lowp vec4 color;
uniform lowp sampler2D texture0;
in lowp vec4 exColor;
in vec2 exTexCoord;
out vec4 outColor;
void main()
{
    outColor = texture(texture0, exTexCoord) * exColor * color;
}
