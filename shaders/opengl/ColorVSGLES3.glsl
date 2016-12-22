#version 300 es
precision highp float;
in vec3 in_Position;
in vec4 in_Color;
uniform mat4 modelViewProj;
out lowp vec4 ex_Color;
void main()
{
    gl_Position = modelViewProj * vec4(in_Position, 1.0);
    ex_Color = in_Color;
}
