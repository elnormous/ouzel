#version 330
in vec3 position0;
in vec4 color0;
uniform mat4 modelViewProj;
out vec4 exColor;
void main()
{
    gl_Position = modelViewProj * vec4(position0, 1.0);
    exColor = color0;
}
