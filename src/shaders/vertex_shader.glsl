#version 430

in layout(location=0) vec2 position;
in layout(location=1) vec3 vertex_color;

out vec3 color;

void main(){
    gl_Position = vec4(position, 0.0, 1.0);
    color = vertex_color;
}