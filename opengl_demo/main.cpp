//
//  main.cpp
//
//  Created by Richard Jacob on 25/8/2024.
//

#pragma region Library
#define GL_SILENCE_DEPRECATION
#include <GLES3/gl3.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glu.h>
#pragma endregion
#define M_PI 3.14152

GLboolean redFlag = true, switchOne = false, switchTwo = false, switchLamp = false, \
    amb1 = true, diff1 = false, spec1 = true, amb2 = true, diff2 = false, spec2 = true, \
    amb3 = true, diff3 = true, spec3 = true;
double windowHeight = 800, windowWidth = 600;
double eyeX = 24.0, eyeY = 5.0, eyeZ = 35.0, refX = 0, refY = 0, refZ = 0;
double theta = 180.0, y = 1.36, z = 7.97888;
GLfloat Alpha1 = 0, Alpha2 = 0, Alpha3 = 0;
GLfloat fanRotationAngle1 = 0.0f, fanRotationAngle2 = 0.0f, fanRotationAngle3 = 0.0f,
    fanRotationAngle4 = 0.0f;
GLboolean _switch1 = true, _switch2 = false, _switch3 = false;
// GLboolean doorSwitch1 = false;

enum {
    CuaNK,		    //3
    CuaPK,
    CuaTL,
    NumJointAngles,	//11
    Quit			//12
};

GLfloat
tuong_tac[NumJointAngles] = {
    0.0,
    0.0, 
    0.0
};

GLint angle = CuaNK;

static GLfloat v_cube[8][3] =
{
    {0.0, 0.0, 0.0}, //0
    {0.0, 0.0, 3.0}, //1
    {3.0, 0.0, 3.0}, //2
    {3.0, 0.0, 0.0}, //3
    {0.0, 3.0, 0.0}, //4
    {0.0, 3.0, 3.0}, //5
    {3.0, 3.0, 3.0}, //6
    {3.0, 3.0, 0.0}  //7
};

static GLubyte quadIndices[6][4] =
{
    {0, 1, 2, 3}, //bottom
    {4, 5, 6, 7}, //top
    {5, 1, 2, 6}, //front
    {0, 4, 7, 3}, // back is clockwise
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left is clockwise
};

static void getNormal3p
(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, \
    GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2 - x1;
    Uy = y2 - y1;
    Uz = z2 - z1;

    Vx = x3 - x1;
    Vy = y3 - y1;
    Vz = z3 - z1;

    Nx = Uy * Vz - Uz * Vy;
    Ny = Uz * Vx - Ux * Vz;
    Nz = Ux * Vy - Uy * Vx;

    glNormal3f(Nx, Ny, Nz);
}

void drawCube()
{
    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
            v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
            v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);
    }
    glEnd();
}

#pragma region Ve khoi lap phuong
void drawCube1(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX = 0, GLfloat ambY = 0, GLfloat ambZ = 0, \
    GLfloat difT = 1, GLfloat ambT = 1, GLfloat shine = 50)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, ambT };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, ambT };
    GLfloat mat_diffuse[] = { difX, difY, difZ, difT };
    GLfloat mat_specular[] = { 0.85, 0.85, 0.85, difT };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glBegin(GL_QUADS);

    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
            v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
            v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);
    }
    glEnd();
}

static GLfloat v_trapezoid[8][3] =
{
    {0.0, 0.0, 0.0}, //0
    {0.0, 0.0, 3.0}, //1
    {3.0, 0.0, 3.0}, //2
    {3.0, 0.0, 0.0}, //3
    {0.5, 3.0, 0.5}, //4
    {0.5, 3.0, 2.5}, //5
    {2.5, 3.0, 2.5}, //6
    {2.5, 3.0, 0.5}  //7
};

static GLubyte TquadIndices[6][4] =
{
    {0, 1, 2, 3}, //bottom
    {4, 5, 6, 7}, //top
    {5, 1, 2, 6}, //front
    {0, 4, 7, 3}, // back is clockwise
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left is clockwise
};
#pragma endregion

#pragma region Ve hinh lang tru day la hinh thang
// Hàm drawTrapezoid có chức năng vẽ một hình lăng trụ với hai đáy là hình thang \
// (có thể hiểu là hình hộp chữ nhật bị cắt xéo) sử dụng thư viện OpenGL.
void drawTrapezoid(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine = 50)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0 };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_emission[] = { difX, difY, difZ, 0.0 };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    if (switchLamp == true) {
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    }

    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(v_trapezoid[TquadIndices[i][0]][0], v_trapezoid[TquadIndices[i][0]][1], v_trapezoid[TquadIndices[i][0]][2],
            v_trapezoid[TquadIndices[i][1]][0], v_trapezoid[TquadIndices[i][1]][1], v_trapezoid[TquadIndices[i][1]][2],
            v_trapezoid[TquadIndices[i][2]][0], v_trapezoid[TquadIndices[i][2]][1], v_trapezoid[TquadIndices[i][2]][2]);

        glVertex3fv(&v_trapezoid[TquadIndices[i][0]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][1]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][2]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][3]][0]);
    }
    glEnd();
}
#pragma endregion


//Drawing pyramid *************************
static GLfloat v_pyramid[5][3] =
{
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 2.0},
    {2.0, 0.0, 2.0},
    {2.0, 0.0, 0.0},
    {1.0, 4.0, 1.0}
};

static GLubyte p_Indices[4][3] =
{
    {4, 1, 2},
    {4, 2, 3},
    {4, 3, 0},
    {4, 0, 1}
};

static GLubyte PquadIndices[1][4] =
{
    {0, 3, 2, 1}
};

