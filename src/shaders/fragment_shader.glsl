#version 410 core

in vec3 v_vertex_colors;
out vec4 color;

void main()
{
    color = vec4(v_vertex_colors.x, v_vertex_colors.y, v_vertex_colors.z, 1.0f);
}
