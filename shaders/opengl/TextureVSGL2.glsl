#version 120
attribute vec3 inPosition;
attribute vec4 inColor;
attribute vec2 inTexCoord0;
uniform mat4 modelViewProj;
varying vec4 exColor;
varying vec2 exTexCoord;
void main()
{
    gl_Position = modelViewProj * vec4(inPosition, 1.0);
    exColor = inColor;
    exTexCoord = inTexCoord0;
}