#pragma region Vẽ hình chóp
void drawpyramid(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, \
    GLfloat difT = 1, GLfloat ambT = 1, GLfloat shine = 50)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, ambT};
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, ambZ};
    GLfloat mat_diffuse[] = { difX, difY, difZ, difT };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, difT };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_TRIANGLES);
    for (GLint i = 0; i < 4; i++)
    {
        getNormal3p(v_pyramid[p_Indices[i][0]][0], v_pyramid[p_Indices[i][0]][1], v_pyramid[p_Indices[i][0]][2],
            v_pyramid[p_Indices[i][1]][0], v_pyramid[p_Indices[i][1]][1], v_pyramid[p_Indices[i][1]][2],
            v_pyramid[p_Indices[i][2]][0], v_pyramid[p_Indices[i][2]][1], v_pyramid[p_Indices[i][2]][2]);
        glVertex3fv(&v_pyramid[p_Indices[i][0]][0]);
        glVertex3fv(&v_pyramid[p_Indices[i][1]][0]);
        glVertex3fv(&v_pyramid[p_Indices[i][2]][0]);
    }
    glEnd();

    glBegin(GL_QUADS);
    for (GLint i = 0; i < 1; i++)
    {
        getNormal3p(v_pyramid[PquadIndices[i][0]][0], v_pyramid[PquadIndices[i][0]][1], v_pyramid[PquadIndices[i][0]][2],
            v_pyramid[PquadIndices[i][1]][0], v_pyramid[PquadIndices[i][1]][1], v_pyramid[PquadIndices[i][1]][2],
            v_pyramid[PquadIndices[i][2]][0], v_pyramid[PquadIndices[i][2]][1], v_pyramid[PquadIndices[i][2]][2]);
        glVertex3fv(&v_pyramid[PquadIndices[i][0]][0]);
        glVertex3fv(&v_pyramid[PquadIndices[i][1]][0]);
        glVertex3fv(&v_pyramid[PquadIndices[i][2]][0]);
        glVertex3fv(&v_pyramid[PquadIndices[i][3]][0]);
    }
    glEnd();

}
#pragma endregion

#pragma region Ve da giac 2D loi
void polygon(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, \
    GLfloat difT = 1, GLfloat ambT = 1, GLfloat shine = 0)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, ambT};
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0 };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, difT };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(6, 0);
    glVertex2f(5.8, 1);
    glVertex2f(5.2, 2);
    glVertex2f(5, 2.2);
    glVertex2f(4, 2.8);
    glVertex2f(3, 3);
    glVertex2f(2, 2.8);
    glVertex2f(1, 2.2);
    glVertex2f(0.8, 2);
    glVertex2f(0.2, 1);
    //glVertex2f(0,0);

    glEnd();
}
#pragma endregion

#pragma region Ve hinh thang
void polygonLine(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0 };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glBegin(GL_LINE_STRIP);
    //glVertex2f(0,0);
    glVertex2f(6, 0);
    glVertex2f(5.8, 1);
    glVertex2f(5.2, 2);
    glVertex2f(5, 2.2);
    glVertex2f(4, 2.8);
    glVertex2f(3, 3);
    glVertex2f(2, 2.8);
    glVertex2f(1, 2.2);
    glVertex2f(0.8, 2);
    glVertex2f(0.2, 1);
    glVertex2f(0, 0);

    glEnd();
}
#pragma endregion

#pragma region Ve hinh cau
void drawSphere(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, \
    GLfloat difT = 1, GLfloat ambT = 1, GLfloat shine = 90)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, ambT };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0 };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, difT };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glutSolidSphere(0.5, 20, 20);
}
#pragma endregion

#pragma region Quay camera theo trục X, Y, Z
void rotateCameraX(double angleDegree) {
    // Chuyển đổi góc sang radian
    double angleRad = angleDegree * M_PI / 180.0;

    // Lấy vector up hiện tại của camera
    double upX = 0; // Giả sử vector up ban đầu là (0, 1, 0)
    double upY = 1;
    double upZ = 0;

    // Tính vector hướng nhìn hiện tại (đã chuẩn hóa)
    double lookX = refX - eyeX;
    double lookY = refY - eyeY;
    double lookZ = refZ - eyeZ;
    double lookLength = sqrt(lookX * lookX + lookY * lookY + lookZ * lookZ);
    lookX /= lookLength;
    lookY /= lookLength;
    lookZ /= lookLength;

    // Tính vector right (trục X cục bộ của camera)
    double rightX = lookY * upZ - lookZ * upY;
    double rightY = lookZ * upX - lookX * upZ;
    double rightZ = lookX * upY - lookY * upX;

    // Quay vector up và look quanh trục right
    // Lưu ý: Ta cần quay cả up và look để giữ camera ổn định
    double rotatedUpX = upX * cos(angleRad) + lookX * sin(angleRad);
    double rotatedUpY = upY * cos(angleRad) + lookY * sin(angleRad);
    double rotatedUpZ = upZ * cos(angleRad) + lookZ * sin(angleRad);

    double rotatedLookX = -upX * sin(angleRad) + lookX * cos(angleRad);
    double rotatedLookY = -upY * sin(angleRad) + lookY * cos(angleRad);
    double rotatedLookZ = -upZ * sin(angleRad) + lookZ * cos(angleRad);

    // Cập nhật điểm đặt camera
    refX = eyeX + rotatedLookX;
    refY = eyeY + rotatedLookY;
    refZ = eyeZ + rotatedLookZ;

    glutPostRedisplay();
}

void rotateCameraY(double angleDegree) {
    // Chuyển đổi góc sang radian
    double angleRad = angleDegree * M_PI / 180.0;

    // Tính vector hướng nhìn hiện tại (đã chuẩn hóa)
    double lookX = refX - eyeX;
    double lookY = refY - eyeY;
    double lookZ = refZ - eyeZ;

    // Quay vector hướng nhìn quanh trục Y (trục Y toàn cục)
    double rotatedLookX = lookX * cos(angleRad) - lookZ * sin(angleRad);
    double rotatedLookY = lookY; // Y không đổi khi xoay quanh trục Y toàn cục
    double rotatedLookZ = lookX * sin(angleRad) + lookZ * cos(angleRad);

    // Cập nhật điểm đặt camera
    refX = eyeX + rotatedLookX;
    refY = eyeY + rotatedLookY;
    refZ = eyeZ + rotatedLookZ;

    glutPostRedisplay();
}

