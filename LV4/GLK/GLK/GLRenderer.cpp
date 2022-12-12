#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#pragma comment(lib, "GL\\glut32.lib")
#pragma comment(lib,"GL\\GLU32.lib")

CGLRenderer::CGLRenderer(void)
{
	this->angle_v = 0;
	this->angle_h = 0;

	// Camera angles?
	this->angle_cx = 0;
	this->angle_cy = 0;
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	
	if (nPixelFormat == 0) return false; 

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);
  	
	if (!bResult) return false; 

   	m_hrc = wglCreateContext(pDC->m_hDC); 

	if (!m_hrc) return false; 

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	// Postavljamo kontekst po kome ce OpenGL da crta:
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	// Postavljamo boju bafera (pozadina?)
	glClearColor(0.5, 0.75, 1, 0.0);

	// Sa glEnable omogucavamo nesto, ovde konkretno, dubinu?
	glEnable(GL_DEPTH_TEST);

	//---------------------------------
	// Vracamo kontekst, ponovo je default onaj koji je bio prilikom inicijalizacije projekta:
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(angle_h, .0, 0, 1.0);
	glRotatef(angle_v, 0, 1.0, .0);

	DrawAxis(3.0);
	DrawGrid(1.0, 1.0, 10, 10);

	glPushMatrix();
	//Posto je prvobitno sve nacrtano u vecoj razmeri, samo je skalirano na polovinu
	glScalef(0.3, 0.3, 0.3);
	
	// Test Code:

	glTranslatef(0.0, 0.0, 0.0);
	DrawPot();

	glColor3f(0.00000, 0.90196, 0.00000);

	glTranslatef(0.0, 0.6, 0.0);
	glTranslatef(0.0, 0.25, 0.0);
	DrawCylinder(0.5, 0.175, 0.175, 18);

	glColor3f(0.00000, 0.30196, 0.00000);
	glTranslatef(0.0, 0.25 + 0.1, 0.0);
	DrawSphere(0.1, 15, 15);

	// Centar najnize sfere:
	glPushMatrix();

	DrawLeftArm();
	DrawRightArm();
	DrawMiddleArm();

	// End test code;

	//glTranslatef(0.5, 0.5, 0.0);
	//DrawCylinder(1.0, 0.3, 0.3, 18);
	//DrawPot();
	//DrawFigure();
	glPopMatrix();

	glFlush();
	//---------------------------------
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (double)w / (double)h, 1, 100);
	gluLookAt(3.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

//void CGLRenderer::DrawPrism(float radius, float height, int sides)
//{
//	float t_height = height / 2.0;
//
//	// Sacuvaj transformacionu matricu;
//	glPushMatrix();
//	
//	// Transformacija?
//	glRotatef(45, 0, 1, 0);
//
//	float step = (2 * C_PI) / sides;
//
//	// Baza 1:
//	glBegin(GL_POLYGON);
//	for (int i = 0; i < sides; i++) {
//		glVertex3f(get_x(radius, i * step), t_height, get_y(radius, i * step));
//	}
//	glEnd();
//
//	// Baza 2:
//	glBegin(GL_POLYGON);
//	for (int i = 0; i < sides; i++) {
//		glVertex3f(get_x(radius, i * step), -t_height, get_y(radius, i * step));
//	}
//	glEnd();
//
//	// Omotac:
//	glBegin(GL_QUAD_STRIP);
//	for (int i = 0; i < sides; i++) {
//		glVertex3f(get_x(radius, i * step), -t_height, get_y(radius, i * step));
//		glVertex3f(get_x(radius, (i + 1) * step), -t_height, get_y(radius, (i + 1) * step));
//		glVertex3f(get_x(radius, i * step), t_height, get_y(radius, i * step));
//		glVertex3f(get_x(radius, (i + 1) * step), t_height, get_y(radius, (i + 1) * step));
//	}
//	glEnd();
//
//	// Vrati transformacionu matricu;
//	glPopMatrix();
//}

void CGLRenderer::DrawSphere(double r, int nSegAlpha, int nSegBeta)
{
	for (double i = 0; i <= nSegAlpha; i++) {

		double alpha = C_PI * (-0.5 + (double)(i - 1) / nSegAlpha);
		
		// Svi se nalaze na istoj udaljenosti od centra sfere?
		double z0 = sin(alpha);
		double zr0 = cos(alpha);

		double beta = C_PI * (-0.5 + (double)i / nSegAlpha);
		double z1 = sin(beta);
		double zr1 = cos(beta);

		glBegin(GL_QUAD_STRIP);
		for (double j = 0; j <= nSegBeta; j++) {
			
			double lng = 2 * C_PI * (double)(j - 1) / nSegBeta;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(r * x * zr0, r * y * zr0, r * z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(r * x * zr1, r * y * zr1, r * z1);
		}
		glEnd();

	}
}

void CGLRenderer::DrawCylinder(double h, double r1, double r2, int nSeg)
{
	float step = (2 * C_PI) / nSeg;
	float t_height = h / 2.0;

	// Top base:
	glBegin(GL_POLYGON);
	for (double i = 0; i < nSeg; i++) {
		glVertex3f(get_x(r1, i * step), t_height, get_y(r1, i * step));
	}
	glEnd();

	// Bottom base:
	glBegin(GL_POLYGON);
	for (double i = 0; i < nSeg; i++) {
		glVertex3f(get_x(r2, i * step), -t_height, get_y(r2, i * step));
	}
	glEnd();

	// Omotac:
	glBegin(GL_QUAD_STRIP);
	for (long i = 0; i < nSeg; i++) {
		glVertex3f(get_x(r2, i * step), -t_height, get_y(r2, i * step));
		glVertex3f(get_x(r2, (i + 1) * step), -t_height, get_y(r2, (i + 1) * step));
		glVertex3f(get_x(r1, i * step), t_height, get_y(r1, i * step));
		glVertex3f(get_x(r1, (i + 1) * step), t_height, get_y(r1, (i + 1) * step));
	}
	glEnd();

}

void CGLRenderer::DrawCone(double h, double r, int nSeg = 18)
{
	float step = (C_PI * 2) / nSeg;
	float t_height = h / 2.0;		// Pozicioniramo se na polovinu figure:

	// Base:
	glBegin(GL_POLYGON);
	for (float i = 0; i < nSeg; i++) {
		glVertex3f(get_x(r, i * step), -t_height, get_y(r, i * step));
	}
	glEnd();

	// Omotac:
	glBegin(GL_TRIANGLE_STRIP);
	for (float i = 0; i < nSeg; i++) {
		glVertex3f(get_x(r, i * step), -t_height, get_y(r, i * step));
		glVertex3f(0, t_height, 0);
		glVertex3f(get_x(r, (i + 1) * step), -t_height, get_y(r, (i + 1) * step));
	}
	glEnd();
}

void CGLRenderer::DrawLeftArm()
{
	glPushMatrix();
	glColor3f(0.00000, 0.90196, 0.00000);

	glRotatef(45, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	DrawCylinder(0.5, 0.175, 0.175, 18);

	// First joint
	glTranslatef(0.0, 0.25 + 0.1, 0.0);
	glPushMatrix();
	//DrawSphere(0.1, 15, 15);

	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	DrawCone(0.5, 0.175);

	// Seccond joint:
	glColor3f(0.00000, 0.30196, 0.00000);
	glTranslatef(0.0, 0.25 + 0.1, 0.0);
	DrawSphere(0.1, 15, 15);

	glPopMatrix();
	DrawSphere(0.1, 15, 15);

	glPopMatrix();
	glPopMatrix();

}

void CGLRenderer::DrawRightArm()
{
	glPushMatrix();
	glColor3f(0.00000, 0.90196, 0.00000);

	glRotatef(-45, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	DrawCylinder(0.5, 0.175, 0.175, 18);

	// First sphere:
	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	glPushMatrix();

	glTranslatef(0.0, 0.25 + 0.1, 0.0);
	DrawCone(0.5, 0.175, 18);

	// Soccond sphere:
	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	glPushMatrix();

	glTranslatef(0.0, 0.25 + 0.1, 0.0);
	DrawCone(0.5, 0.175, 18);

	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	glColor3f(0.00000, 0.30196, 0.00000);
	DrawSphere(0.1, 15, 15);

	// Seccond sphere:
	glPopMatrix();
	glColor3f(0.00000, 0.30196, 0.00000);
	DrawSphere(0.1, 15, 15);

	// Fists sphere:
	glPopMatrix();
	glColor3f(0.00000, 0.30196, 0.00000);
	DrawSphere(0.1, 15, 15);

	glPopMatrix();

}

void CGLRenderer::DrawMiddleArm()
{
	glPushMatrix();
	glColor3f(0.00000, 0.90196, 0.00000);

	// najdonji cilindar:
	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	DrawCylinder(0.5, 0.175, 0.175, 18);

	// First joint:
	glTranslatef(0.0, 0.25 + 0.1, 0.0);
	glPushMatrix();

	// Left-middle arm:
	glRotatef(45 + arm_angle, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.1 + 0.25, 0.0);

	glPushMatrix();
	glColor3f(0.90196, 0.90196, 0.00000);
	DrawCylinder(0.5, 0.175, 0.175, 18);

	//ff
	glTranslatef(0.0, 0.25 + 0.1, 0.0);
	glPushMatrix();

	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	glColor3f(0.00000, 0.90196, 0.00000);
	DrawCone(0.5, 0.175, 18);

	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	glColor3f(0.00000, 0.30196, 0.00000);
	DrawSphere(0.1, 15, 15);

	glPopMatrix();
	glColor3f(0.00000, 0.30196, 0.00000);
	DrawSphere(0.1, 15, 15);

	glPopMatrix();
	glPopMatrix();


	// middle=right arm
	glRotatef(-45, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	glColor3f(0.00000, 0.90196, 0.00000);
	DrawCylinder(0.5, 0.175, 0.175, 18);

	// first:
	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	glPushMatrix();

	glTranslatef(0.0, 0.25 + 0.1, 0.0);
	DrawCylinder(0.5, 0.175, 0.175, 18);

	glTranslatef(0.0, 0.1 + 0.25, 0.0);
	glColor3f(0.00000, 0.30196, 0.00000);
	DrawSphere(0.1, 15, 15);

	// ff:
	glPopMatrix();
	glColor3f(0.00000, 0.30196, 0.00000);
	DrawSphere(0.1, 15, 15);


	// first joint:
	glPopMatrix();

	glColor3f(0.00000, 0.30196, 0.00000);
	DrawSphere(0.1, 15, 15);

	glPopMatrix();
}

void CGLRenderer::DrawAxis(double width = 3.0)
{
	glLineWidth(1.0);

	glBegin(GL_LINES);
	//================================
	
	// X axis:
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(width, 0.0, 0.0);

	// Green axis:
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, width, 0.0);

	// Blue axis:
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, width);

	//================================
	glEnd();
}

void CGLRenderer::DrawGrid(double width, double height, int nSegW, int nSegH)
{
	glLineWidth(1.0);
	glColor3f(.9, .9, .9);
	
	double width_step = width / nSegW * 2;
	double height_step = height / nSegH * 2;

	// Sirina: (x)
	glBegin(GL_LINES);
	for (float i = -1; i <= 1; i+= width_step) {
		glVertex3f(i, 0.0, -1.0);
		glVertex3f(i, 0.0, 1.0);
	}
	glEnd();

	// Dubina: (z)
	glBegin(GL_LINES);
	for (float i = -1.0; i < 1.0; i += height_step) {
		glVertex3f(-1.0, 0.0, i);
		glVertex3f(1.0, 0.0, i);
	}

	glEnd();
}

void CGLRenderer::SetAngles(float angle_h, float angle_v)
{
	this->angle_v = angle_h;
	this->angle_h = angle_v;
}

void CGLRenderer::DrawPot() {
	int sides = 8;
	float t_height = 0.4 / 2.0;

	glPushMatrix();

	// Donji deo saksije:
	glTranslatef(0.0, 0.2, 0.0);
	glColor3f(0.52549, 0.34902, 0.17647);
	DrawCylinder(0.4, 0.4, 0.3, sides);

	// Gornji deo saksije:
	glTranslatef(0.0, 0.3, 0.0);
	DrawCylinder(0.2, 0.5, 0.5, sides);

	glPopMatrix();

}

//void CGLRenderer::DrawPot()
//{
//	int sides = 8;
//	float t_height = 0.4 / 2.0;
//
//	// Cuvamo transformacionu matricu:
//	glPushMatrix();
//
//	glTranslatef(0, 0.2, 0);
//
//	float step = (2 * C_PI) / sides;
//
//	glColor3f(0.62, 0.3, 0.17);
//
//	// Baza 1:
//	glBegin(GL_POLYGON);
//	for (int i = 0; i < sides; i++) {
//		glVertex3f(get_x(0.45, i * step), t_height, get_y(0.45, i * step));
//	}
//	glEnd();
//
//	// Baza 2:
//	glBegin(GL_POLYGON);
//	for (int i = 0; i < sides; i++) {
//		glVertex3f(get_x(0.4, i * step), -t_height, get_y(0.4, i * step));
//	}
//	glEnd();
//
//	// Omotac:
//	glBegin(GL_POLYGON);
//	for (int i = 0; i < sides; i++) {
//		glVertex3f(get_x(0.4, i * step), -t_height, get_y(0.4, i * step));
//		glVertex3f(get_x(0.4, (i + 1) * step), -t_height, get_y(0.4, (i + 1) * step));
//		glVertex3f(get_x(0.45, i * step), t_height, get_y(0.45, i * step));
//		glVertex3f(get_x(0.45, (i + 1) * step), t_height, get_y(0.45, (i + 1) * step));
//	}
//	glEnd();
//
//	// Translorano je vec na 0.2, dodamo jos 0.2 da postavimo na 
//	// pocetak donjeg dela i 0.1 da centriramo gornji deo koji je debljine 0.2
//	// +0.01 cisto da se lepo vidi depth;
//
//	glTranslatef(0, 0.3, 0);
//	glColor4f(0.65, 0.3, 0.17, 0.5);
//	this->DrawCylinder(0.5, 0.2, 0.2, 4);
//
//	glPopMatrix();
//
//}

void CGLRenderer::DrawFigure()
{
	// Bazna kupa:
	glColor3f(0, 1, 0);
	glPushMatrix();

	glTranslatef(0, 0.8, 0);
	DrawCone(0.2, 0.4);

	glTranslatef(0, 0.3, 0);
	DrawSphere(0.1, 15.0, 15.0);

	MiddleBranch();
	RightBranch();
	LeftBranch();

	glPopMatrix();

}

float CGLRenderer::get_x(float radius, float angle)
{
	return radius * cos(angle);
}

float CGLRenderer::get_y(float radius, float angle)
{
	return radius * sin(angle);
}

void CGLRenderer::MiddleBranch()
{

	glPushMatrix();

	//Custom rotation
	glRotatef(angle_cx, 1, 0, 0);
	glRotatef(angle_cy, 0, 1, 0);

	glTranslatef(0, 0.1, 0);


	glColor3f(1, 1, 0);

	glTranslatef(0, 0.2, 0);

	DrawCone(0.1, 0.4);

	glTranslatef(0, 0.2, 0);
	glTranslatef(0, 0.1, 0);
	glColor3f(0, 1, 0);

	DrawSphere(0.1, 15.0, 15.0);

	glPushMatrix();
	glRotatef(-45, 1, 0, 0);
	glTranslatef(0, 0.3, 0);
	DrawCylinder(0.2, 0.4, 0.4, 4);
	glTranslatef(0, 0.2, 0);
	glTranslatef(0, 0.1, 0);
	DrawSphere(0.1, 15.0, 15.0);
	glPopMatrix();

	glPushMatrix();
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, 0.3, 0);
	DrawCone(0.2, 0.4);
	glTranslatef(0, 0.3, 0);
	DrawSphere(0.1, 15.0, 15.0);
	glPopMatrix();

	glPopMatrix();
}

void CGLRenderer::RightBranch()
{
	glPushMatrix();
	glColor3f(0, 1, 0);
	glRotatef(-45, 1, 0, 0);
	glTranslatef(0, 0.3, 0);
	DrawCylinder(0.1, 0.4, 0.4, 18);
	glTranslatef(0, 0.3, 0);
	DrawSphere(0.1, 15.0, 15.0);
	glPopMatrix();
}

void CGLRenderer::LeftBranch()
{


	glPushMatrix();
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, 0.3, 0);
	DrawCylinder(0.1, 0.4, 0.4, 4);
	glTranslatef(0, 0.3, 0);
	DrawSphere(0.1, 15.0, 15.0);


	glPushMatrix();
	glRotatef(-45, 1, 0, 0);

	glTranslatef(0, 0.3, 0);
	DrawCylinder(0.1, 0.4, 0.4, 4);
	glTranslatef(0, 0.3, 0);


	DrawSphere(0.1, 15.0, 15.0);

	glTranslatef(0, 0.3, 0);
	DrawCylinder(0.2, 0.4, 0.4, 4);
	glTranslatef(0, 0.3, 0);

	DrawSphere(0.1, 15.0, 15.0);
	glPopMatrix();

	glPushMatrix();


	glTranslatef(0, 0.3, 0);
	DrawCone(0.1, 0.4);
	glTranslatef(0, 0.3, 0);


	DrawSphere(0.1, 15.0, 15.0);

	glTranslatef(0, 0.3, 0);
	DrawCone(0.2, 0.4);
	glTranslatef(0, 0.3, 0);


	DrawSphere(0.1, 15.0, 15.0);
	glPopMatrix();

	glPushMatrix();
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, 0.3, 0);

	DrawCylinder(0.1, 0.4, 0.4, 4);
	glTranslatef(0, 0.3, 0);

	DrawSphere(0.1, 15.0, 15.0);
	glPopMatrix();
	glPopMatrix();
}