#version 330
in vec3 inPosition;
in vec4 inColor;
uniform mat4 modelViewProj;
out vec4 exColor;
void main()
{
    gl_Position = modelViewProj * vec4(inPosition, 1.0);
    exColor = inColor;
}