void rotateCameraZ(double angleDegree) {
    // Giống với hàm rotateCameraY, chỉ khác là xoay quanh trục Z toàn cục
    double angleRad = angleDegree * M_PI / 180.0;
    double lookX = refX - eyeX;
    double lookY = refY - eyeY;
    double lookZ = refZ - eyeZ;

    double rotatedLookX = lookX * cos(angleRad) + lookY * sin(angleRad);
    double rotatedLookY = -lookX * sin(angleRad) + lookY * cos(angleRad);
    double rotatedLookZ = lookZ; // Z không đổi khi xoay quanh trục Z toàn cục

    refX = eyeX + rotatedLookX;
    refY = eyeY + rotatedLookY;
    refZ = eyeZ + rotatedLookZ;

    glutPostRedisplay();
}
#pragma endregion

#pragma region Di chuyển camera theo 3 trục X, Y, Z
void moveCamera(double distanceX, double distanceY, double distanceZ) {
    // Tính vector hướng nhìn hiện tại (đã chuẩn hóa)
    double lookX = refX - eyeX;
    double lookY = refY - eyeY;
    double lookZ = refZ - eyeZ;
    double lookLength = sqrt(lookX * lookX + lookY * lookY + lookZ * lookZ);
    lookX /= lookLength;
    lookY /= lookLength;
    lookZ /= lookLength;

    // Lấy vector up hiện tại của camera (giả sử ban đầu là (0, 1, 0))
    double upX = 0;
    double upY = 1;
    double upZ = 0;

    // Tính vector right (trục X cục bộ)
    double rightX = lookY * upZ - lookZ * upY;
    double rightY = lookZ * upX - lookX * upZ;
    double rightZ = lookX * upY - lookY * upX;

    // Chuẩn hóa vector right
    double rightLength = sqrt(rightX * rightX + rightY * rightY + rightZ * rightZ);
    rightX /= rightLength;
    rightY /= rightLength;
    rightZ /= rightLength;

    // Tịnh tiến điểm nhìn và điểm đặt camera theo các trục cục bộ
    eyeX += distanceX * rightX + distanceY * upY + distanceZ * lookX;
    eyeY += distanceX * rightY + distanceY * upY + distanceZ * lookY;
    eyeZ += distanceX * rightZ + distanceY * upZ + distanceZ * lookZ;
    refX += distanceX * rightX + distanceY * upY + distanceZ * lookX;
    refY += distanceX * rightY + distanceY * upY + distanceZ * lookY;
    refZ += distanceX * rightZ + distanceY * upZ + distanceZ * lookZ;

    glutPostRedisplay(); // Yêu cầu vẽ lại khung hình
}
#pragma endregion

void cupboard() 
{
    //Cupboard/Almari ************************************************************

        //cupboard
    glPushMatrix();
    glTranslatef(4, 0, 4.4);
    glScalef(0.5, 1, 0.5);
    drawCube1(0.5, 0.2, 0.2, 0.25, 0.1, 0.1);
    glPopMatrix();

    //cupboard's 1st vertical stripline
    glPushMatrix();
    glTranslatef(4, 1, 5.9);
    glScalef(0.5, 0.01, 0.0001);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();

    //cupboard's 2nd vertical stripline
    glPushMatrix();
    glTranslatef(4, 0.5, 5.9);
    glScalef(0.5, 0.01, 0.0001);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();

    //cupboard's last stripline
    glPushMatrix();
    glTranslatef(4, 0, 5.9);
    glScalef(0.5, 0.01, 0.0001);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();

    //cupboard's lst horizontal stripline
    glPushMatrix();
    glTranslatef(5.5, 0, 5.9);
    glScalef(0.01, 1, 0.0001);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();

    //cupboard's right side horizontal stripline
    glPushMatrix();
    glTranslatef(4.75, 1, 5.9);
    glScalef(0.01, 0.67, 0.0001);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();

    //cupboard's left side horizontal stripline
    glPushMatrix();
    glTranslatef(4, 0, 5.9);
    glScalef(0.01, 1, 0.0001);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();

    //cupboard's handle right
    glPushMatrix();
    glTranslatef(5, 1.4, 5.9);
    glScalef(0.02, 0.18, 0.01);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();

    //cupboard's handle right sphere
    glPushMatrix();
    glTranslatef(5.02, 1.9, 5.91);
    glScalef(0.02, 0.02, 0.01);
    drawSphere(0.2, 0.1, 0.1, 0.1, 0.05, 0.05, 10);
    glPopMatrix();

    //cupboard's handle left
    glPushMatrix();
    glTranslatef(4.5, 1.4, 5.9);
    glScalef(0.02, 0.18, 0.01);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();

    //cupboard's handle left sphere
    glPushMatrix();
    glTranslatef(4.52, 1.9, 5.91);
    glScalef(0.02, 0.02, 0.01);
    drawSphere(0.2, 0.1, 0.1, 0.1, 0.05, 0.05, 10);
    glPopMatrix();

    //cupboard's drawer's 1st handle
    glPushMatrix();
    glTranslatef(4.5, 0.7, 5.9);
    glScalef(0.16, 0.02, 0.01);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();

    //cupboard's drawer's 2nd handle
    glPushMatrix();
    glTranslatef(4.5, 0.25, 5.9);
    glScalef(0.16, 0.02, 0.01);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();
}

