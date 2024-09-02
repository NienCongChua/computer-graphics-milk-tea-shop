#version 460

in vec3 position_eye;
in vec3 normal_eye;
in vec4 fragment_color;

out vec4 out_color;

uniform vec4 AmbientProduct[9];
uniform vec4 DiffuseProduct[9];
uniform vec4 SpecularProduct[9];
uniform vec4 LightPosition[9]; // Uniform cho v? trí c?a các ngu?n sáng
uniform vec3 LightDirection[9]; // Uniform cho h??ng c?a các ngu?n sáng
uniform float Shininess;

void main() {
    vec3 N = normalize(normal_eye);
    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);
    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);

    for (int i = 0; i < 9; i++) {
        vec3 L = normalize(LightPosition[i].xyz - position_eye); // S?a dòng này ?? tính L d?a trên LightPosition
        vec3 E = normalize(-position_eye);
        vec3 H = normalize(L + E);

        float Kd = max(dot(L, N), 0.0);
        diffuse += DiffuseProduct[i] * Kd;
        specular += SpecularProduct[i] * pow(max(dot(N, H), 0.0), Shininess);

    }

    out_color = (ambient + diffuse + specular) * fragment_color;
    out_color.a = 1.0;
}