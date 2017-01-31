#version 330
uniform vec4 color;
uniform sampler2D texture0;
in vec4 exColor;
in vec2 exTexCoord;
out vec4 outColor;
void main()
{
    outColor = texture(texture0, exTexCoord) * exColor * color;
}