void room()
{
    // carpet
    glPushMatrix();
    glTranslatef(0, -5, 0);
    glScalef(6, 0.01, 10);
    drawCube1(0.4, 0.1, 0.0, 0.20, 0.05, 0.0);
    glPopMatrix();

    // back wall
    glPushMatrix();
    glTranslatef(0, -5, 0);
    glScalef(6, 3.5, 0.1);
    drawCube1(1, 0.8, 0.7, 0.5, 0.4, 0.35);
    glPopMatrix();

    // left wall
    glPushMatrix();
    glTranslatef(0, -5, 0);
    glScalef(0.1, 3.5, 9);
    drawCube1(1, 0.8, 0.7, 0.5, 0.4, 0.35);
    glPopMatrix();

    // right wall
    glPushMatrix();
    glTranslatef(17.7, -5, 0);
    glScalef(0.1, 3.5, 9);
    drawCube1(1, 0.8, 0.7, 0.5, 0.4, 0.35);
    glPopMatrix();

    // Top wall
    glPushMatrix();
    glTranslatef(0, 5.5, 0);
    glScalef(6, 0.1, 9);
    drawCube1(0.715, 0.59, 0.597, 0.867, 0.8125, 0.8164);
    glPopMatrix();

    // Front top wall
    glPushMatrix();
    glTranslatef(0, 3.8, 27);
    glScalef(6, 0.6, 0.05);
    drawCube1(0.852, 0.21875, 0.2305, 0.887, 0.3, 0.33);
    glPopMatrix();

#pragma region The room in the left
    // The front top in the room
    glPushMatrix();
    glTranslatef(12, 3.8, 13);
    glScalef(2, 0.6, 0.05);
    drawCube1(0.852, 0.21875, 0.2305, 0.887, 0.3, 0.33);
    glPopMatrix();

    // The frone left in the room
    glPushMatrix();
    glTranslatef(12, 3.8, 0);
    glScalef(0.05, 0.6, 4.35);
    drawCube1(0.852, 0.21875, 0.2305, 0.887, 0.3, 0.33);
    glPopMatrix();

    // The glass frone left in the room
    glPushMatrix();
    glTranslatef(12, -5.05, 0);
    glScalef(0.05, 2.95, 4.35);
    drawCube1(0.625, 0.586, 0.587, 0.734, 0.624, 0.628, 0.8, 0.85);
    glPopMatrix();

    // The glass frone top in the room
    glPushMatrix();
    glTranslatef(12, 2.602, 13);
    glScalef(2, 0.45, 0.05);
    drawCube1(0.625, 0.586, 0.587, 0.734, 0.624, 0.628, 0.8, 0.85);
    glPopMatrix();

    // The glass frone left bottom in the room
    glPushMatrix();
    glTranslatef(12, -5.05, 13);
    glScalef(0.3, 2.95, 0.05);
    drawCube1(0.625, 0.586, 0.587, 0.734, 0.624, 0.628, 0.8, 0.85);
    glPopMatrix();

    // The glass frone right bottom in the room
    glPushMatrix();
    glTranslatef(16.5, -5.05, 13);
    glScalef(0.5, 2.95, 0.05);
    drawCube1(0.625, 0.586, 0.587, 0.734, 0.624, 0.628, 0.8, 0.85);
    glPopMatrix();

    // The matix bottom
    glPushMatrix();
    glTranslatef(12, -5.05, 0);
    glScalef(0.05, 0.15, 4.35);
    drawCube1(0.2385, 0.2205, 0.2225, 0.2385, 0.2205, 0.2225);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(16.5, -5.05, 13);
    glScalef(0.5, 0.15, 0.05);
    drawCube1(0.2385, 0.2205, 0.2225, 0.2385, 0.2205, 0.2225);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(12, -5.05, 13);
    glScalef(0.3, 0.15, 0.05);
    drawCube1(0.2385, 0.2205, 0.2225, 0.2385, 0.2205, 0.2225);
    glPopMatrix();

    // Khung cua
    glPushMatrix();
    glTranslatef(12.9, -5.05, 13);
    glScalef(0.01, 2.55, 0.05);
    drawCube1(0.1, 0.1, 0.1, 0.1, 0.1, 0.1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(16.5, -5.05, 13);
    glScalef(0.01, 2.55, 0.05);
    drawCube1(0.1, 0.1, 0.1, 0.1, 0.1, 0.1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(12.9, 2.602, 13);
    glScalef(1.2, 0.01, 0.05);
    drawCube1(0.1, 0.1, 0.1, 0.1, 0.1, 0.1);
    glPopMatrix();

#pragma endregion

    // Cua kinh ngoai
    glPushMatrix();
    glTranslatef(10, -5.05, 27);
    glScalef(2.6, 3.6, 0.05);
    drawCube1(0.625, 0.586, 0.587, 0.734, 0.624, 0.628, 0.3, 0.4);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10, -5.05, 27.1);
    glScalef(0.05, 2.95, 0.05);
    drawCube1(0, 0, 0, 0, 0, 0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -5.0, 27);
    glScalef(6, 0.05, 0.05);
    drawCube1(0.05, 0.05, 0.05, 0.05, 0.05, 0.05);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -5.05, 27.1);
    glScalef(0.4, 2.95, 0.05);
    drawCube1(0.625, 0.586, 0.587, 0.734, 0.624, 0.628, 0.3, 0.4);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 3.75, 27);
    glScalef(6, 0.05, 0.05);
    drawCube1(0.05, 0.05, 0.05, 0.05, 0.05, 0.05);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.1, -5.05, 27.1);
    glScalef(0.05, 2.95, 0.05);
    drawCube1(0, 0, 0, 0, 0, 0);
    glPopMatrix();
}

