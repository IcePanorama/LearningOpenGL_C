#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertex_colors;

out vec3 v_vertex_colors;

uniform float u_offset;

void main()
{
    v_vertex_colors = vertex_colors;
    gl_Position = vec4(position.x, position.y + u_offset, position.z, 1.0f);
}
