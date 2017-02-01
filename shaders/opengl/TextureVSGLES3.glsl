#version 300 es
precision highp float;
in vec3 position0;
in vec4 color0;
in vec2 texCoord0;
uniform mat4 modelViewProj;
out lowp vec4 exColor;
out vec2 exTexCoord;
void main()
{
    gl_Position = modelViewProj * vec4(position0, 1.0);
    exColor = color0;
    exTexCoord = texCoord0;
}