namespace Cua {
    void CuaNhaKho() {
        glPushMatrix();
        glTranslatef(13, -5.04, 13);
        glScalef(1.15, 2.549, 0.05);
        drawCube1(0.625, 0.586, 0.587, 0.734, 0.624, 0.628, 0.95, 0.95);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16, -1, 13);
        glScalef(0.05, 0.05, 0.15);
        drawCube1(0.1, 0.1, 0.1, 0.1, 0.1, 0.1);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16, 0, 13);
        glScalef(0.05, 0.05, 0.15);
        drawCube1(0.1, 0.1, 0.1, 0.1, 0.1, 0.1);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16, -1, 13.3);
        glScalef(0.05, 0.35, 0.05);
        drawCube1(0.1, 0.1, 0.1, 0.1, 0.1, 0.1);
        glPopMatrix();
    }

    void CuaQuan() {
        glPushMatrix();
        glTranslatef(1.2, -4.8, 27.1);
        glScalef(2.95, 2.85, 0.05);
        drawCube1(0.625, 0.586, 0.587, 0.734, 0.624, 0.628, 0.85, 0.85);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.3, -1, 27.5);
        glScalef(0.5, 0.5, 0.05);
        drawSphere(0.852, 0.21875, 0.2305, 0.887, 0.3, 0.33);
        glPopMatrix();

    }

    void CuaTuLanh() {
        glPushMatrix();
        glTranslatef(0.45, 0, 0);
        glPushMatrix();
        glTranslatef(16.0, -3.6, 13.55);
        glScalef(0.01, 2, 0.79);
        drawCube1(0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(15.8, -1.0, 13.7);
        glScalef(0.07, 0.05, 0.05);
        drawCube1(0.15, 0.15, 0.15, 0.15, 0.15, 0.15);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(15.8, 0, 13.7);
        glScalef(0.07, 0.05, 0.05);
        drawCube1(0.15, 0.15, 0.15, 0.15, 0.15, 0.15);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(15.8, -1.0, 13.7);
        glScalef(0.05, 0.35, 0.05);
        drawCube1(0.15, 0.15, 0.15, 0.15, 0.15, 0.15);
        glPopMatrix();
        glPopMatrix();
    }
}

namespace Ghe {
    void chanGhe() {

    }
}

namespace QuatTran {
    void quat_tran_s() {
        glPushMatrix();
        glTranslatef(9.75, 3, 30.35);
        glScalef(0.3, 0.1, 0.3);
        glRotatef(180, 1, 0, 0);
        drawpyramid(0.348, 0.547, 0.492, 0.465, 0.806, 0.824);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(9.98, 2, 30);
        glScalef(0.05, 0.3, 0.05);
        drawCube1(0.348, 0.547, 0.492, 0.465, 0.806, 0.824);
        glPopMatrix();
    }

    void quat_tran_q() {

        // Canh quat
        glPushMatrix();
        glTranslatef(9.75, 2, 30);
        glScalef(0.2, 0.01, 1);
        drawCube1(0.348, 0.547, 0.492, 0.465, 0.806, 0.824);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(7.1, 2, 28.6);
        glRotatef(60, 0, 1, 0);
        glScalef(0.2, 0.01, 1);
        drawCube1(0.348, 0.547, 0.492, 0.465, 0.806, 0.824);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(10.1, 2, 30.3);
        glRotatef(120, 0, 1, 0);
        glScalef(0.2, 0.01, 1);
        drawCube1(0.348, 0.547, 0.492, 0.465, 0.806, 0.824);
        glPopMatrix();
    }

    void bau_quat() {
        // Bau quat
        glPushMatrix();
        glTranslatef(10, 2, 30);
        glScalef(1, 0.2, 1);
        drawSphere(0.348, 0.547, 0.492, 0.465, 0.806, 0.824);
        glPopMatrix();
    }
}

namespace TuLanh {
    void detulanh() {
        // De tu lanh
        glPushMatrix();
        glTranslatef(16.45, -5.05, 14);
        glScalef(0.5, 0.4, 0.8);
        drawCube1(0.836, 0.836, 0.836, 0.9, 0.9, 0.9);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16.45, 2.42, 14);
        glScalef(0.5, 0.4, 0.8);
        drawCube1(0.536, 0.536, 0.536, 0.7, 0.7, 0.7);
        glPopMatrix();
    }

    void VanKeTuLanh() {
        glPushMatrix();
        glTranslatef(16.4, -5.05, 14.3);
        glScalef(0.01, 0.4, 0.03);
        drawCube1(0.605, 0.605, 0.605, 0.605, 0.605, 0.605);
        glPopMatrix();
    }

    void VeTuLanh() {
        detulanh();
        VanKeTuLanh();
        glPushMatrix();
        glTranslatef(0, 0, 0.4);
        VanKeTuLanh();
        glPopMatrix();
        VanKeTuLanh();
        glPushMatrix();
        glTranslatef(0, 0, 0.8);
        VanKeTuLanh();
        glPopMatrix();
        VanKeTuLanh();
        glPushMatrix();
        glTranslatef(0, 0, 1.2);
        VanKeTuLanh();
        glPopMatrix();
        VanKeTuLanh();
        glPushMatrix();
        glTranslatef(0, 0, 1.6);
        VanKeTuLanh();
        glPopMatrix();
    }

    void VeThanTuLanh() {
        glPushMatrix();
        glTranslatef(16.45, -3.6, 14);
        glScalef(0.5, 2.0, 0.03);
        drawCube1(0.8, 0.8, 0.8, 0.8, 0.9, 0.9);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16.45, -3.6, 16.3);
        glScalef(0.5, 2.0, 0.03);
        drawCube1(0.8, 0.8, 0.8, 0.8, 0.9, 0.9);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.65, -3.6, 14);
        glScalef(0.02, 2.0, 0.8);
        drawCube1(0.6, 0.6, 0.6, 0.65, 0.65, 0.65);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16.45, -3.55, 14);
        glScalef(0.5, 0.02, 0.8);
        drawCube1(0.3, 0.3, 0.3, 0.4, 0.4, 0.4);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16.45, 2.35, 14);
        glScalef(0.5, 0.02, 0.8);
        drawCube1(0.3, 0.3, 0.3, 0.4, 0.4, 0.4);
        glPopMatrix();

        // Ngan tu lanh
        glPushMatrix();
        glTranslatef(16.45, -2.5, 14);
        glScalef(0.5, 0.02, 0.8);
        drawCube1(0.4, 0.4, 0.4, 0.5, 0.5, 0.5);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16.45, -1.4, 14);
        glScalef(0.5, 0.02, 0.8);
        drawCube1(0.4, 0.4, 0.4, 0.5, 0.5, 0.5);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16.45, -0.3, 14);
        glScalef(0.5, 0.02, 0.8);
        drawCube1(0.4, 0.4, 0.4, 0.5, 0.5, 0.5);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16.45, 0.8, 14);
        glScalef(0.5, 0.02, 0.8);
        drawCube1(0.4, 0.4, 0.4, 0.5, 0.5, 0.5);
        glPopMatrix();
    }

    void DoTrongTuLanh() {
#pragma region Ngan tu lanh 1
        // Ngan 1
        glPushMatrix();
        glTranslatef(17.3, -0.1, 14);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 14);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 14);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.3, -0.1, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.3, -0.1, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.3, -0.1, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.3, -0.1, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.3, -0.1, 15.5);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 15.5);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 15.5);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();

        /////////////////////////////////////////////////
        glPushMatrix();
        glTranslatef(0, 0, 0.12);

        glPushMatrix();
        glTranslatef(17.45, 0.1, 14);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.05, 0.1, 14);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.45, 0.1, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.05, 0.1, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.45, 0.1, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.05, 0.1, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.45, 0.1, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.05, 0.1, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.45, 0.1, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.05, 0.1, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();

        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.15, 0.33, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.15, 0.33, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.15, 0.33, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.15, 0.33, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.793, 0.543, 0.15625, 0.793, 0.543, 0.15625);
        glPopMatrix();
