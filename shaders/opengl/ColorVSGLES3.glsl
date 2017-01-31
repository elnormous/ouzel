#version 300 es
precision highp float;
in vec3 inPosition;
in vec4 inColor;
uniform mat4 modelViewProj;
out lowp vec4 exColor;
void main()
{
    gl_Position = modelViewProj * vec4(inPosition, 1.0);
    exColor = inColor;
}
