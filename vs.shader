
#version 460

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) in vec3 vertex_normal; // Thay ??i vec4 thành vec3

uniform mat4 projection_mat_shader;
uniform mat4 view_mat_shader;
uniform mat4 model_mat_shader;
uniform vec3 LightDirection[9];

out vec3 position_eye;
out vec3 normal_eye;
out vec4 fragment_color;

void main(void)
{
    vec4 position_eye_4 = view_mat_shader * model_mat_shader * vertex_position;
    position_eye = position_eye_4.xyz / position_eye_4.w; // Chia cho w ?? chu?n hóa
    normal_eye = normalize(mat3(transpose(inverse(view_mat_shader * model_mat_shader))) * vertex_normal); 

    gl_Position = projection_mat_shader * position_eye_4;

    fragment_color = vertex_color;
}