# pragma endregion

#pragma region Ngan tu lanh 2
        glPushMatrix();
        glTranslatef(0, -1.1, 0);

        glPushMatrix();
        glTranslatef(17.3, -0.1, 14);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 14);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 14);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.3, -0.1, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.3, -0.1, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.3, -0.1, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.3, -0.1, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.3, -0.1, 15.5);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.9, -0.1, 15.5);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.2, -0.1, 15.5);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();

        /////////////////////////////////////////////////
        glPushMatrix();
        glTranslatef(0, 0, 0.12);

        glPushMatrix();
        glTranslatef(17.45, 0.1, 14);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.05, 0.1, 14);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.45, 0.1, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.05, 0.1, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.45, 0.1, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.05, 0.1, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.45, 0.1, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.05, 0.1, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.45, 0.1, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.05, 0.1, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();

        glPopMatrix();
        glPushMatrix();
        glTranslatef(17.15, 0.33, 14.3);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.15, 0.33, 14.6);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.15, 0.33, 14.9);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(17.15, 0.33, 15.2);
        glScalef(0.3, 0.3, 0.3);
        drawSphere(0.85, 0.723, 0.2, 0.85, 0.723, 0.2);
        glPopMatrix();

        glPopMatrix();
#pragma endregion

#pragma region Ngan tu lanh 3
        glPushMatrix();
        glTranslatef(0.55, -0.3, 0.03);
        glPushMatrix();
        glTranslatef(16.0, -2.2, 13.8);
        glScalef(0.18, 0.15, 0.18);
        drawCube1(0.684, 0.664, 0.637, 0.684, 0.664, 0.637);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16.0, -1.8, 13.8);
        glScalef(0.18, 0.02, 0.18);
        drawCube1(0.2, 0.2, 0.2, 0.2, 0.2, 0.2);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, 0.6);
        glPushMatrix();
        glTranslatef(16.0, -2.2, 13.8);
        glScalef(0.18, 0.15, 0.18);
        drawCube1(0.684, 0.664, 0.637, 0.684, 0.664, 0.637);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16.0, -1.8, 13.8);
        glScalef(0.18, 0.02, 0.18);
        drawCube1(0.2, 0.2, 0.2, 0.2, 0.2, 0.2);
        glPopMatrix();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, 1.2);
        glPushMatrix();
        glTranslatef(16.0, -2.2, 13.8);
        glScalef(0.18, 0.15, 0.18);
        drawCube1(0.684, 0.664, 0.637, 0.684, 0.664, 0.637);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(16.0, -1.8, 13.8);
        glScalef(0.18, 0.02, 0.18);
        drawCube1(0.2, 0.2, 0.2, 0.2, 0.2, 0.2);
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();

#pragma endregion
    }
}

void lightOne()
{
    glPushMatrix();
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 5.0, 5.0, 8.0, 1.0 }; //5 5 10

    //glEnable( GL_LIGHT0);

    if (amb1 == true) { glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); }
    else { glLightfv(GL_LIGHT0, GL_AMBIENT, no_light); }

    if (diff1 == true) { glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); }
    else { glLightfv(GL_LIGHT0, GL_DIFFUSE, no_light); }

    if (spec1 == true) { glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); }
    else { glLightfv(GL_LIGHT0, GL_SPECULAR, no_light); }

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glPopMatrix();
}

void lightTwo()
{
    glPushMatrix();
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 0.9, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 5.0, 8.0, 1.0 };

    //glEnable( GL_LIGHT1);

    if (amb2 == true) { glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient); }
    else { glLightfv(GL_LIGHT1, GL_AMBIENT, no_light); }

    if (diff2 == true) { glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse); }
    else { glLightfv(GL_LIGHT1, GL_DIFFUSE, no_light); }

    if (spec2 == true) { glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular); }
    else { glLightfv(GL_LIGHT1, GL_SPECULAR, no_light); }

    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glPopMatrix();
}

void lampLight()
{
    glPushMatrix();
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.7, 1.5, 9.0, 1.0 };  //0.7, 4.5, 9.0

    //glEnable( GL_LIGHT2);

    if (amb3 == true) { glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient); }
    else { glLightfv(GL_LIGHT2, GL_AMBIENT, no_light); }

    if (diff3 == true) { glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse); }
    else { glLightfv(GL_LIGHT2, GL_DIFFUSE, no_light); }

    if (spec3 == true) { glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular); }
    else { glLightfv(GL_LIGHT2, GL_SPECULAR, no_light); }

    glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    GLfloat spot_direction[] = { 0.3, -1, -0.8 };
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 35.0);
    glPopMatrix();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, refX, refY, refZ, 0, 1, 0); //7,2,15, 0,0,0, 0,1,0

    glEnable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    lightOne();
    lightTwo();
    lampLight();
    room();

    glPushMatrix();
    glTranslatef(0, 0, -0.5);
    TuLanh::VeTuLanh();
    TuLanh::VeThanTuLanh();
    glPopMatrix();
    TuLanh::DoTrongTuLanh();

