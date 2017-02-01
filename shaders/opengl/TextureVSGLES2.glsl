precision highp float;
attribute vec3 position0;
attribute vec4 color0;
attribute vec2 texCoord0;
uniform mat4 modelViewProj;
varying lowp vec4 exColor;
varying vec2 exTexCoord;
void main()
{
    gl_Position = modelViewProj * vec4(position0, 1.0);
    exColor = color0;
    exTexCoord = texCoord0;
}
