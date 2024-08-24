#version 460

in vec3
position_eye,
normal_eye;

uniform mat4 view_mat_shader;

in vec4 fragment_color;
out vec4 out_color;

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color; 
layout (location = 2) in vec4 normal;

void main() {


	vec3 light_position_world = vec3(5.0, 2.0, 5.0);

	vec3 light_position_eye = vec3(view_mat_shader * vec4(light_position_world, 0.6));


	vec3 postion_viewer = vec3(0.0, 0.0, 0.0);

	vec3 n_eye = normalize(normal_eye);

	vec3 distance_to_light_eye = light_position_eye - position_eye;

	vec3 direction_to_light_eye = normalize(distance_to_light_eye);

	vec3 surface_to_viewer_eye = postion_viewer - position_eye;

	surface_to_viewer_eye = normalize(surface_to_viewer_eye);


	float dot_prod = dot(direction_to_light_eye, n_eye);

	dot_prod = max(dot_prod, 0.0);

	vec3 Ld = vec3(0.5, 0.5, 0.5);


	vec3 Kd = vec3(1.0, 0.5, 0.0);

	vec3 Id = Ld * Kd * dot_prod;

	vec3 reflection_eye = reflect(-direction_to_light_eye, n_eye);


	float dot_prod_specular = dot(surface_to_viewer_eye, reflection_eye);

	dot_prod_specular = max(dot_prod_specular, 0.0);


	float specular_exponent = 100.0;

	float specular_factor = pow(dot_prod_specular, specular_exponent);


	vec3 Ls = vec3(1.0, 1.0, 1.0);


	vec3 Ks = vec3(1.0, 1.0, 1.0);


	vec3 Is = Ls * Ks * specular_factor;


	vec3 La = vec3(0.3, 0.3, 0.3);


	vec3 Ka = vec3(1.0, 1.0, 1.0);


	vec3 Ia = La * Ka;


	out_color = vec4(Is + Id + Ia, 1.0)  * fragment_color;

}
