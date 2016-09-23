#version 330
in vec3 in_Position;
in vec4 in_Color;
uniform mat4 modelViewProj;
out vec4 ex_Color;
void main(void)
{
    gl_Position = modelViewProj * vec4(in_Position, 1.0);
    ex_Color = in_Color;
}