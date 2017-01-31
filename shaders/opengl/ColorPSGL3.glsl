#version 330
uniform vec4 color;
in vec4 exColor;
out vec4 outColor;
void main()
{
    outColor = exColor * color;
}
