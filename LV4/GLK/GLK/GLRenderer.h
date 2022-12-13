#pragma once

class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	// Mine~:
	void DrawSphere(double r, int nSegAlpha, int nSegBeta);
	void DrawCylinder(double h, double r1, double r2, int nSeg);
	void DrawCone(double h, double r, int nSeg);

	void DrawLeftArm();
	void DrawRightArm();
	void DrawMiddleArm();

	//void DrawPrism(float radius, float height = 0.3, int sides = 4);

	void DrawAxis(double width);
	void DrawGrid(double width, double height, int nSegW, int nSegH);

	CPoint GetAngle() {
		return CPoint(angle_v, angle_h);
	}

	void SetAngles(float angle_h, float angle_v);

	inline void AngleAdd() {
		arm_angle += 10;
	}

	inline void AngleSub() {
		arm_angle -= 10;
	}


protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	void DrawPot();
	void DrawFigure();

	float get_x(float radius, float angle);
	float get_y(float radius, float angle);

	float angle_v, angle_h;
	float angle_cx, angle_cy;
	float arm_angle = 0.0;

	const double C_PI = 3.14159265358979323846;
	const double C_PI_RAD = 3.14159265358979323846 / 180.0;

	void MiddleBranch();
	void RightBranch();
	void LeftBranch();

};
