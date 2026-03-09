#version 430

in vec3 color;
out vec4 color_value;

void main()
{
    color_value = vec4(color, 1.0);
}