#pragma region Vẽ cửa
    // Cua ben trong
    glPushMatrix();
    glTranslatef(13, -5.04, 13);
    glRotatef(tuong_tac[CuaNK], 0, 1, 0);
    glTranslatef(-13, 5.04, -13);
    Cua::CuaNhaKho();
    glPopMatrix();

    // Cua ben ngoai
    glPushMatrix();
    glTranslatef(tuong_tac[CuaPK], 0, 0);
    Cua::CuaQuan();
    glPopMatrix();

    // Cua tu lanh
    glPushMatrix();
    glTranslatef(16.45, -3.6, 13.55);
    glTranslatef(0, 0, 2.32);
    glRotatef(tuong_tac[CuaTL], 0, 1, 0);
    glTranslatef(0, 0, -2.32);
    glTranslatef(-16.45, 3.6, -13.55);
    Cua::CuaTuLanh();
    glPopMatrix();
#pragma endregion

#pragma region Vẽ quạt trần
    // Ve quat tran 1
    glPushMatrix();
    glTranslatef(-3, 2.6, -23);

    QuatTran::quat_tran_s();    // Quạt trần static

    glPushMatrix();
    glTranslatef(9.75, 2, 30);
    glTranslatef(0.3, 0, 0);
    glRotatef(fanRotationAngle1, 0, 1, 0);
    glTranslatef(-0.3, 0, 0);
    glTranslatef(-9.75, -2, -30);
    QuatTran::quat_tran_q();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10, 2, 30);
    glRotatef(fanRotationAngle1, 0, 1, 0);
    glTranslatef(-10, -2, -30);
    QuatTran::bau_quat();
    glPopMatrix();

    glPopMatrix();
#pragma endregion
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glFlush();
    glutSwapBuffers();
}

void IdleFunc(void)
{
    // Cập nhật góc quay của cánh quạt
    if (_switch1) {
        fanRotationAngle1 += 1.0f;
        if (fanRotationAngle1 > 360.0f) {
            fanRotationAngle1 -= 360.0f;
        }
        glutPostRedisplay();
    }
    glutPostRedisplay();

    if (_switch2) {
        fanRotationAngle2 += 1.0f;
        if (fanRotationAngle2 > 360.0f) {
            fanRotationAngle2 -= 360.0f;
        }
        glutPostRedisplay();
    }
    glutPostRedisplay();

    if (_switch3) {
        fanRotationAngle3 += 1.0f;
        if (fanRotationAngle3 > 360.0f) {
            fanRotationAngle3 -= 360.0f;
        }
        glutPostRedisplay();
    }

    glutPostRedisplay();
}

void KeyboardFunc(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q': //back to default eye point and ref point
        eyeX = 7.0; eyeY = 2.0; eyeZ = 15.0;
        refX = 0.0; refY = 0.0; refZ = 0.0;
        break;
    case 'z': // Xoay camera sang trái (theo chiều kim đồng hồ) theo trục Y
        rotateCameraY(-5.0);
        break;
    case 'Z': // Xoay camera sang phải (ngược chiều kim đồng hồ) theo trục Y
        rotateCameraY(5.0);
        break;
    case 'x': // Xoay camera lên trên theo trục X
        rotateCameraX(5.0);
        break;
    case 'X': // Xoay camera xuống dưới theo trục X
        rotateCameraX(-5.0);
        break;
    case '1': //to turn on and off light one
        if (switchOne == false)
        {
            switchOne = true; amb1 = true; diff1 = true; spec1 = true;
            glEnable(GL_LIGHT0); break;
        }
        else if (switchOne == true)
        {
            switchOne = false; amb1 = false; diff1 = false; spec1 = false; glDisable(GL_LIGHT0); break;
        }
    case '2': //to turn on and off light two
        if (switchTwo == false)
        {
            switchTwo = true; amb2 = true; diff2 = false; spec2 = true;
            glEnable(GL_LIGHT1); break;
        }
        else if (switchTwo == true)
        {
            switchTwo = false; amb2 = false; diff2 = true; spec2 = false;
            glDisable(GL_LIGHT1); break;
        }
    case '3': //to turn on and off light one
        if (switchLamp == false)
        {
            switchLamp = true; amb3 = true; diff3 = true; spec3 = true;
            glEnable(GL_LIGHT2); break;
        }
        else if (switchLamp == true)
        {
            switchLamp = false; amb3 = false; diff3 = false; spec3 = false;
            glDisable(GL_LIGHT2); break;
        }
    case'4': //turn on/off ambient light 1
        if (amb1 == false) { amb1 = true; break; }
        else { amb1 = false; break; }
    case'5':
        if (diff1 == false) { diff1 = true; break; }
        else { diff1 = false; break; }
    case'6':
        if (spec1 == false) { spec1 = true; break; }
        else { spec1 = false; break; }
    case'7': //turn on/off ambient light 2
        if (amb2 == false) { amb2 = true; break; }
        else { amb2 = false; break; }
    case'8':
        if (diff2 == false) { diff2 = true; break; }
        else { diff2 = false; break; }
    case'9':
        if (spec2 == false) { spec2 = true; break; }
        else { spec2 = false; break; }
    case'e': //turn on/off ambient lamp light
        if (amb3 == false) { amb3 = true; break; }
        else { amb3 = false; break; }
    case'r':
        if (diff3 == false) { diff3 = true; break; }
        else { diff3 = false; break; }
    case't':
        if (spec3 == false) { spec3 = true; break; }
        else { spec3 = false; break; }
    case 27:    // Escape key
        exit(1);
    }

    glutPostRedisplay();
}

