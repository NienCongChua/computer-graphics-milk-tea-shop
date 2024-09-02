#pragma region Library
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <Vector_Matrix.h>
#pragma endregion
using namespace std;

#pragma region Default
typedef vec4 point4;
typedef vec4 color4;       // tạo vec4 màu
const int NumPoints = 36;
point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/
point4 vertices[8];/* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8];/*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(0, 0, 1, 1.0);
	vertices[1] = point4(0, 1, 1, 1.0);
	vertices[2] = point4(1, 1, 1, 1.0);
	vertices[3] = point4(1, 0, 1, 1.0);
	vertices[4] = point4(0, 0, 0, 1.0);
	vertices[5] = point4(0, 1, 0, 1.0);
	vertices[6] = point4(1, 1, 0, 1.0);
	vertices[7] = point4(1, 0, 0, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
bool lightSwitches[9] = { false };
vec3 to_vec3(const vec4& u) {
	return vec3(u.x, u.y, u.z);
}
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(to_vec3(u), to_vec3(v)));

	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
// ----------------------------------------
const size_t BufferSize = sizeof(points) + sizeof(colors) + sizeof(normals);
const size_t VertexSize = sizeof(points[0]) + sizeof(colors[0]) + sizeof(normals[0]);
const size_t RgbOffset = sizeof(points[0]);  // Offset từ vị trí đến màu sắc
const size_t NormalOffset = sizeof(points[0]) + sizeof(colors[0]);  // Offset từ vị trí đến pháp tuyến

void generateGeometry(void)
{
	initCube();
	makeColorCube();
}
// định nghĩa stack cho ma trận
class MatrixStack {
	int    index;
	int    size;
	mat4* matrices;

public:
	MatrixStack(int numMatrices = 32) :index(0), size(numMatrices)
	{
		matrices = new mat4[numMatrices];
	}

	~MatrixStack()
	{
		delete[]matrices;
	}

	// phép toán đẩy vào
	mat4& push(const mat4& m) {
		assert(index + 1 < size);
		matrices[index++] = m;
		return matrices[index];
	}

	// phép toán lấy ra
	mat4& pop(void) {
		assert(index - 1 >= 0);
		index--;
		return matrices[index];
	}
};

MatrixStack  mvstack;

int CurrentWidth = 700,
CurrentHeight = 700;

GLuint
VaoId,
VboId,
VertexShaderId,
FragmentShaderId,
ProgramId;

GLuint  ColorBuffer, NormalBuffer;

mat4
model_mat_cpp,
view_mat_cpp,
projection_mat_cpp;

int
model_mat_location,
view_mat_location,
projection_mat_location;

// Dùng biến đổi mô hình
float
r[] = { 0.0f, 0.0f, 0.0f },
s[] = { 1.0f, 1.0f, 1.0f },
t[] = { 0.0f, 0.0f, 0.0f };

// Dùng kiểm tra tịnh tiến, quay, co giãn
bool
translated = false,
rotated = false,
scaled = false;

string ReadShaderSourceFile(string fileName) {
	fstream reader(fileName.c_str());
	string line;
	string code = "";
	while (getline(reader, line)) {
		code += line + "\n";
	}
	reader.close();
	return code;
}
#pragma endregion

#pragma region Khai bao bien
// Biến góc quay camera
GLfloat cameraAngleX = 0.0f;
GLfloat cameraAngleY = 0.0f;
GLfloat cameraAngleZ = 0.0f;

enum {
	_cuaTL,
	_cuaBT,
	_napTR,
	NumJointAngles
};

GLfloat
thetar[NumJointAngles] = {
	360,
	0,
	0
};

enum {
	_CuaChinh,
	CuaChinh
};
GLfloat
ttCuaChinh[CuaChinh] = {
	0.0
};

enum {
	_ghe1,
	_ghe2,
	_ghe3,
	_ghe4,
	_ghe5,
	_ghe6,
	Ghe
};

GLfloat ttGhe[Ghe] = {
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0
};
GLint DichGhe = _ghe1;

enum {
	_QuatTran1,
	_QuatTran2,
	_QuatTran3,
	_QuatTran4,
	_QuatTran5,
	_QuatTran6,
	quatTran,
	Quit
};
GLint qt = _QuatTran1;

GLfloat QuayQuat[quatTran] = {
	0.0,
	0.0,
	0.0,
	0.0,
	0.0,
	0.0

};

bool _switchAirCondition = true,
_switchQuatTran1 = false,
_switchQuatTran2 = false,
_switchQuatTran3 = false,
_switchQuatTran4 = false,
_switchQuatTran5 = false,
_switchQuatTran6 = false,
_switchCuaChinh = true;
float Alpha = 0.0f;
float Beta = 90.0f;

void IdleFunc1(void);
void IdleFunc(void);

float cameraAngle = 0.0f;
float _cam = 0.4;
bool _switchComputer = false;

vec3 lightDirections[9] = {
	vec3(10.0, 0.0, 0.0), // Light 1 direction
	vec3(0.0, 10.0, 0.0), // Light 2 direction
	vec3(0.0, 0.0, 10.0), // Light 3 direction
	vec3(10.0, 10.0, 0.0), // Light 4 direction
	vec3(10.0, 0.0, 10.0), // Light 5 direction
	vec3(0.0, 10.0, 10.0), // Light 6 direction
	vec3(10.0, 10.0, 10.0), // Light 7 direction
	vec3(-10.0, 0.0, 0.0), // Light 8 direction
	vec3(0.0, 0.0, 0.0)  // Light 9 direction
};
GLuint lightDirectionLocation[9];

#pragma endregion

#pragma region Default
void CreateVaoVbo()
{
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	// Tạo và gán buffer cho vị trí các điểm (points)
	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(point4), 0); // position
	glEnableVertexAttribArray(0);

	// Tạo và gán buffer cho màu sắc (colors)
	glGenBuffers(1, &ColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(color4), 0); // color
	glEnableVertexAttribArray(1);

	// Tạo và gán buffer cho vector pháp tuyến (normals)
	glGenBuffers(1, &NormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0); // normal
	glEnableVertexAttribArray(2);
}
// ------------------------------------------
void CreatShaders()
{
	string vertexSrc = ReadShaderSourceFile("./vs.shader");
	string fragmentSrc = ReadShaderSourceFile("./fs.shader");

	const GLchar* VertexShader = vertexSrc.c_str();
	const GLchar* FragmentShader = fragmentSrc.c_str();

	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
	glCompileShader(VertexShaderId);

	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
	glCompileShader(FragmentShaderId);

	ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);
	glLinkProgram(ProgramId);
	glUseProgram(ProgramId);
}
// ------------------------------------------
void CloseFunc()
{
	glUseProgram(0);

	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(FragmentShaderId);
	glDeleteShader(VertexShaderId);

	glDeleteProgram(ProgramId);

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}
// ------------------------------------------

GLuint ambientProductLocation[9];
GLuint diffuseProductLocation[9];
GLuint specularProductLocation[9];
GLuint lightPositionLocation[9];
GLuint shininessLocation;

void SetupUniformLocations() {
	// Lấy giá trị của các uniform locations
	for (int i = 0; i < 9; i++) {
		ambientProductLocation[i] = glGetUniformLocation(ProgramId, ("AmbientProduct[" + to_string(i) + "]").c_str());
		diffuseProductLocation[i] = glGetUniformLocation(ProgramId, ("DiffuseProduct[" + to_string(i) + "]").c_str());
		specularProductLocation[i] = glGetUniformLocation(ProgramId, ("SpecularProduct[" + to_string(i) + "]").c_str());
		lightPositionLocation[i] = glGetUniformLocation(ProgramId, ("LightPosition[" + to_string(i) + "]").c_str());
		lightDirectionLocation[i] = glGetUniformLocation(ProgramId, ("LightDirection[" + to_string(i) + "]").c_str());
	}
	shininessLocation = glGetUniformLocation(ProgramId, "Shininess");
}

GLfloat light[3] = { 74, 73, 67 };
float Nitpick(int a) {
	return (float)a / 255;
}
color4 ConvertRGB(int a, int b, int c) {
	return color4(Nitpick(a), Nitpick(b), Nitpick(c), 1.0);
}

color4 light_ambient = color4(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse = ConvertRGB(light[0], light[1], light[2]);
color4 light_specular = color4(1.0, 1.0, 1.0, 1.0);
point4 light_position[] = {
	point4(1, -10, -10, 1.0), // Light 1
	point4(-10, 10, -10, 1.0), // Light 2
	point4(10, 10, 10, 1.0), // Light 3
	point4(-10, -10, 10, 1.0), // Light 4
	point4(0, 10, 0, 1.0), // Light 5
	point4(0, -10, 0, 1.0), // Light 6
	point4(10, 0, 0, 1.0), // Light 7
	point4(-10, 0, 0, 1.0), // Light 8
	point4(0, 0, 10, 1.0) // Light 9
};
color4 material_ambient(0.2, 0.2, 0.2, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 1.0, 1.0, 1.0);
float material_shininess = 100;
int on = 0;

vec4 color(int a, int b, int c, int d = 1) {
	return vec4((float)a / 256, (float)b / 256, (float)c / 256, d);
}

void FillColor(vec4 color) {
	material_ambient = color;
	material_diffuse = color;
	material_specular = color;
	material_shininess = 100;

	// Tạo mảng để lưu trữ giá trị của các uniform
	GLfloat ambientProductArray[9][4];
	GLfloat diffuseProductArray[9][4];
	GLfloat specularProductArray[9][4];
	GLfloat lightPositionArray[9][4];
	GLfloat lightDirectionArray[9][3];

	// Calculate and update lighting properties for each light source
	for (int i = 0; i < 9; i++) {
		if (lightSwitches[i]) { // Check if the light is switched on
			color4 ambient_product = light_ambient * material_ambient;
			color4 diffuse_product = light_diffuse * material_diffuse;
			color4 specular_product = light_specular * material_specular;

			// Lưu trữ giá trị của các uniform vào mảng
			ambientProductArray[i][0] = ambient_product.x;
			ambientProductArray[i][1] = ambient_product.y;
			ambientProductArray[i][2] = ambient_product.z;
			ambientProductArray[i][3] = ambient_product.w;

			diffuseProductArray[i][0] = diffuse_product.x;
			diffuseProductArray[i][1] = diffuse_product.y;
			diffuseProductArray[i][2] = diffuse_product.z;
			diffuseProductArray[i][3] = diffuse_product.w;

			specularProductArray[i][0] = specular_product.x;
			specularProductArray[i][1] = specular_product.y;
			specularProductArray[i][2] = specular_product.z;
			specularProductArray[i][3] = specular_product.w;

			lightPositionArray[i][0] = light_position[i].x;
			lightPositionArray[i][1] = light_position[i].y;
			lightPositionArray[i][2] = light_position[i].z;
			lightPositionArray[i][3] = light_position[i].w;

			lightDirectionArray[i][0] = lightDirections[i].x;
			lightDirectionArray[i][1] = lightDirections[i].y;
			lightDirectionArray[i][2] = lightDirections[i].z;
		}
		else { // Set light properties to 0 if the light is off
			// Lưu trữ giá trị 0 vào mảng
			for (int j = 0; j < 4; j++) {
				ambientProductArray[i][j] = 0.0f;
				diffuseProductArray[i][j] = 0.0f;
				specularProductArray[i][j] = 0.0f;
				lightPositionArray[i][j] = 0.0f;
			}
		}
	}

	// Cập nhật tất cả các uniform cùng một lúc
	for (int i = 0; i < 9; i++) {
		glUniform4fv(ambientProductLocation[i], 1, ambientProductArray[i]);
		glUniform4fv(diffuseProductLocation[i], 1, diffuseProductArray[i]);
		glUniform4fv(specularProductLocation[i], 1, specularProductArray[i]);
		glUniform4fv(lightPositionLocation[i], 1, lightPositionArray[i]);
		glUniform3fv(lightDirectionLocation[i], 1, lightDirectionArray[i]);
	}

	glUniform1f(shininessLocation, material_shininess);
}

void cube()
{
	mvstack.push(model_mat_cpp);

	mat4 instance = identity_mat4();
	instance = scale(vec3(1, 1, 1));
	mat4 model_cube = model_mat_cpp * instance;

	glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_cube.m);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model_mat_cpp = mvstack.pop();
}
#pragma endregion

#pragma region Draw Function
namespace BinhNuoc {
	void Ke() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 8.5, -16)) *
			scale(vec3(4, 3.5, 0.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 8.5, -6)) *
			scale(vec3(4, 3.5, 0.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 12, -16)) *
			scale(vec3(4, 0.5, 10.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 10.5, -16)) *
			scale(vec3(4, 0.2, 10.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 9, -16)) *
			scale(vec3(4, 0.2, 10.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void BinhNuoc() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 12, -15.5)) *
			scale(vec3(3, 5, 3));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(3, 13.8, -14)) *
			scale(vec3(0.4, 0.2, 0.2));
		FillColor(color(0.0, 0.0, 0.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(3.4, 13.5, -14)) *
			scale(vec3(0.2, 0.8, 0.2));
		FillColor(color(0.0, 0.0, 0.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(3.4, 14.2, -14.1)) *
			scale(vec3(0.2, 0.2, 0.5));
		FillColor(color(0.0, 0.0, 0.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void KeBen() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 8.5, -11.6)) *
			scale(vec3(4, 1.5, 0.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 8.5, -8.6)) *
			scale(vec3(4, 1.5, 0.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 10, -11.6)) *
			scale(vec3(4, 0.5, 3.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace ThungRac {
	void Thungrac() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, 0)) *
			scale(vec3(3.5, 5, 0.5));
		FillColor(color(142.0, 176.0, 221.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, 3)) *
			scale(vec3(3.5, 5, 0.5));
		FillColor(color(142.0, 176.0, 221.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(3, 0, 0)) *
			scale(vec3(0.5, 5, 3.5));
		FillColor(color(142.0, 176, 221.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, 0)) *
			scale(vec3(0.5, 5, 3.5));
		FillColor(color(142.0, 176.0, 221.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void NapThung() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 5, 0)) *
			scale(vec3(3.5, 0.5, 3.5));
		FillColor(color(142.0, 176.0, 221.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace Nha {
	void nha() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25)) *
			scale(vec3(65, 0.05, 50));
		FillColor(color(126.0, 116.0, 114.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, -5, -25)) *
			scale(vec3(65, 5, 60));
		FillColor(color(126.0, 116.0, 114.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25)) *
			scale(vec3(65, 30, 0.05));
		FillColor(color(230.0, 210.0, 220.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 30, -25)) *
			scale(vec3(65, 0.05, 50));
		FillColor(color(126.0, 116.0, 114.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25)) *
			scale(vec3(0.05, 30, 50));
		FillColor(color(126.0, 116.0, 114.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(65, 0, -25)) *
			scale(vec3(0.05, 30, 50));
		FillColor(color(126.0, 116.0, 114.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(25, 0, -25)) *
			scale(vec3(0.05, 25, 20));
		FillColor(color(126.0, 116.0, 114.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(25, 25, -25)) *
			scale(vec3(0.05, 5, 20));
		FillColor(color(209, 10, 13));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 25, -5)) *
			scale(vec3(25, 5, 0.05));
		FillColor(color(209, 10, 13));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 20, -5)) *
			scale(vec3(25, 5, 0.05));
		FillColor(color(180, 159, 158));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -5)) *
			scale(vec3(10, 20, 0.05));
		FillColor(color(180, 159, 158));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(20, 0, -5)) *
			scale(vec3(5, 20, 0.05));
		FillColor(color(180, 159, 158));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace PhongTrong {
	void Ban() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25)) *
			scale(vec3(24.8, 0.5, 4));
		FillColor(color(144.0, 122.0, 101.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void Ban2() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25)) * rotate_y(90) *
			scale(vec3(16, 0.5, 4));
		FillColor(color(144.0, 122.0, 101.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void Ghe() {

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 6.5, -25)) * rotate_y(90) *
			scale(vec3(4, 0.5, 4));
		FillColor(color(144.0, 122.0, 101.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25.5)) *
			scale(vec3(0.5, 6.5, 0.5));
		FillColor(color(144.0, 122.0, 101.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -29)) *
			scale(vec3(0.5, 6.5, 0.5));
		FillColor(color(144.0, 122.0, 101.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(3.5, 0, -29)) *
			scale(vec3(0.5, 6.5, 0.5));
		FillColor(color(144.0, 122.0, 101.0));
		cube();

		model_mat_cpp = mvstack.pop();
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(3.5, 0, -25.5)) *
			scale(vec3(0.5, 6.5, 0.5));
		FillColor(color(144.0, 122.0, 101.0));
		cube();
		model_mat_cpp = mvstack.pop();

	}
}

namespace Cua {
	void cuaPhongTrong() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			scale(vec3(10, 20, 0.1));
		FillColor(color(150, 129, 110));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(1, 8.5, -1)) *
			scale(vec3(0.5, 0.5, 2));
		FillColor(color(15, 12, 11));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(1, 10.5, -1)) *
			scale(vec3(0.5, 0.5, 2));
		FillColor(color(15, 12, 11));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(1, 8.5, 0.5)) *
			scale(vec3(0.5, 2.5, 0.5));
		FillColor(color(15, 12, 11));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(1, 8.5, -1)) *
			scale(vec3(0.5, 2.5, 0.5));
		FillColor(color(15, 12, 11));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace TuLanh {
	void DeTu() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25)) *
			scale(vec3(4.5, 2, 1));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -23.9)) *
			scale(vec3(4.5, 2, 1));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -22.8)) *
			scale(vec3(4.5, 2, 1));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -21.7)) *
			scale(vec3(4.5, 2, 1));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -20.6)) *
			scale(vec3(4.5, 2, 1));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -19.5)) *
			scale(vec3(4.5, 2, 1));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void KhungTu() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25)) *
			scale(vec3(4, 0.1, 6.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 16.4, -25)) *
			scale(vec3(4, 0.1, 6.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25)) *
			scale(vec3(4, 16.5, 0.1));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -18.5)) *
			scale(vec3(4, 16.5, 0.1));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		//Mặt sau
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25)) *
			scale(vec3(0.1, 16.5, 6.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();
		// Đế trên
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 16.5, -25)) *
			scale(vec3(4.5, 2, 6.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 3.3, -25)) *
			scale(vec3(4, 0.1, 6.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 6.6, -25)) *
			scale(vec3(4, 0.1, 6.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 9.9, -25)) *
			scale(vec3(4, 0.1, 6.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 13.2, -25)) *
			scale(vec3(4, 0.1, 6.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void CanhTu() {
		//Cánh trước
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(4, -7.9, -14.5)) *
			scale(vec3(0.1, 16.5, 6.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(4, -1, -14)) *
			scale(vec3(0.6, 0.3, 0.3));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(4, 1, -14)) *
			scale(vec3(0.6, 0.3, 0.3));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(4.5, -1, -14)) *
			scale(vec3(0.3, 2.3, 0.3));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace DieuHoa {
	void Khoisaudh() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp * translate(vec3(64, 20, 0)) * scale(vec3(1, 4, 8));
		FillColor(color(255, 255, 255));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void Khoitruocdh() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp * translate(vec3(63, 21, 0)) * scale(vec3(1, 3, 8));
		FillColor(color(255, 255, 255));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void CanhGio() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp * translate(vec3(64, 20, 0)) * rotate_z(45) * rotate_z(Alpha) * scale(vec3(0.01, 1.42, 8));
		FillColor(color(255, 255, 255));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void khoiben() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp * translate(vec3(63, 20, 8)) * scale(vec3(2, 4, 0.3));
		FillColor(color(255, 255, 255));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void khoiben1() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp * translate(vec3(63, 20, -0.3)) * scale(vec3(2, 4, 0.3));
		FillColor(color(255, 255, 255));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void congtac() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp * translate(vec3(64.9, 10, 0)) * scale(vec3(0.1, 1, 0.5));
		FillColor(color(255, 255, 255));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void cangat() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp * translate(vec3(64.9, 10.4, 0.2)) * rotate_z(Beta) * rotate_z(45) * scale(vec3(0.1, 0.4, 0.1));
		FillColor(color(0, 0, 0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace BanGhe {
	void Ban() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(55, -10, -25.4)) * rotate_y(90) *
			scale(vec3(4, 0.5, 4));
		FillColor(color(246.0, 243.0, 236.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(53, -2, -22)) * rotate_y(90) *
			scale(vec3(8, 0.5, 8));
		FillColor(color(246.0, 243.0, 236.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(56.8, -9.5, -26.9)) * rotate_y(90) *
			scale(vec3(0.5, 8, 0.5));
		FillColor(color(246.0, 243.0, 236.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void GheNgoai() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 4.5, -25)) * rotate_y(90) *
			scale(vec3(4, 0.5, 4));
		FillColor(color(184.0, 68.0, 71.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -25.5)) *
			scale(vec3(0.5, 4.5, 0.5));
		FillColor(color(184.0, 68.0, 71.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, -29)) *
			scale(vec3(0.5, 4.5, 0.5));
		FillColor(color(184.0, 68.0, 71.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(3.5, 0, -29)) *
			scale(vec3(0.5, 4.5, 0.5));
		FillColor(color(184.0, 68.0, 71.0));
		cube();

		model_mat_cpp = mvstack.pop();
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(3.5, 0, -25.5)) *
			scale(vec3(0.5, 4.5, 0.5));
		FillColor(color(184.0, 68.0, 71.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void TuaLung1() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 4.5, -25)) * rotate_y(90) *
			scale(vec3(0.5, 6, 4));
		FillColor(color(184.0, 68.0, 71.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void TuaLung2() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 4.5, -25)) * rotate_y(90) *
			scale(vec3(4, 6, 0.5));
		FillColor(color(184.0, 68.0, 71.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace QuatTran {
	void QuatTranStatic() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(20, 5, 0)) *
			scale(vec3(0.5, 0.3, 0.5));
		FillColor(color(113 , 202.0, 204.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(20.15, 3.5, 0.15)) *
			scale(vec3(0.2, 1.5, 0.2));
		FillColor(color(113, 202.0, 204.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}

	void QuatTranRotate() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 1.5, 0)) *
			scale(vec3(0.8, 0.3, 0.8));
		FillColor(color(113, 202.0, 204.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0.1, 1.65, 0)) *
			scale(vec3(0.6, 0.05, 5));
		FillColor(color(113, 202.0, 204.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0.1, 1.65, -5)) *
			scale(vec3(0.6, 0.05, 5));
		FillColor(color(113, 202.0, 204.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0.2, 1.65, 0.1)) *
			scale(vec3(5, 0.05, 0.6));
		FillColor(color(113, 202.0, 204.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(-4.8, 1.65, 0.1)) *
			scale(vec3(5, 0.05, 0.6));
		FillColor(color(113, 202.0, 204.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace KhongGianBenNgoai {
	void banNgoi() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(60, -2, -35)) *
			scale(vec3(5, 0.1, 45));
		FillColor(color(150, 129, 110));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(60, -10, 10)) *
			scale(vec3(5, 8, 0.1));
		FillColor(color(150, 129, 110));
		cube();
		model_mat_cpp = mvstack.pop();
	}

	void ghe() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(60, -12, -4));
		PhongTrong::Ghe();
		model_mat_cpp = mvstack.pop();
	}
}

namespace CuaQuan {
	void Tuong() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(50, 0, 4.5)) *
			scale(vec3(15, 25, 0.05));
		FillColor(color(246.0, 243.0, 236.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, 4.5)) *
			scale(vec3(30, 25, 0.05));
		FillColor(color(246.0, 243.0, 236.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 25, 4.5)) *
			scale(vec3(65, 5, 0.05));
		FillColor(color(207.0, 14.0, 18.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}

	void KhungCua() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(50, 0, 4.5)) *
			scale(vec3(0.2, 25, 0.1));
		FillColor(color(207.0, 14.0, 18.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(30, 0, 4.5)) *
			scale(vec3(0.2, 25, 0.1));
		FillColor(color(207.0, 14.0, 18.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(30, 0, 4.5)) *
			scale(vec3(20, 0.2, 0.1));
		FillColor(color(207.0, 14.0, 18.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(25, 25, 4.5)) *
			scale(vec3(20, 0.2, 0.1));
		FillColor(color(207.0, 14.0, 18.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(30.1, 0.1, 4.5)) *
			scale(vec3(19.8, 24.8, 0.1));
		FillColor(color(246.0, 243.0, 236.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace Table {
	void back_Table() {
		// mặt bàn
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, -2, -6)) *
			scale(vec3(6, 0.5, 15));
		FillColor(color(219, 187, 138));
		cube();
		model_mat_cpp = mvstack.pop();
		// Khối đằng trước
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(5, -10, -6)) *
			scale(vec3(0.5, 8, 15));
		FillColor(color(192, 192, 192));
		cube();
		model_mat_cpp = mvstack.pop();
		// Khôi bên cạnh_1
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, -10, -6)) *
			scale(vec3(6, 8, 0.2));
		FillColor(color(192, 192, 192));
		cube();
		model_mat_cpp = mvstack.pop();
		// Khôi bên cạnh_2
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, -10, 8.8)) *
			scale(vec3(6, 8, 0.2));
		FillColor(color(192, 192, 192));
		cube();
		model_mat_cpp = mvstack.pop();
	}
	void font_table() {
		// mặt bàn
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(14.5, -3, -5)) *
			scale(vec3(8, 0.5, 20));
		FillColor(color(255, 0, 0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(22, -10, -5)) *
			scale(vec3(0.3, 7, 20));
		FillColor(color(192, 192, 192));
		cube();
		model_mat_cpp = mvstack.pop();
		// Khối bên cạnh1
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(15, -10, 14.6)) *
			scale(vec3(7, 7, 0.3));
		FillColor(color(192, 192, 192));
		cube();
		model_mat_cpp = mvstack.pop();
		// Khối bên cạnh2
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(15, -10, -5)) *
			scale(vec3(7, 7, 0.3));
		FillColor(color(192, 192, 192));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace Poster {
	void Poster1() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(21, 7.5, -14)) * rotate_z(20) *
			scale(vec3(0.5, 4, 2.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(18, 7.5, -14)) * rotate_z(-25) *
			scale(vec3(0.5, 4, 2.5));
		FillColor(color(240.0, 240.0, 240.0));
		cube();
		model_mat_cpp = mvstack.pop();
	}

	void MayTinh() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(17, 7.5, -2.5)) *
			scale(vec3(3, 0.5, 4));
		FillColor(color(0, 0, 0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(18, 8, -0.7)) *
			scale(vec3(0.5, 4.5, 0.5));
		FillColor(color(0, 0, 0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp * rotate_z(-35) *
			translate(vec3(7.5, 18.5, -3)) *
			scale(vec3(0.5, 3, 5));
		FillColor(color(0, 0, 0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(14.5, 7.5, -3)) *
			scale(vec3(2, 0.3, 5));
		FillColor(color(0, 0, 0));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(17, 0, 2)) *
			scale(vec3(4, 4, 2));
		FillColor(color(0, 0, 0));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace Television {
	void TV1() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(17, 11.72, 7)) *
			scale(vec3(0.2, 10, 0.2));
		FillColor(color(0, 0, 0));
		cube();
		model_mat_cpp = mvstack.pop();
		// Tv1
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(15.2, 10, 3)) * rotate_z(-45) *
			scale(vec3(0.2, 5, 8));
		FillColor(color(192, 192, 192));
		cube();
		model_mat_cpp = mvstack.pop();
		// Dây Điện
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(17, 11.72, -2)) *
			scale(vec3(0.2, 10, 0.2));
		FillColor(color(0, 0, 0));
		cube();
		model_mat_cpp = mvstack.pop();
		// Tv2
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(15.2, 10, -6)) * rotate_z(-45) *
			scale(vec3(0.2, 5, 8));
		FillColor(color(192, 192, 192));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}

namespace Camera {
	void cameraStatic() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			scale(vec3(0.5, 0.2, 0.5));
		FillColor(color(127, 127, 127));
		cube();
		model_mat_cpp = mvstack.pop();
	}

	void cameraRotate() {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			scale(vec3(0.5, 0.5, 0.45));
		FillColor(color(200, 200, 200));
		cube();
		model_mat_cpp = mvstack.pop();

		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp *
			translate(vec3(0, 0, 0.45)) *
			scale(vec3(0.5, 0.5, 0.05));
		FillColor(color(20, 20, 20));
		cube();
		model_mat_cpp = mvstack.pop();
	}
}
#pragma endregion

#pragma region Display Function
void DisplayFunc(void)
{
#pragma region Default
	model_mat_cpp = identity_mat4();
	model_mat_location = glGetUniformLocation(ProgramId, "model_mat_shader");
	glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat_cpp.m);
	if (translated)
	{
		model_mat_cpp = model_mat_cpp * translate(vec3(t[0], t[1], t[2]));
		glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat_cpp.m);
	}
	if (scaled)
	{
		model_mat_cpp = model_mat_cpp * scale(vec3(s[0], s[1], s[2]));
		glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat_cpp.m);
	}
	
	// model_mat_cpp = model_mat_cpp * translate(vec3(-40, -10, -25));

	// Các tham số của hàm lookat
	vec3	eye(0.0f, 0.0f, 2.0f),
		at(0.0f, 0.0f, 0.0f),
		up(0.0f, 1.0f, 0.0f);

	// Áp dụng góc quay camera theo trục Z
	eye = vec3(eye.x * cos(cameraAngleZ) - eye.y * sin(cameraAngleZ),
		eye.x * sin(cameraAngleZ) + eye.y * cos(cameraAngleZ),
		eye.z);

	// Áp dụng góc quay camera theo trục Y
	eye = vec3(eye.x * cos(cameraAngleY) + eye.z * sin(cameraAngleY),
		eye.y,
		-eye.x * sin(cameraAngleY) + eye.z * cos(cameraAngleY));

	// Áp dụng góc quay camera theo trục X
	eye = vec3(eye.x,
		eye.y * cos(cameraAngleX) - eye.z * sin(cameraAngleX),
		eye.y * sin(cameraAngleX) + eye.z * cos(cameraAngleX));

	view_mat_cpp = lookat(eye, at, up);
	view_mat_location = glGetUniformLocation(ProgramId, "view_mat_shader");
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat_cpp.m);

	float
		znear = 0.1f,
		zfar = 100.0f,
		fov = 90.0f,
		aspect = (float)CurrentWidth / CurrentHeight;
	projection_mat_cpp = perspective(fov, aspect, znear, zfar);
	projection_mat_location = glGetUniformLocation(ProgramId, "projection_mat_shader");
	glUniformMatrix4fv(projection_mat_location, 1, GL_FALSE, projection_mat_cpp.m);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(0.0, 0.0, 0.0, 0.0); // Set background color to black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//------------draw-----------
	model_mat_cpp = model_mat_cpp * identity_mat4();
#pragma endregion
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, -10));
	Nha::nha();
	DieuHoa::Khoisaudh();
	DieuHoa::Khoitruocdh();
	DieuHoa::CanhGio();
	DieuHoa::khoiben();
	DieuHoa::khoiben1();
	DieuHoa::congtac();
	DieuHoa::cangat();
	model_mat_cpp = mvstack.pop();

#pragma region Binh Nuoc
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 10.5));
	BinhNuoc::Ke();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 10.5));
	BinhNuoc::BinhNuoc();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 13.8));
	BinhNuoc::BinhNuoc();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 17.1));
	BinhNuoc::BinhNuoc();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 17.1));
	BinhNuoc::KeBen();
	model_mat_cpp = mvstack.pop();

#pragma endregion

#pragma region Thung Rac
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 10.5));
	ThungRac::Thungrac();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp * translate(vec3(0, 0, 16.5));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -5, 0)) *
		translate(vec3(0, 0, -3)) *
		rotate_z(thetar[_napTR]) *
		translate(vec3(0, 0, -3)) *
		translate(vec3(0, -5, 0));
	//translate(vec3(0, 5, 0));
	ThungRac::NapThung();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

#pragma endregion

#pragma region Bàn Ghế
	//Trong cung
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(-12, 0, 3));
	BanGhe::Ban();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(36, -10, 3));
	BanGhe::GheNgoai();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(36, -10, 3));
	BanGhe::TuaLung2();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(43, -10, 10));
	BanGhe::GheNgoai();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(43, -10, 10));
	BanGhe::TuaLung1();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(43, -10, -4));
	BanGhe::GheNgoai();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(43, -10, -7.5));
	BanGhe::TuaLung1();
	model_mat_cpp = mvstack.pop();

	//Ngoai
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(-12, 0, 25));
	BanGhe::Ban();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(36, -10, 25));
	BanGhe::GheNgoai();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(36, -10, 25));
	BanGhe::TuaLung2();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(43, -10, 18));
	BanGhe::GheNgoai();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(43, -10, 14.5));
	BanGhe::TuaLung1();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(43, -10, 32));
	BanGhe::GheNgoai();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(43, -10, 32));
	BanGhe::TuaLung1();
	model_mat_cpp = mvstack.pop();
#pragma endregion

#pragma region Khong gian ben ngoai
	KhongGianBenNgoai::banNgoi();
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(-ttGhe[_ghe1], 0, 0));
	KhongGianBenNgoai::ghe();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(-ttGhe[_ghe2], 0, 7));
	KhongGianBenNgoai::ghe();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(-ttGhe[_ghe3], 0, 14));
	KhongGianBenNgoai::ghe();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(-ttGhe[_ghe4], 0, 21));
	KhongGianBenNgoai::ghe();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(-ttGhe[_ghe5], 0, 28));
	KhongGianBenNgoai::ghe();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(-ttGhe[_ghe6], 0, 35));
	KhongGianBenNgoai::ghe();
	model_mat_cpp = mvstack.pop();
#pragma endregion

#pragma region Quat tran
	// Quat tran 1
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(35, 14.7, -27));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp;
	QuatTran::QuatTranStatic();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp * translate(vec3(19.85, 1.7, -0.15));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0.4, 0, 0.4)) *
		rotate_y(QuayQuat[_QuatTran1]) *
		translate(vec3(-0.4, 0, -0.4));
	QuatTran::QuatTranRotate();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

	// Quat tran 2
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(19, 14.7, -27));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp;
	QuatTran::QuatTranStatic();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp * translate(vec3(19.85, 1.7, -0.15));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0.4, 0, 0.4)) *
		rotate_y(QuayQuat[_QuatTran2]) *
		translate(vec3(-0.4, 0, -0.4));
	QuatTran::QuatTranRotate();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

	// Quat tran 3
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(35, 14.7, -10));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp;
	QuatTran::QuatTranStatic();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp * translate(vec3(19.85, 1.7, -0.15));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0.4, 0, 0.4)) *
		rotate_y(QuayQuat[_QuatTran3]) *
		translate(vec3(-0.4, 0, -0.4));
	QuatTran::QuatTranRotate();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

	// Quat tran 4
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(19, 14.7, -10));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp;
	QuatTran::QuatTranStatic();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp * translate(vec3(19.85, 1.7, -0.15));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0.4, 0, 0.4)) *
		rotate_y(QuayQuat[_QuatTran4]) *
		translate(vec3(-0.4, 0, -0.4));
	QuatTran::QuatTranRotate();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

	// Quat tran 5
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(35, 14.7, 7));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp;
	QuatTran::QuatTranStatic();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp * translate(vec3(19.85, 1.7, -0.15));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0.4, 0, 0.4)) *
		rotate_y(QuayQuat[_QuatTran5]) *
		translate(vec3(-0.4, 0, -0.4));
	QuatTran::QuatTranRotate();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

	// Quat tran 6
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(19, 14.7, 7));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp;
	QuatTran::QuatTranStatic();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp * translate(vec3(19.85, 1.7, -0.15));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0.4, 0, 0.4)) *
		rotate_y(QuayQuat[_QuatTran6]) *
		translate(vec3(-0.4, 0, -0.4));
	QuatTran::QuatTranRotate();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();
#pragma endregion

#pragma region Poster
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 10.5));
	Poster::Poster1();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 13.1));
	Poster::Poster1();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 15.7));
	Poster::Poster1();
	model_mat_cpp = mvstack.pop();
#pragma endregion

#pragma region Phòng trong
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0.05, 0, -10));
	PhongTrong::Ban();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, 0, 10));
	PhongTrong::Ban2();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(20, -10, -1));
	PhongTrong::Ghe();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(12, -10, -1));
	PhongTrong::Ghe();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(4, -10, 0));
	PhongTrong::Ghe();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(4, -10, 8));
	PhongTrong::Ghe();
	model_mat_cpp = mvstack.pop();
#pragma endregion

#pragma region Tu Lanh
	//Tủ lạnh
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 10.5));
	TuLanh::DeTu();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -7.8, 10.5));
	TuLanh::KhungTu();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp * translate(vec3(8, -15.8, -16));

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(-4, 7.9, 14.5))*
		translate(vec3(0, 0, -6.5)) *
		rotate_y(thetar[_cuaTL]) *
		translate(vec3(0, 0, -6.5)) *
		translate(vec3(-4, 7.9, 14.5));
	translate(vec3(4, -7.9, -14.5));
	TuLanh::CanhTu();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();
#pragma endregion

#pragma region Cua
	// Cua phong trong 
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp * translate(vec3(10, -10, -15));
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(10, 0, 0)) *
		rotate_y(thetar[_cuaBT]) *
		translate(vec3(-10, 0, 0));
	Cua::cuaPhongTrong();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

	// Cua chinh
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 10.5));
	CuaQuan::Tuong();
	model_mat_cpp = mvstack.pop();

	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0 - ttCuaChinh[_CuaChinh], -10, 10.5));
	CuaQuan::KhungCua();
	model_mat_cpp = mvstack.pop();

#pragma endregion

#pragma region table and television 
	Table::back_Table();
	Table::font_table();
	Television::TV1();
#pragma endregion

#pragma region Camera
	// Camera 1
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(45, 19.7, -34));
	Camera::cameraStatic();
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -0.55, 0)) *
		translate(vec3(0.25, 0, 0.25)) *
		rotate_y(cameraAngle) *
		translate(vec3(-0.25, 0, -0.25));
	Camera::cameraRotate();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

	// Camera 2
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, 19.7, 5)) *
		rotate_y(90);
	Camera::cameraStatic();
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -0.55, 0)) *
		translate(vec3(0.25, 0, 0.25)) *
		rotate_y(cameraAngle) *
		translate(vec3(-0.25, 0, -0.25));
	Camera::cameraRotate();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

	// Camera 3
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(12.5, 19.7, -14.5));
	Camera::cameraStatic();
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -0.55, 0)) *
		translate(vec3(0.25, 0, 0.25)) *
		rotate_y(cameraAngle) *
		translate(vec3(-0.25, 0, -0.25));
	Camera::cameraRotate();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

	// Camera 4
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(64.5, 19.7, -10)) *
		rotate_y(270);
	Camera::cameraStatic();
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -0.55, 0)) *
		translate(vec3(0.25, 0, 0.25)) *
		rotate_y(cameraAngle) *
		translate(vec3(-0.25, 0, -0.25));
	Camera::cameraRotate();
	model_mat_cpp = mvstack.pop();
	model_mat_cpp = mvstack.pop();

#pragma endregion

#pragma region MayTinh
	mvstack.push(model_mat_cpp);
	model_mat_cpp = model_mat_cpp *
		translate(vec3(0, -10, 10.5));
	Poster::MayTinh();
	model_mat_cpp = mvstack.pop();

	if (_switchComputer) {
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp * translate(vec3(0, -9.95, 10.5));
		mvstack.push(model_mat_cpp);
		model_mat_cpp = model_mat_cpp * rotate_z(-35) *
			translate(vec3(7.5, 18.5, -3)) *
			scale(vec3(0.02, 2.8, 4.8));
		FillColor(color(255, 255, 255));
		cube();
		model_mat_cpp = mvstack.pop();
		model_mat_cpp = mvstack.pop();
	}
	else {

	}
#pragma endregion
	glutSwapBuffers();
}
#pragma endregion

#pragma region Reshape Function
void ReshapeFunc(int Width, int Height)
{
	CurrentWidth = Width;
	CurrentHeight = Height;

	glViewport(0, 0, CurrentWidth, CurrentHeight);
}
#pragma endregion

#pragma region Idle Function
void IdleFunc1(void)
{
	Alpha -= 0.08f;
	if (Alpha < 30.0f) {
		glutIdleFunc(IdleFunc);
	}

	if (_switchQuatTran1) {
		QuayQuat[_QuatTran1] += 20;
		if (QuayQuat[_QuatTran1] > 360) {
			QuayQuat[_QuatTran1] -= 360;
		}
	}

	if (_switchQuatTran2) {
		QuayQuat[_QuatTran2] += 20;
		if (QuayQuat[_QuatTran2] > 360) {
			QuayQuat[_QuatTran2] -= 360;
		}
	}

	if (_switchQuatTran3) {
		QuayQuat[_QuatTran3] += 20;
		if (QuayQuat[_QuatTran3] > 360) {
			QuayQuat[_QuatTran3] -= 360;
		}
	}

	if (_switchQuatTran4) {
		QuayQuat[_QuatTran4] += 20;
		if (QuayQuat[_QuatTran4] > 360) {
			QuayQuat[_QuatTran4] -= 360;
		}
	}

	if (_switchQuatTran5) {
		QuayQuat[_QuatTran5] += 20;
		if (QuayQuat[_QuatTran5] > 360) {
			QuayQuat[_QuatTran5] -= 360;
		}
	}

	if (_switchQuatTran6) {
		QuayQuat[_QuatTran6] += 20;
		if (QuayQuat[_QuatTran6] > 360) {
			QuayQuat[_QuatTran6] -= 360;
		}
	}

	if (_switchCuaChinh == true && ttCuaChinh[_CuaChinh] < 20) {
		ttCuaChinh[_CuaChinh] += 0.001;
	}
	else if (_switchCuaChinh == false && ttCuaChinh[_CuaChinh] > 0) {
		ttCuaChinh[_CuaChinh] -= 0.001;
	}
	else {

	}

	if (cameraAngle < -60 || cameraAngle > 60) {
		_cam = -_cam;
	}
	cameraAngle += _cam;
	glutPostRedisplay();
}
void IdleFunc(void)
{
	Alpha += 0.08f;
	if (Alpha > 60.0f) {
		glutIdleFunc(IdleFunc1);
	}

	if (_switchQuatTran1) {
		QuayQuat[_QuatTran1] += 20;
		if (QuayQuat[_QuatTran1] > 360) {
			QuayQuat[_QuatTran1] -= 360;
		}
	}

	if (_switchQuatTran2) {
		QuayQuat[_QuatTran2] += 20;
		if (QuayQuat[_QuatTran2] > 360) {
			QuayQuat[_QuatTran2] -= 360;
		}
	}

	if (_switchQuatTran3) {
		QuayQuat[_QuatTran3] += 20;
		if (QuayQuat[_QuatTran3] > 360) {
			QuayQuat[_QuatTran3] -= 360;
		}
	}

	if (_switchQuatTran4) {
		QuayQuat[_QuatTran4] += 20;
		if (QuayQuat[_QuatTran4] > 360) {
			QuayQuat[_QuatTran4] -= 360;
		}
	}

	if (_switchQuatTran5) {
		QuayQuat[_QuatTran5] += 20;
		if (QuayQuat[_QuatTran5] > 360) {
			QuayQuat[_QuatTran5] -= 360;
		}
	}

	if (_switchQuatTran6) {
		QuayQuat[_QuatTran6] += 20;
		if (QuayQuat[_QuatTran6] > 360) {
			QuayQuat[_QuatTran6] -= 360;
		}
	}

	if (_switchCuaChinh == true && ttCuaChinh[_CuaChinh] < 20) {
		ttCuaChinh[_CuaChinh] += 0.1;
	}
	else if (_switchCuaChinh == false && ttCuaChinh[_CuaChinh] > 0) {
		ttCuaChinh[_CuaChinh] -= 0.1;
	}
	else {

	}

	if (cameraAngle < -60 || cameraAngle > 60) {
		_cam = -_cam;
	}
	cameraAngle += _cam;
	glutPostRedisplay();
}
void IdleFunc2(void)
{
	if (_switchAirCondition == true && Alpha > 0) {
		Alpha -= 0.08f;
	}

	if (_switchQuatTran1) {
		QuayQuat[_QuatTran1] += 20;
		if (QuayQuat[_QuatTran1] > 360) {
			QuayQuat[_QuatTran1] -= 360;
		}
	}

	if (_switchQuatTran2) {
		QuayQuat[_QuatTran2] += 20;
		if (QuayQuat[_QuatTran2] > 360) {
			QuayQuat[_QuatTran2] -= 360;
		}
	}

	if (_switchQuatTran3) {
		QuayQuat[_QuatTran3] += 20;
		if (QuayQuat[_QuatTran3] > 360) {
			QuayQuat[_QuatTran3] -= 360;
		}
	}

	if (_switchQuatTran4) {
		QuayQuat[_QuatTran4] += 20;
		if (QuayQuat[_QuatTran4] > 360) {
			QuayQuat[_QuatTran4] -= 360;
		}
	}

	if (_switchQuatTran5) {
		QuayQuat[_QuatTran5] += 20;
		if (QuayQuat[_QuatTran5] > 360) {
			QuayQuat[_QuatTran5] -= 360;
		}
	}

	if (_switchQuatTran6) {
		QuayQuat[_QuatTran6] += 20;
		if (QuayQuat[_QuatTran6] > 360) {
			QuayQuat[_QuatTran6] -= 360;
		}
	}

	if (_switchCuaChinh == true && ttCuaChinh[_CuaChinh] < 20) {
		ttCuaChinh[_CuaChinh] += 0.1;
	}
	else if (_switchCuaChinh == false && ttCuaChinh[_CuaChinh] > 0){
		ttCuaChinh[_CuaChinh] -= 0.1;
	}
	else {

	}

	if (cameraAngle < -60 || cameraAngle > 60) {
		_cam = -_cam;
	}
	cameraAngle += _cam;
	glutPostRedisplay();
}
#pragma endregion

#pragma region Keyboard event
void KeyboardFunc(unsigned char key, int x, int y)
{
	float cameraSpeed = 0.1f;
	switch (key) {
	case 27:
		glutLeaveMainLoop();
		break;
	case '=':
	case '+':
		s[0] *= 1.05f;
		s[1] *= 1.05f;
		s[2] *= 1.05f;
		scaled = true;
		break;
	case '-':
	case '_':
		s[0] /= 1.05f;
		s[1] /= 1.05f;
		s[2] /= 1.05f;
		scaled = true;
		break;
	case '1': lightSwitches[0] = !lightSwitches[0]; break;
	case '2': lightSwitches[1] = !lightSwitches[1]; break;
	case '3': lightSwitches[2] = !lightSwitches[2]; break;
	case '4': lightSwitches[3] = !lightSwitches[3]; break;
	case '5': lightSwitches[4] = !lightSwitches[4]; break;
	case '6': lightSwitches[5] = !lightSwitches[5]; break;
	case '7': lightSwitches[6] = !lightSwitches[6]; break;
	case '8': lightSwitches[7] = !lightSwitches[7]; break;
	case '9': lightSwitches[8] = !lightSwitches[8]; break;
	case 'W':
	case 'w': cameraAngleX += 0.1f; break; // Quay theo trục X
	case 's':
	case 'S': cameraAngleX -= 0.1f; break; // Quay theo trục X
	case 'D':
	case 'd': cameraAngleY += 0.1f; break; // Quay theo trục Y
	case 'A':
	case 'a': cameraAngleY -= 0.1f; break; // Quay theo trục Y
	case 'o':
		thetar[_cuaTL] += 5;
		if (thetar[_cuaTL] > 360) {
			thetar[_cuaTL] -= 5;
		}
		break;
	case 'O':
		thetar[_cuaTL] -= 5;
		if (thetar[_cuaTL] < 240) {
			thetar[_cuaTL] += 5;
		}
		break;
	case 'p':
		thetar[_cuaBT] += 5;
		if (thetar[_cuaBT] > 150) {
			thetar[_cuaBT] -= 5;
		}
		break;
	case 'P':
		thetar[_cuaBT] -= 5;
		if (thetar[_cuaBT] < 0) {
			thetar[_cuaBT] += 5;
		}
		break;
	case'\\':
		if (_switchAirCondition == false) {
			Beta += 90;
			glutIdleFunc(IdleFunc2);
			_switchAirCondition = true;
			break;
		}
		else {
			Beta -= 90;
			glutIdleFunc(IdleFunc);
			_switchAirCondition = false;
			break;
		}
	case '\'':
		if (qt == _QuatTran1) {
			_switchQuatTran1 = !_switchQuatTran1;
			break;
		}
		else if (qt == _QuatTran2) {
			_switchQuatTran2 = !_switchQuatTran2;
			break;
		}
		else if (qt == _QuatTran3) {
			_switchQuatTran3 = !_switchQuatTran3;
			break;
		}
		else if (qt == _QuatTran4) {
			_switchQuatTran4 = !_switchQuatTran4;
			break;
		}
		else if (qt == _QuatTran5) {
			_switchQuatTran5 = !_switchQuatTran5;
			break;
		}
		else if (qt == _QuatTran6) {
			_switchQuatTran6 = !_switchQuatTran6;
			break;
		}
		else {
			break;
		}
		break;
	case ';':
		_switchCuaChinh = !_switchCuaChinh;
		break;
	case '[':
		if (ttGhe[DichGhe] < 8) {
			ttGhe[DichGhe] += 0.1;
		}
		break;
	case ']':
		if (ttGhe[DichGhe] > 0) {
			ttGhe[DichGhe] -= 0.1;
		}
		break;
	case 'i':
		thetar[_napTR] += 5;
		if (thetar[_napTR] > 40) {
			thetar[_napTR] -= 5;
		}
		break;
	case 'I':
		thetar[_napTR] -= 5;
		if (thetar[_napTR] < 0) {
			thetar[_napTR] += 5;
		}
		break;
	}

}

void SpecialFunc(int key, int x, int y)
{

	switch (key)
	{
	case GLUT_KEY_LEFT:
		t[0] -= 0.5f;
		translated = true;
		break;
	case GLUT_KEY_RIGHT:
		t[0] += 0.5f;
		translated = true;
		break;
	case GLUT_KEY_DOWN:
		t[2] -= 0.5f;
		translated = true;
		break;
	case GLUT_KEY_UP:
		t[2] += 0.5f;
		translated = true;
		break;
	case GLUT_KEY_PAGE_UP:
		t[1] -= 0.5f;
		translated = true;
		break;
	case GLUT_KEY_PAGE_DOWN:
		t[1] += 0.5f;
		translated = true;
		break;
	case GLUT_KEY_ALT_R:
		_switchComputer = !_switchComputer;
		break;
	}
}
#pragma endregion

#pragma region Menu
void menu(int option)
{
	if (option == Quit) {
		exit(EXIT_SUCCESS);
	}
	qt = option;
	DichGhe = option;
}
#pragma endregion

#pragma region Main
int main(int argc, char* argv[])
{
	std::cout << "|--------------------- Huong Dan Su Dung ---------------------|" << std::endl;
	std::cout << "|     Phim     |                  Chuc nang                   |" << std::endl;
	std::cout << "|-------------------------------------------------------------|" << std::endl;
	std::cout << "| ;            |  Dong / Mo cua chinh                         |" << std::endl;
	std::cout << "| '            |  Bat / Tat quat tran                         |" << std::endl;
	std::cout << "| \\            |  Bat / Tat dieu hoa                          |" << std::endl;
	std::cout << "| Key Up       |  Di chuyen camera vao trong                  |" << std::endl;
	std::cout << "| Key Down     |  Di chuyen camera ra ngoai                   |" << std::endl;
	std::cout << "| Key Right    |  Di chuyen camera sang phai                  |" << std::endl;
	std::cout << "| Key Left     |  Di chuyen camera sang trai                  |" << std::endl;
	std::cout << "| Page Up      |  Di chuyen camera len tren                   |" << std::endl;
	std::cout << "| Page Down    |  Di chuyen camera xuong duoi                 |" << std::endl;
	std::cout << "| a            |  Quay camera sang trai                       |" << std::endl;
	std::cout << "| d            |  Quay camera sang phai                       |" << std::endl;
	std::cout << "| s            |  Quay camera xuong duoi                      |" << std::endl;
	std::cout << "| w            |  Quay camera len tren                        |" << std::endl;
	std::cout << "| 1 - 9        |  Dieu khien anh sang                         |" << std::endl;
	std::cout << "| o/O          |  Dong / Mo cua tu lanh                       |" << std::endl;
	std::cout << "| P/p          |  Dong / Mo cua phong nho                     |" << std::endl;
	std::cout << "| I/i          |  Dong / Mo nap thung rac                     |" << std::endl;
	std::cout << "| [            |  Di chuyen ghe ra ngoai                      |" << std::endl;
	std::cout << "| ]            |  Di chuyen ghe vao trong                     |" << std::endl;
	std::cout << "| Double Alt R |  Bat/tat may tinh tien                       |" << std::endl;
	std::cout << "| Right Mouse  |  Chon chuc nang                              |" << std::endl;
	std::cout << "|-------------------------------------------------------------|" << std::endl;
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(CurrentWidth, CurrentHeight);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow(argv[0]);

	glewExperimental = GL_TRUE;
	glewInit();

	generateGeometry();
	CreateVaoVbo();
	CreatShaders();

	SetupUniformLocations();

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutIdleFunc(IdleFunc2);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutCloseFunc(CloseFunc);

	// Create cabinet submenu
	int QuatTranMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Quat Tran 1", _QuatTran1);
	glutAddMenuEntry("Quat Tran 2", _QuatTran2);
	glutAddMenuEntry("Quat Tran 3", _QuatTran3);
	glutAddMenuEntry("Quat Tran 4", _QuatTran4);
	glutAddMenuEntry("Quat Tran 5", _QuatTran5); 
	glutAddMenuEntry("Quat Tran 6", _QuatTran6);
	glutAddMenuEntry("Quit", Quit);


	int GheMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Ghe 1", _ghe1);
	glutAddMenuEntry("Ghe 2", _ghe2);
	glutAddMenuEntry("Ghe 3", _ghe3);
	glutAddMenuEntry("Ghe 4", _ghe4);
	glutAddMenuEntry("Ghe 5", _ghe5);
	glutAddMenuEntry("Ghe 6", _ghe6);
	glutAddMenuEntry("Quit", Quit);


	// Create the main menu and attach the submenus
	int mainMenu = glutCreateMenu(menu);
	glutAddSubMenu("Quat tran", QuatTranMenu);
	glutAddSubMenu("Ghe", GheMenu);
	glutAddMenuEntry("Quit", Quit);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}
#pragma endregion