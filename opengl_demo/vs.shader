#version 460

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) in vec4 vertex_normal;

uniform mat4 projection_mat_shader;
uniform mat4 view_mat_shader;
uniform mat4 model_mat_shader;

out vec3 position_eye;
out vec3 normal_eye;

out vec4 fragment_color;

void main(void)
{
	position_eye = vec3(view_mat_shader * model_mat_shader * vertex_position);
	normal_eye = vec3(view_mat_shader * model_mat_shader * vertex_normal);

	gl_Position = projection_mat_shader * vec4(position_eye, 1.0);

	fragment_color = vertex_color;
}