void SpecialFunc(int key, int x, int y)
{

    switch (key)
    {
    case GLUT_KEY_LEFT:
        moveCamera(-1.0, 0.0, 0.0);
        break;
    case GLUT_KEY_RIGHT:
        moveCamera(1.0, 0.0, 0.0);
        break;
    case GLUT_KEY_UP:
        moveCamera(0.0, 0.0, 1.0);
        break;
    case GLUT_KEY_DOWN:
        moveCamera(0.0, 0.0, -1.0);
        break;
    case GLUT_KEY_PAGE_UP:
        moveCamera(0.0, 1.0, 0.0);
        break;
    case GLUT_KEY_PAGE_DOWN:
        moveCamera(0.0, -1.0, 0.0);
        break;
    case GLUT_KEY_END:
        if (_switch1) {
            _switch1 = false;
        }
        else _switch1 = true;
    case GLUT_KEY_F1:
        tuong_tac[CuaNK] += 5;
        if (tuong_tac[CuaNK] > 100) {
            tuong_tac[CuaNK] -= 5;
        }
        break;
    case GLUT_KEY_F2:
        tuong_tac[CuaNK] -= 5;
        if (tuong_tac[CuaNK] < 0) {
            tuong_tac[CuaNK] += 5;
        }
        break;
    case GLUT_KEY_F3:
        tuong_tac[CuaPK] += 0.1;
        if (tuong_tac[CuaPK] >= 8) {
            tuong_tac[CuaPK] -= 0.1;
        }
        break;
    case GLUT_KEY_F4:
        tuong_tac[CuaPK] -= 0.1;
        if (tuong_tac[CuaPK] < 0) {
            tuong_tac[CuaPK] += 0.1;
        }
        break;
    case GLUT_KEY_F5:
        tuong_tac[CuaTL] += 5;
        if (tuong_tac[CuaTL] > 120) {
            tuong_tac[CuaTL] -= 5;
        }
        break;
    case GLUT_KEY_F6:
        tuong_tac[CuaTL] -= 5;
        if (tuong_tac[CuaTL] < 0) {
            tuong_tac[CuaTL] += 5;
        }
        break;
    }
}

void animate()
{
    if (redFlag == true)
    {
        theta += 2;
        z -= 0.02; //0.016667;
        if (theta >= 196 && theta <= 210)
        {
            y = 1.44;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.42;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.4;
        }
        else if (theta >= 164 && theta <= 178)
        {
            y = 1.42;
        }

        if (theta == 210)
        {
            redFlag = false;
        }
    }
    else if (redFlag == false)
    {
        theta -= 2;
        z += 0.02;//0.016667;

        if (theta >= 196 && theta <= 210)
        {
            y = 1.44;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.42;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.4;
        }
        else if (theta >= 164 && theta <= 178)
        {
            y = 1.42;
        }

        if (theta == 150)
        {
            redFlag = true;
        }
    }

    glutPostRedisplay();

}

void fullScreen(int w, int h)
{
    //Prevent a divide by zero, when window is too short;you cant make a window of zero width.
    if (h == 0)
        h = 1;
    float ratio = (GLfloat)w / (GLfloat)h;         //Calculate aspect ratio of the window

    //Set the perspective coordinate system
    glMatrixMode(GL_PROJECTION);                   //Use the Projection Matrix
    glLoadIdentity();                              //Reset Matrix

    glViewport(0, 0, w, h);                        //Set the viewport to be the entire window
    gluPerspective(60, ratio, 1, 500);             //Set the correct perspective.
    //glFrustum(-2.5,2.5,-2.5,2.5, ratio, 200);
    glMatrixMode(GL_MODELVIEW);                    //Get Back to the Modelview
}

void menu(int option)
{
    if (option == Quit) {
        exit(EXIT_SUCCESS);
    }

    angle = option;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    std::cout << "|-------------------- Huong Dan Su Dung --------------------|" << std::endl;
    std::cout << "|    Phim    |                  Chuc nang                   |" << std::endl;
    std::cout << "|-----------------------------------------------------------|" << std::endl;
    std::cout << "| F1         |  Mo cua ben trong                            |" << std::endl;
    std::cout << "| F2         |  Dong cua ben trong                          |" << std::endl;
    std::cout << "| F3         |  Mo cua chinh                                |" << std::endl;
    std::cout << "| F4         |  Dong cua chinh                              |" << std::endl;
    std::cout << "| F5         |  Mo cua tu lanh                              |" << std::endl;
    std::cout << "| F6         |  Dong cua tu lanh                            |" << std::endl;
    std::cout << "| Key Up     |  Di chuyen camera vao trong                  |" << std::endl;
    std::cout << "| Key Down   |  Di chuyen camera ra ngoai                   |" << std::endl;
    std::cout << "| Key Right  |  Di chuyen camera sang phai                  |" << std::endl;
    std::cout << "| Key Down   |  Di chuyen camera sang trai                  |" << std::endl;
    std::cout << "| Page Up    |  Di chuyen camera len tren                   |" << std::endl;
    std::cout << "| Page Down  |  Di chuyen camera xuong duoi                 |" << std::endl;
    std::cout << "| z          |  Quay camera sang trai                       |" << std::endl;
    std::cout << "| Z          |  Quay camera sang phai                       |" << std::endl;
    std::cout << "| x          |  Quay camera xuong duoi                      |" << std::endl;
    std::cout << "| X          |  Quay camera len tren                        |" << std::endl;
    std::cout << "| q          |  Di chyen vao ben trong nha                  |" << std::endl;
    std::cout << "| 1 - 9      |  Dieu khien anh sang                         |" << std::endl;
    std::cout << "| End        |  Tat / Mo quat tran                          |" << std::endl;
    std::cout << "|-----------------------------------------------------------|" << std::endl;
    std::cout << std::endl << "                                      Design by Richard Jacob" << std::endl;

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowHeight, windowWidth);
    glutCreateWindow("Milk Tea Shop");

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glutReshapeFunc(fullScreen);
    glutDisplayFunc(display);
    glutIdleFunc(IdleFunc);
    glutKeyboardFunc(KeyboardFunc);
    glutSpecialFunc(SpecialFunc);

    // tạo menu
    glutCreateMenu(menu);
    glutAddMenuEntry("Cua nha kho", CuaNK);
    glutAddMenuEntry("quit", Quit);

    // kích hoạt menu bằng nhấn nút giữa chuột
    glutAttachMenu(GLUT_MIDDLE_BUTTON);
    //glutIdleFunc(animate);
    glutMainLoop();

    return 0;
}
