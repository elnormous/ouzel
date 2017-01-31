#version 300 es
precision highp float;
in vec3 inPosition;
in vec4 inColor;
in vec2 inTexCoord0;
uniform mat4 modelViewProj;
out lowp vec4 exColor;
out vec2 exTexCoord;
void main()
{
    gl_Position = modelViewProj * vec4(inPosition, 1.0);
    exColor = inColor;
    exTexCoord = inTexCoord0;
}
