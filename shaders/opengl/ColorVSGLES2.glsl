precision highp float;
attribute vec3 inPosition;
attribute vec4 inColor;
uniform mat4 modelViewProj;
varying lowp vec4 exColor;
void main()
{
    gl_Position = modelViewProj * vec4(inPosition, 1.0);
    exColor = inColor;
}
