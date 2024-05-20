/*
 *		This Code Was Created By Jeff Molofee 2000
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing This Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */
#include <queue>
#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "Grid.h"
#include "GLFW/glfw3.h"
#include "wingdi.h"
#include <GL/glut.h>
#include <crtdbg.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include <list>
#define MAX_PARTICLES 5000
using namespace cv;
using namespace std;
using namespace glm;



HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application



float slowdown = 1.0;
float velocity = 0.0;
float zoom = -20.0;
float pan = 0.0;
float tilt = 0.0;

int loop;
int fall;

//floor colors
float r = 0.0;
float g = 1.0;
float b = 0.0;
float ground_points[21][21][3];
float ground_colors[21][21][4];
float accum = -10.0;

typedef struct {
	Vec3f point;
	int neighbors;
}Vertice;


typedef struct {
	int** weights;
}Edge;

typedef struct {
	Vertice** vertice;
}Graph;


struct Compare {
	float * dist;
	int width;
	bool operator() (const Vec2d p1, const Vec2d p2) const {
		int p1i = p1[0];
		int p1j = p1[1];
		int p2i = p2[0];
		int p2j = p2[1];

		return dist[(p1i * width) + p1j]  < dist[(p2i * width) + p2j]; // the operator returns true if a is higher priority than b
	}
};
typedef struct {
	// Life
	bool alive;	// is the particle alive?
	float life;	// particle lifespan
	float fade; // decay
	// color
	float red;
	float green;
	float blue;
	// Position/direction
	float xpos;
	float ypos;
	float zpos;
	// Velocity/Direction, only goes down in y dir
	float vel;
	// Gravity
	float gravity;
}particles;

// Paticle System
particles par_sys[MAX_PARTICLES];

_CrtMemState sOld;
_CrtMemState sNew;
_CrtMemState sDiff;
int Width = 768;
GLfloat	rtri;				// Angle For The Triangle ( NEW )
GLfloat	rquad;				// Angle For The Quad ( NEW )
int Height = 1024;
bool	keys[256];			// Array Used For The Keyboard Routine
bool	active = TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen = TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
Mat     image;
Grid    grid; 
Grid    BackUpGrid; 
const float piover180 = 0.0174532925f;
GLfloat moveX =0;
GLfloat moveY =0;
GLfloat moveZ =0;
GLfloat player_pov = 0 ;
GLfloat player_head = 0;
GLfloat rotationY = 0;
GLfloat rotationX = 0;
GLfloat rotationZ = 0;
GLfloat headup = 360.0f;
GLfloat lookupdown = 0.0f;
GLfloat fov = 45.0f;
int R = 1;
float MouseXPos, MouseYPos , BackUpY , BackUpX;
float RectangleHeight =20;
float RectangleWidth =20;
bool pickingMode = false;
bool DoneDrawingUnique = false;
bool firstTime = true;
bool showLines = true;
float fogScale = 0.0f;
bool HighResolution = false;
bool fog_rain = false;
bool rain_fog = false;
int fine_factor = 1;
std::queue<Vec3f> ShortestPath;
bool CalcPath = false;
bool low_res = true;


LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height == 0)// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}
		
	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 10000.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix


}
void initParticles(int i) {
	par_sys[i].alive = true;
	par_sys[i].life = 3.0;
	par_sys[i].fade = float(rand() % 100) / 10000.0f + 0.003f;

	par_sys[i].xpos = (rand() % image.rows);// - 10;
	par_sys[i].ypos = (rand() % image.cols);// -10;
	par_sys[i].zpos = 200;

	par_sys[i].red = 0.5;
	par_sys[i].green = 0.5;
	par_sys[i].blue = 1.0;

	par_sys[i].vel = velocity;
	par_sys[i].gravity = -0.8;//-0.8;

}
float distance3d(Vec3f v1 , Vec3f v2) {
	int v1x = v1[0];
	int v1y = v1[1];
	int v1z = v1[2];
	int v2x = v2[0];
	int v2y = v2[1];
	int v2z = v2[2];
	return sqrt(pow((v1x - v2x), 2) + pow((v1y - v2y), 2) + pow((v1z - v2z), 2));

}
int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{


	//Debug printing
	/*
	AllocConsole();
	FILE* dummy;
	freopen_s(&dummy,"CONOUT$", "w", stdout);*/
	std::string image_path = "C:/Users/noams/Downloads/another_photo.png";
	//std::string image_path = samples::findFile("starry_night.jpg");
	image = imread(image_path, IMREAD_GRAYSCALE);
	grid = Grid(RectangleHeight, RectangleWidth, image, image.cols, image.rows);
	// Initialize particles
	for (loop = 0; loop < MAX_PARTICLES; loop++) {
		initParticles(loop);
	}

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	return TRUE;										// Initialization Went OK
}

int translate2Dto1D(Vec2d kodkod, int width ) {
	int i = kodkod[0];
	int j = kodkod[1];
	return (i * width) + j;

}
Vec2d translate1Dto2D(int i ,  int width) {
	return Vec2d(int(i/width),i % width);

}

void initFog() {
	// Set the fog color to white
	GLfloat fogColor[4] = { 0.8f, 0.8f, 0.8f, 0.8f };
	// Enable fog
	//glEnable(GL_FOG);

	// Set the fog color
	glFogfv(GL_FOG_COLOR, fogColor);

	// Set the fog density to 0.35, which is a good value for exponential fog
	glFogf(GL_FOG_DENSITY, 0.35 + fogScale);

	// Set the fog equation to GL_EXP, which gives an exponential decrease in density
	glFogi(GL_FOG_MODE, GL_LINEAR);

	// Set the fog range to start at 0 and end at 1
	glFogf(GL_FOG_START, 0.0);
	glFogf(GL_FOG_END, 1000.0);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Set the depth function to GL_LESS, so that objects closer to the camera are drawn in front of objects that are farther away
	//glDepthFunc(GL_LESS);

	// Draw your scene here


}
void drawRain() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(0.5, 0.5, 1.0);
			glBegin(GL_LINES);
			glVertex3f(x, y, z);
			glVertex3f(x, y, z+10);
			glEnd();

			// Update values
			//Move
			// Adjust slowdown for speed!
			par_sys[loop].zpos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].zpos <= 0.0) {
				par_sys[loop].life = -1.0;
			}
			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}

void DrawLines(Triangle* tr) {

	GLfloat black[3];
	black[0] = 0.0f;
	black[1] = 0.0f;
	black[2] = 0.0f;
	GLfloat v1[3];
	v1[0] = tr->getVertice(0).getV1();
	v1[1] = tr->getVertice(0).getV2();
	v1[2] = tr->getVertice(0).getV3();
	GLfloat v2[3];
	v2[0] = tr->getVertice(1).getV1();
	v2[1] = tr->getVertice(1).getV2();
	v2[2] = tr->getVertice(1).getV3();
	GLfloat v3[3];
	v3[0] = tr->getVertice(2).getV1();
	v3[1] = tr->getVertice(2).getV2();
	v3[2] = tr->getVertice(2).getV3();
	glColor3fv(black);
	glVertex3fv(v1);
	glVertex3fv(v2);

	glColor3fv(black);
	glVertex3fv(v1);
	glVertex3fv(v3);


	glColor3fv(black);
	glVertex3fv(v2);
	glVertex3fv(v3);

}
void DrawTriangle(Triangle * tr , bool pickingMode) {
	PointColor p1 = tr->getVertice(0);
	PointColor p2 = tr->getVertice(1);
	PointColor p3 = tr->getVertice(2);
	
	//std::cout << p1.getV1() << ":" << p1.getV2() << ":" << p1.getV3() << ":\n";

	//std::cout << p2.getV1() << ":" << p2.getV2() << ":" << p2.getV3() << ":\n";

	//std::cout << p3.getV1() << ":" << p3.getV2() << ":" << p3.getV3() << ":\n";
	uchar p1Color = p1.getColor();
	uchar p2Color = p2.getColor();
	uchar p3Color = p3.getColor();
	if (pickingMode) {

		glColor3ub(p1.getColorRGB().val[0],p1.getColorRGB().val[1],p1.getColorRGB().val[2]);          // Red
		glVertex3f(p1.getV1(), p1.getV2(), p1.getV3());          // Top Of Triangle (Left)
		glColor3ub(p2.getColorRGB().val[0], p2.getColorRGB().val[1], p2.getColorRGB().val[2]);         // Red
		glVertex3f(p2.getV1(), p2.getV2(), p2.getV3());   // Left Of Triangle (Left)
		glColor3ub(p3.getColorRGB().val[0], p3.getColorRGB().val[1], p3.getColorRGB().val[2]);            // Red
		glVertex3f(p3.getV1(), p3.getV2(), p3.getV3());   // Lef
	}
	else {
		if (!tr->getMark()) {
			glColor3ub(p1Color,0 ,255);          // Red
			glVertex3f(p1.getV1(), p1.getV2(), p1.getV3());          // Top Of Triangle (Left)
			glColor3ub(p2Color, 0, 255);          // Red
			glVertex3f(p2.getV1(), p2.getV2(), p2.getV3());   // Left Of Triangle (Left)
			glColor3ub(p3Color, 0, 255);            // Red
			glVertex3f(p3.getV1(), p3.getV2(), p3.getV3());   // Lef
		}
		else
		{
			glColor3f(0.0f, 255.0f, 0.0f);          // Green
			glVertex3f(p1.getV1(), p1.getV2(), p1.getV3());          // Top Of Triangle (Left)
			glColor3f(0.0f, 255.0f, 0.0f);          // Green
			glVertex3f(p2.getV1(), p2.getV2(), p2.getV3());   // Left Of Triangle (Left)
			glColor3f(0.0f, 255.0f, 0.0f);          // Green
			glVertex3f(p3.getV1(), p3.getV2(), p3.getV3());   // Lef
		}

	}
	
}

void RenderScenes(Triangle** triangles , bool pickingMode) {
	
	//int k = 0;
	//while (triangles[k] != NULL) {
	//	glBegin(GL_TRIANGLES);
 	//	Triangle* temp = triangles[k]; 
	//	DrawTriangle(temp,pickingMode);
	//	glEnd();
	//	// DrawLine
	//	glBegin(GL_LINES);
	//	DrawLines(triangles[k]);
	//	glEnd();
  	//	k++;
 	//}


	int k = 0;
	glBegin(GL_TRIANGLES);
	while (triangles[k] != NULL) {
		Triangle* temp = triangles[k];
		DrawTriangle(temp, pickingMode);
		k++;
	}
	glEnd();
	glBegin(GL_LINES);
	k = 0; 
	if(!pickingMode && showLines){
	   while (triangles[k] != NULL) {
		
		   // DrawLine
		   Triangle* temp = triangles[k];
		   DrawLines(triangles[k]);
		
		   k++;
	   }
	}
	glEnd();
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	if (!pickingMode && CalcPath) {
		Vec3f prev1 = ShortestPath.front();
		GLfloat prev[3];
		prev[0] = prev1[0];
		prev[1] = prev1[1];
		prev[2] = prev1[2];
		ShortestPath.pop(); 
		GLubyte red[3];
		red[0] = 255;
		red[1] = 0;
		red[2] = 0;
		while (!ShortestPath.empty()) {
			Vec3f curr1 = ShortestPath.front(); 
			GLfloat curr[3];
			curr[0] = curr1[0];
			curr[1] = curr1[1];
			curr[2] = curr1[2];
			ShortestPath.pop();
			glColor3ubv(red);
			glVertex3fv(prev);
			glVertex3fv(curr);
			prev[0] = curr[0];
			prev[1] = curr[1];
			prev[2] = curr[2];
			
		}
		CalcPath = false;
	}
	
	glEnd();
	glLineWidth(1.0f);
	
	
}

void Selection() {
	GLuint	buffer[512];										// Set Up A Selection Buffer
	GLint	hits;												// The Number Of Objects That We Selected
	//float	buffer1[512];
	// The Size Of The Viewport. [0] Is <x>, [1] Is <y>, [2] Is <length>, [3] Is <width>
	GLint	viewport[4];

	// This Sets The Array <viewport> To The Size And Location Of The Screen Relative To The Window
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, buffer);								// Tell OpenGL To Use Our Array For Selection
	//glFeedbackBuffer(512, buffer1);
	// Puts OpenGL In Selection Mode. Nothing Will Be Drawn.  Object ID's and Extents Are Stored In The Buffer.
	(void)glRenderMode(GL_SELECT);

	glInitNames();												// Initializes The Name Stack
	glPushName(0);												// Push 0 (At Least One Entry) Onto The Stack

	glMatrixMode(GL_PROJECTION);								// Selects The Projection Matrix
	glPushMatrix();												// Push The Projection Matrix
	glLoadIdentity();											// Resets The Matrix

	// This Creates A Matrix That Will Zoom Up To A Small Portion Of The Screen, Where The Mouse Is.
	gluPickMatrix((GLdouble)MouseXPos, (GLdouble)(viewport[3] - MouseYPos), 1.0f, 1.0f, viewport);

	// Apply The Perspective Matrix
	gluPerspective(45.0f, (GLfloat)(viewport[2] - viewport[0]) / (GLfloat)(viewport[3] - viewport[1]), 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);	// Select The Modelview Matrix												// Render The Targets To The Selection Buffer
	RenderScenes(grid.getTriangle(),false);
	glMatrixMode(GL_PROJECTION);								// Select The Projection Matrix
	glPopMatrix();												// Pop The Projection Matrix
	glMatrixMode(GL_MODELVIEW);									// Select The Modelview Matrix
	hits = glRenderMode(GL_RENDER);								// Switch To Render Mode, Find Out How Many
	int choose; 
	int depth; 
	if (hits > 0)												// If There Were More Than 0 Hits
	{	     								
		choose = buffer[3];									// Make Our Selection The First Object
		depth = buffer[1];
		GLuint dep = (GLuint)depth;
		int a = 1;

	}
	else {
		return;
	}

}

uchar CalcHeightInverse(float z) {


	return (-(uchar)z * 16);

}
int FindMin(float * dip, int numOfVertices,bool * visited) {
	int min = 1000000;
	int k = 0;
	int minInd = -1;
	while (k < numOfVertices) {
		
		if ((dip[k] < min) && !visited[k]) {
			min = dip[k];
			minInd = k;
		}
		k++;
	}
	return minInd;
}

float Distance(Vec3f v1, Vec3f v2) {
	return sqrt
		(pow(v1[0] - v2[0], 2) + pow(v1[1] - v2[1], 2));
}
Vec3f ConvertTo3Vecf(PointColor p) {
	return Vec3f(p.getV1(), p.getV2(), p.getV3());
}
cv::Mat ConvertToMatPoints(Grid* grid) {
	int HeightGrid = grid->getHeight();
	int WidthGrid = grid->getWidth();
	Triangle** tr = grid->getTriangle();
	cv::Mat GridMat(HeightGrid, WidthGrid, CV_32FC3);
	GridMat.setTo(cv::Vec3f(-300, -300, -300));
	int k = 0;
	while (tr[k]) {
		Triangle* tr1 = tr[k];
		Triangle* tr2 = tr[k+1];
		int square = k / 2; 
		int i = int(square / HeightGrid);
		int j = square % (WidthGrid);
		if (GridMat.at<Vec3f>(i, j) == cv::Vec3f(-300, -300, -300)) {
			GridMat.at<Vec3f>(i, j) = ConvertTo3Vecf(tr1->getVertice(0));
		}
		if ((j+1 < WidthGrid) && GridMat.at<Vec3f>(i, j+1) == cv::Vec3f(-300, -300, -300)) {
			GridMat.at<Vec3f>(i, j+1) = ConvertTo3Vecf(tr2->getVertice(1));
		}
		if ((i+1 < HeightGrid) && GridMat.at<Vec3f>(i+1,j) == cv::Vec3f(-300, -300, -300)) {
			GridMat.at<Vec3f>(i+1, j) = ConvertTo3Vecf(tr1->getVertice(1));
		}
		if (((j + 1 < WidthGrid) && (i + 1 < HeightGrid))&& GridMat.at<Vec3f>(i+1, j+1) == cv::Vec3f(-300, -300, -300)) {
			GridMat.at<Vec3f>(i+1, j+1) = ConvertTo3Vecf(tr2->getVertice(2));
		}
		k = k  + 2;
	}
	return GridMat;
	

}
cv::Mat ConvertToMatPointsColor(Grid* grid) {
	int HeightGrid = grid->getHeight();
	int WidthGrid = grid->getWidth();
	Triangle** tr = grid->getTriangle();
	cv::Mat GridMat(HeightGrid, WidthGrid, CV_32FC3);
	GridMat.setTo(cv::Vec3f(-300, -300, -300));
	int k = 0;
	while (tr[k]) {
		Triangle* tr1 = tr[k];
		Triangle* tr2 = tr[k + 1];
		int square = k / 2;
		int i = int(square / HeightGrid);
		int j = square % (WidthGrid);
		if (GridMat.at<Vec3f>(i, j) == cv::Vec3f(-300, -300, -300)) {
			Vec3f pointWithColor = Vec3f(tr1->getVertice(0).getV1(), tr1->getVertice(0).getV2(), tr1->getVertice(0).getColor());
			GridMat.at<Vec3f>(i, j) = pointWithColor;
		}
		if ((j + 1 < WidthGrid) && GridMat.at<Vec3f>(i, j + 1) == cv::Vec3f(-300, -300, -300)) {
			Vec3f pointWithColor = Vec3f(tr2->getVertice(1).getV1(), tr2->getVertice(1).getV2(), tr2->getVertice(1).getColor());
			GridMat.at<Vec3f>(i, j + 1) = pointWithColor;
		}
		if ((i + 1 < HeightGrid) && GridMat.at<Vec3f>(i + 1, j) == cv::Vec3f(-300, -300, -300)) {
			Vec3f pointWithColor = Vec3f(tr1->getVertice(1).getV1(), tr1->getVertice(1).getV2(), tr1->getVertice(1).getColor());
			GridMat.at<Vec3f>(i + 1, j) = pointWithColor;
		}
		if (((j + 1 < WidthGrid) && (i + 1 < HeightGrid)) && GridMat.at<Vec3f>(i + 1, j + 1) == cv::Vec3f(-300, -300, -300)) {
			Vec3f pointWithColor = Vec3f(tr2->getVertice(2).getV1(), tr2->getVertice(2).getV2(), tr2->getVertice(2).getColor());
			GridMat.at<Vec3f>(i + 1, j + 1) = pointWithColor;
		}
		k = k + 2;
	}
	return GridMat;


}
bool inBoundaries(float height , float width ,int i, int j) {
	if ((0 <= i && i < height) && (0 <= j && j < width)) {
		return true;

	}
	else {
		return false;
	}

}

void DrawPick() {
		unsigned int i = 0;
		while (grid.getTriangle()[i] != NULL) {
			
			uchar r = (i & 0x000000FF) >> 0;
			uchar g = (i & 0x0000FF00) >> 8;
			uchar b = (i & 0x00FF0000) >> 16;
			
			//std::cout << "R:" << r
			//	<< "G:" << g
			//	<< "B:" << b << "\n";
			Triangle* tr = grid.getTriangle()[i];
			Vec3b colorVec = Vec3b(r, g, b);
			//if (tr != nullptr) {
			tr->getVerticeAddress(0)->setColorRGB(colorVec);
			tr->getVerticeAddress(1)->setColorRGB(colorVec);
			tr->getVerticeAddress(2)->setColorRGB(colorVec);
			//}
			i = i + 1;
		}
	pickingMode = true;
}

void Pick() {

	Triangle** triangles = grid.getTriangle();
	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	uchar data[3];
	cv::Mat img(viewport[3], viewport[2], CV_8UC3);
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
	glReadPixels(0, 0, img.cols, img.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
	cv::Mat flipped(img);
	cv::flip(img, flipped, 0);
	cv::imwrite("snapshot.png", img);

	glReadPixels(MouseXPos,viewport[3] - MouseYPos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
	//uchar color = image.at<Vec3b>(Mou, j * widthSquare);

	int pickedID =
		(data[0] +
		data[1]  * 256 +
		data[2] * 256 * 256 );
	std::cout << "R:" << (int)data[0]
		<< "G:" << (int)data[1]
		<< "B:" << (int)data[2] << "\n" <<
		"XPOS:" << MouseXPos <<
		"YPOS:" << MouseYPos << "\n" <<
		"BackUPXPOS:" << MouseXPos <<
		"BackUPYPOS:" << MouseYPos <<
		"PickedID:" << pickedID << "\n";
	    
	Triangle* trianglePicked = triangles[pickedID];
	if (trianglePicked != NULL) {
		PointColor p1 = trianglePicked->getVertice(0);
		PointColor p2 = trianglePicked->getVertice(1);
		PointColor p3 = trianglePicked->getVertice(2);
		if (trianglePicked->getMark()) {
			trianglePicked->setMark(false);
		}
		else {
			trianglePicked->setMark(true);
		}
	}

}

void UpdateResolutionZ(Grid * grid,Grid * grid2 , int R) {
	cv::Mat gridMat = ConvertToMatPoints(grid);
	cv::Mat gridMat2 = ConvertToMatPoints(grid2);
	int k = 0;
	cv::Mat AccurateGridAverages(grid->getHeight(), grid->getWidth(), CV_32F);
	AccurateGridAverages.setTo(0);
	cv::Mat AccurateGrid;
	cv::Mat AccurateGrid2;
	AccurateGrid2 = ConvertToMatPoints(grid2);
	AccurateGrid = ConvertToMatPoints(grid);

	for (int i = 0; i < AccurateGrid.size().height; i++) {
		for (int j = 0; j < AccurateGrid.size().width; j++) {

			std::queue<Vec3d> queue1;
			//std::queue<Vec2f> queue2;
			float sum = 0;
			int curriGrid2 = R * i;
			int currjGrid2 = R * j;
			int numofVertices = 0;
			Vec3f startingVertice(AccurateGrid2.at<Vec3f>(curriGrid2, currjGrid2));
			std::vector<vector<bool>> visited(AccurateGrid2.size().height);
			for (int m = 0; m < AccurateGrid2.size().height; m++) {
				visited[m] = vector<bool>(AccurateGrid2.size().width);

			}
			queue1.push(Vec3d(curriGrid2, currjGrid2, 0));
			//queue2.push(Vec2f(curriGrid2, currjGrid2));
			while (!queue1.empty()) {
				// get Neightbors
				Vec3d temp;
				temp = queue1.front();
				//Vec2f gridPoint = queue2.front();
				int curriGrid2 = temp[0];
				int currjGrid2 = temp[1];
				queue1.pop();
				// queue2.pop();
				for (int k1 = -1; k1 <= 1; k1++) {
					for (int k2 = -1; k2 <= 1; k2++) {
						Vec3f compareTo(0, 0, 0);
						if (inBoundaries(AccurateGrid2.size().height, AccurateGrid2.size().width, curriGrid2 + k1, currjGrid2 + k2)) {
							compareTo = Vec3f((AccurateGrid2.at<Vec3f>(curriGrid2 + k1, currjGrid2 + k2)));

						}
						if (inBoundaries(AccurateGrid2.size().height, AccurateGrid2.size().width, curriGrid2 + k1, currjGrid2 + k2) && (Distance(compareTo, startingVertice) < 50) && !visited[curriGrid2 + k1][currjGrid2 + k2]) {
							visited[curriGrid2 + k1][currjGrid2 + k2] = true;
							float toAdd = AccurateGrid2.at<Vec3f>(curriGrid2 + k1, currjGrid2 + k2)[2];
							sum = sum + toAdd;
							numofVertices = numofVertices + 1;
							queue1.push(Vec3d(curriGrid2 + k1, currjGrid2 + k2, 0));

						}
					}
				}



			}

			AccurateGridAverages.at<float>(i, j) = sum / numofVertices;
		}
	}
	int a = 0;
	int l = 0;
	Triangle** tr = grid->getTriangle();
	k = 0;
	while (tr[k]) {
		Triangle* tr1 = tr[k];
		Triangle* tr2 = tr[k + 1];
		int square = k / 2;
		int i = int(square / grid->getHeight());
		int j = square % (grid->getWidth());
		tr1->getVerticeAddress(0)->setV3(AccurateGridAverages.at<float>(i, j));
		tr2->getVerticeAddress(0)->setV3(AccurateGridAverages.at<float>(i, j));
		if(i + 1< grid->getHeight()){
	       tr1->getVerticeAddress(1)->setV3(AccurateGridAverages.at<float>(i + 1, j));
		}
		if (j + 1 < grid->getWidth()) {
			tr2->getVerticeAddress(1)->setV3(AccurateGridAverages.at<float>(i, j + 1));
		}
		
		if ((i + 1 < grid->getHeight()) && (j + 1 < grid->getWidth())) {
			tr2->getVerticeAddress(2)->setV3(AccurateGridAverages.at<float>(i + 1, j + 1));
			tr1->getVerticeAddress(2)->setV3(AccurateGridAverages.at<float>(i + 1, j + 1));
		}
		k = k + 2;

	}
	a = 0; 
}

void UpdateResolutionColor(Grid* grid, Grid* grid2, int R) {
	cv::Mat gridMat = ConvertToMatPoints(grid);
	cv::Mat gridMat2 = ConvertToMatPoints(grid2);
	int k = 0;
	cv::Mat AccurateGridAverages(grid->getHeight(), grid->getWidth(), CV_32F);
	AccurateGridAverages.setTo(0);
	cv::Mat AccurateGrid;
	cv::Mat AccurateGrid2;
	AccurateGrid2 = ConvertToMatPointsColor(grid2);
	AccurateGrid = ConvertToMatPointsColor(grid);

	for (int i = 0; i < AccurateGrid.size().height; i++) {
		for (int j = 0; j < AccurateGrid.size().width; j++) {

			std::queue<Vec3d> queue1;
			//std::queue<Vec2f> queue2;
			float sum = 0;
			int curriGrid2 = R * i;
			int currjGrid2 = R * j;
			int numofVertices = 0;
			Vec3f startingVertice(AccurateGrid2.at<Vec3f>(curriGrid2, currjGrid2));
			std::vector<vector<bool>> visited(AccurateGrid2.size().height);
			for (int m = 0; m < AccurateGrid2.size().height; m++) {
				visited[m] = vector<bool>(AccurateGrid2.size().width);

			}
			queue1.push(Vec3d(curriGrid2, currjGrid2, 0));
			//queue2.push(Vec2f(curriGrid2, currjGrid2));
			while (!queue1.empty()) {
				// get Neightbors
				Vec3d temp;
				temp = queue1.front();
				//Vec2f gridPoint = queue2.front();
				int curriGrid2 = temp[0];
				int currjGrid2 = temp[1];
				queue1.pop();
				// queue2.pop();
				for (int k1 = -1; k1 <= 1; k1++) {
					for (int k2 = -1; k2 <= 1; k2++) {
						Vec3f compareTo(0, 0, 0);
						if (inBoundaries(AccurateGrid2.size().height, AccurateGrid2.size().width, curriGrid2 + k1, currjGrid2 + k2)) {
							compareTo = Vec3f((AccurateGrid2.at<Vec3f>(curriGrid2 + k1, currjGrid2 + k2)));

						}
						if (inBoundaries(AccurateGrid2.size().height, AccurateGrid2.size().width, curriGrid2 + k1, currjGrid2 + k2) && (Distance(compareTo, startingVertice) < 20) && !visited[curriGrid2 + k1][currjGrid2 + k2]) {
							visited[curriGrid2 + k1][currjGrid2 + k2] = true;
							float toAdd = AccurateGrid2.at<Vec3f>(curriGrid2 + k1, currjGrid2 + k2)[2];
							sum = sum + toAdd;
							numofVertices = numofVertices + 1;
							queue1.push(Vec3d(curriGrid2 + k1, currjGrid2 + k2, 0));

						}
					}
				}



			}

			AccurateGridAverages.at<float>(i, j) = sum / numofVertices;
		}
	}
	int a = 0;
	int l = 0;
	Triangle** tr = grid->getTriangle();
	k = 0;
	while (tr[k]) {
		Triangle* tr1 = tr[k];
		Triangle* tr2 = tr[k + 1];
		int square = k / 2;
		int i = int(square / grid->getHeight());
		int j = square % (grid->getWidth());
		tr1->getVerticeAddress(0)->setColor(AccurateGridAverages.at<float>(i, j));
		tr2->getVerticeAddress(0)->setColor(AccurateGridAverages.at<float>(i, j));
		if (i + 1 < grid->getHeight()) {
			tr1->getVerticeAddress(1)->setColor(AccurateGridAverages.at<float>(i + 1, j));
		}
		if (j + 1 < grid->getWidth()) {
			tr2->getVerticeAddress(1)->setColor(AccurateGridAverages.at<float>(i, j + 1));
		}

		if ((i + 1 < grid->getHeight()) && (j + 1 < grid->getWidth())) {
			tr2->getVerticeAddress(2)->setColor(AccurateGridAverages.at<float>(i + 1, j + 1));
			tr1->getVerticeAddress(2)->setColor(AccurateGridAverages.at<float>(i + 1, j + 1));
		}
		k = k + 2;

	}
	a = 0;
}
int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	initFog();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	GLfloat scenery = 360.0f - player_pov;
	Triangle** triangles = grid.getTriangle();
	//gluLookAt(Ray.x, Ray.y, Ray.z, 0, 0, 0, 0, 1, 0);
	glRotatef(lookupdown, 1.0f, 0, 0);
	glRotatef(player_pov, 0, 0, 1.0f);
	glTranslatef(-moveX, moveY, -3 - moveZ);
	
	
	if (pickingMode) {
		glDisable(GL_FOG);
		RenderScenes(triangles, true);
		Triangle** temp = triangles;
		int k = 0;
		while (*temp != NULL) {
			Triangle* tr = *temp;
   		    std::cout << "R:" << (int)tr->getVertice(0).getColorRGB().val[0]
				<< "G:" << (int)tr->getVertice(0).getColorRGB().val[1]
				<< "B:" << (int)tr->getVertice(0).getColorRGB().val[2] << "\n";
			temp = temp + 1;
			k = k + 1;
		}
		int show_K = k;
		int a = 1;
		DoneDrawingUnique = true;
	}
	else {
		//initFog();
		RenderScenes(triangles, false);
		
		if (fog_rain) {
			
			drawRain();
		}
		if (rain_fog) {
			glEnable(GL_FOG);

		}
		else if (!rain_fog) {
			glDisable(GL_FOG);
		}
	}
		

	
	
	return TRUE;
	// Everything Went OK
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	grid.KillGrid();
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))										// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}

	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle = WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	
	switch (uMsg)									// Check For Windows Messages
	{
	case WM_ACTIVATE:							// Watch For Window Activate Message
	{
		if (!HIWORD(wParam))					// Check Minimization State
		{
			active = TRUE;						// Program Is Active
		}
		else
		{
			active = FALSE;						// Program Is No Longer Active
		}

		return 0;								// Return To The Message Loop
	}

	case WM_SYSCOMMAND:							// Intercept System Commands
	{
		switch (wParam)							// Check System Calls
		{
		case SC_SCREENSAVE:					// Screensaver Trying To Start?
		case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
			return 0;							// Prevent From Happening
		}
		break;									// Exit
	}

	case WM_CLOSE:								// Did We Receive A Close Message?
	{
		PostQuitMessage(0);						// Send A Quit Message
		return 0;								// Jump Back
	}
	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		keys[wParam] = TRUE;					// If So, Mark It As TRUE
		return 0;								// Jump Back
	}

	case WM_KEYUP:								// Has A Key Been Released?
	{
		keys[wParam] = FALSE;					// If So, Mark It As FALSE
		return 0;								// Jump Back
	}

	case WM_SIZE:								// Resize The OpenGL Window
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}	
	case WM_LBUTTONDOWN:
	{
		MouseXPos = LOWORD(lParam);
		MouseYPos = HIWORD(lParam);
		DrawPick();
		//Selection();
	}
	case WM_MOUSEMOVE:								// Resize The OpenGL Window
	{
		if (!pickingMode  && !DoneDrawingUnique){
		   float xpos = LOWORD(lParam);
		   float ypos = HIWORD(lParam);
		  // std::cout << xpos << ":" << ypos << "\n";
		   MouseXPos = xpos;
		   MouseYPos = ypos;
		   BackUpY = ypos;
		   BackUpX = xpos;
		}

	}
	case WM_MOUSEACTIVATE:
	{
		keys[8] = TRUE;
		return 0;
	}
}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,		// Previous Instance
	LPSTR		lpCmdLine,			// Command Line Parameters
	int			nCmdShow)			// Window Show State
{
	_CrtMemCheckpoint(&sOld); //take a snapshot
	MSG		msg;									// Windows Message Structure
	BOOL	done = FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL, "Would You Like To Run In Fullscreen Mode?", "Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		fullscreen = FALSE;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow((char*)"NeHe's OpenGL Framework", Width, Height, 16, fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while (!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
			{
				done = TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active)								// Program Active?
			{
				if (keys[VK_ESCAPE])				// Was ESC Pressed?
				{
					done = TRUE;						// ESC Signalled A Quit
				}
				else								// Not Time To Quit, Update Screen
				{
					if (pickingMode) {
						DrawGLScene();

						//SwapBuffers(hDC);
						glFlush();
						glFinish();
						//SwapBuffers(hDC);
						Pick();
						pickingMode = false;
						DrawGLScene();

					}
					else
					{
						DrawGLScene();
					}
					SwapBuffers(hDC);
					// Swap Buffers (Double Buffering)
				}
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1] = FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen = !fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow((char*)"NeHe's OpenGL Framework", 640, 480, 16, fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1] = FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen = !fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow((char*)"NeHe's OpenGL Framework", 640, 480, 16, fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
			if (keys[VK_UP])                // Is Up Arrow Being Pressed?
			{
				moveY -= cos(player_pov * piover180) * 1.0f;              // If So, Decrease xspeed
				moveX += sin(player_pov * piover180) * 1.0f;
				moveZ -= cos(lookupdown * piover180) * 1.0f;              // If So, Decrease xspeed

			}
			if (keys[VK_DOWN])              // Is Down Arrow Being Pressed?
			{
				moveY += cos(player_pov * piover180) * 1.0f;              // If So, Decrease xspeed
				moveX -= sin(player_pov * piover180) * 1.0f;             // If So, Increase xspeed
				moveZ += cos(lookupdown * piover180) * 1.0f;
			}
			if (keys[VK_RIGHT])             // Is Right Arrow Being Pressed?
			{
				player_pov += 1.0f;            // If So, Increas	e yspeed
			}
			if (keys[VK_LEFT])              // Is Left Arrow Being Pressed?
			{
				player_pov -= 1.0f;              // If So, Decrease yspeed
			}
			if (keys['Y'])              // Is Left Arrow Being Pressed?
			{
				lookupdown -= 1.0f;

			}
			if (keys['H'])              // Is Left Arrow Being Pressed?
			{
				lookupdown += 1.0f;


			}

			if (keys['F'])              // Is Left Arrow Being Pressed?
			{
				fogScale += 0.01f;            // If So, Decrease yspeed
				keys['F'] = false;
			}
			if (keys['G'])              // Is Left Arrow Being Pressed?
			{
				fogScale -= 0.01f;            // If So, Decrease yspeed
				keys['G'] = false;
			}
			if (keys['O'])              // Is Left Arrow Being Pressed?
			{
				velocity += 1.0f;            // If So, Decrease yspeed
			}
			if (keys['P'])              // Is Left Arrow Being Pressed?
			{
				velocity -= 1.0f;            // If So, Decrease yspeed
			}
			if (keys['J']) {
				// Set the x-coord variable
				showLines = !showLines;

			}
			if (keys['L']) {
				// Set the x-coord variable
				fog_rain = !fog_rain;
				keys['L'] = false;



			}
			if (keys['K']) {
				// Set the x-coord variable
				rain_fog = !rain_fog;
				keys['K'] = false;



			}
			if (keys['R']) {
				// Set the x-coord variable
				if (!HighResolution) {
					R = 2;
					Grid grid2 = Grid(R * RectangleHeight, R * RectangleWidth, image, image.cols, image.rows);
					BackUpGrid = Grid(grid);
					UpdateResolutionZ(&grid, &grid2, R);
					UpdateResolutionColor(&grid, &grid2, R);
					HighResolution = true;
					keys['R'] = false;
				}
				else
				{
					keys['R'] = false;
					grid=Grid(BackUpGrid);
					HighResolution = false;
				}
			


			}
			if (keys['C']) {
				// Set the x-coord variable
				if (low_res) {
					fine_factor++;
					Grid grid2 = Grid(2 * RectangleHeight, 2 * RectangleWidth, image, image.cols, image.rows);
					Triangle** triangles = grid.getTriangle();
					Triangle** triangles2 = grid2.getTriangle();
					int k = 0;
					while (triangles[k] != NULL) {
						if (triangles[k]->getMark()) {

							int movedCols = k / (2 * RectangleWidth);
							int b = 2 * RectangleWidth;
							int smallIndex = ((((k % b) / 2) * 4) + 1) + (movedCols * 8 * RectangleWidth);
							triangles2[smallIndex]->setMark(true);

						}
						k++;
					}
					grid = Grid(grid2);
					low_res = false;
					
				}
				keys['C'] = false;
				
			}
			if (keys['B']) {
				if (!low_res) {
					// Set the x-coord variable
					fine_factor--;
					Grid grid2 = Grid(RectangleHeight, RectangleWidth, image, image.cols, image.rows);
					Triangle** triangles = grid.getTriangle();
					Triangle** triangles2 = grid2.getTriangle();
					int k = 0;
					while (triangles[k] != NULL) {
						if (triangles[k]->getMark()) {


							int triangles_inrow = 4 * RectangleWidth;
							int row = (k % triangles_inrow)/2;
							int col = k / triangles_inrow;
							int transformedCol = col / 2;
							int transformedRow = row / 2;
							int transformedIndex = (transformedCol * 2 * RectangleWidth) + (2 * transformedRow);
							triangles2[transformedIndex]->setMark(true);


						}
						k++;
					}
					grid = Grid(grid2);
					low_res = true;
				}
				keys['B'] = false;


			}
			if (keys['V']) {
				// Set the x-coord variable
				cv::Mat Points = ConvertToMatPoints(&grid);
				Triangle** triangles = grid.getTriangle();
				int k = 0;
				cv::Vec2d OriginDest[2];
				int m = 0; 
				while (triangles[k]) {
					if (triangles[k]->getMark() || triangles[k+1]->getMark()) {
						int square = k / 2;
						int i = int(square / Points.size().height);
						int j = square % (Points.size().width);
						if (triangles[k]->getMark() && triangles[k + 1]->getMark()){
							OriginDest[m++] = Vec2d(i, j);
							OriginDest[m++] = Vec2d(i, j);
							
						}
						if (triangles[k]->getMark() && !triangles[k + 1]->getMark()) {
							OriginDest[m++] = Vec2d(i, j);
						}
						if (!triangles[k]->getMark() && triangles[k + 1]->getMark()) {
							OriginDest[m++] = Vec2d(i, j);
						}
						if (m == 2) {
							break;
						}
					}
					k= k +2;

				}
				
				int numofVertices = Points.size().height * Points.size().width;
				//std::priority_queue<Vec3d,std::vector<Vec3d>,Compare> queue1;
				//queue1.push(OriginDest[0]);




				float * dist = new float[numofVertices];
				int * prev = new int[numofVertices];
				bool* visited = new bool[numofVertices];
				//std::priority_queue<Vec2d , std::vector<Vec2d>, Compare> queue1({Compare{dist, Points.size().width}});
				std::vector<Vec2d> queue1;
				Vec2d origin = Vec2d(OriginDest[0]);
				//bool * visited = new bool[numofVertices];
 			    int index_origin = translate2Dto1D(origin, Points.size().width);
				int l = 0;
				while(l< numofVertices){
					if (l == index_origin)
					{
						dist[l] = 0;
					}
					else {
						dist[l] = 1000000;

					}
					l++;
					
				}
				int l2 = 0;
				while (l2 < numofVertices) {
					queue1.push_back(translate1Dto2D(l2, Points.size().width));
					prev[l2] = -1;
					l2++;

				}
				int l3 = 0;
				while (l3 < numofVertices) {
					visited[l3] = false;
					l3++;

				}
				

				while (!queue1.empty()) {
					int mindist = FindMin(dist, numofVertices,visited);
					Vec2d u = translate1Dto2D(mindist,Points.size().width);
					//Vec2d u = queue1.top();
					int GridI = u[0];
					int GridJ = u[1];
					queue1.erase(std::find(queue1.begin(), queue1.end(), u));
					visited[translate2Dto1D(u, Points.size().width)]=true;
					if (u == OriginDest[1]) {
						break; 
					}
					for (int k1 = -1; k1 <= 1; k1++) {
						for (int k2 = -1; k2 <= 1; k2++) {
							int a = 1;
							if (inBoundaries(Points.size().height, Points.size().width, GridI + k1, GridJ + k2) && (k1 != 0 || k2 != 0) && !visited[translate2Dto1D(Vec2d(GridI + k1, GridJ + k2), Points.size().width)]) {
								Vec3f N1 = Points.at<Vec3f>(GridI+k1, GridJ + k2);
								Vec3f curr = Points.at<Vec3f>(GridI, GridJ);
								float distanceToNeighbor = distance3d(N1, curr);
								float alt = dist[translate2Dto1D(u, Points.size().width)] + distanceToNeighbor;
								if (alt < dist[translate2Dto1D(Vec2d(GridI + k1, GridJ + k2), Points.size().width)]) {
									dist [translate2Dto1D(Vec2d(GridI + k1, GridJ + k2), Points.size().width)]= alt;
									prev[translate2Dto1D(Vec2d(GridI + k1, GridJ + k2), Points.size().width)] = translate2Dto1D(u, Points.size().width);
								}



							}
						}
					}
					

				}
				

				
				Vec3f temp = Points.at<Vec3f>(OriginDest[1][0], OriginDest[1][1]);
				Vec2d currIndices = Vec2d(OriginDest[1][0], OriginDest[1][1]);
				Vec3f source = Points.at<Vec3f>(OriginDest[0][0], OriginDest[0][1]);
				ShortestPath.push(temp);
				while (temp != source) {
					//ShortestPath.push(temp);
					int GridI = currIndices[0];
					int GridJ = currIndices[1];
					int previous = prev[(GridI* Points.size().width) + (GridJ)];
					currIndices = Vec2d(int(previous/ Points.size().width), previous % Points.size().width);
					temp = Points.at<Vec3f>(currIndices[0], currIndices[1]);
					ShortestPath.push(temp);


				}
				CalcPath = true;
				delete prev;
				delete dist;
				delete visited;
				

			    
			}
			if (keys['M'] && !low_res) {
				// Set the x-coord variable
				Grid grid2 = Grid(RectangleHeight, RectangleWidth, image, image.cols, image.rows);
				Triangle** triangles1 = grid.getTriangle();
				Triangle** triangles2 = grid2.getTriangle();
				int k1 = 0;
				while (triangles1[k1] != NULL) {
					if (triangles1[k1]->getMark()) {


						int triangles_inrow = 4 * RectangleWidth;
						int row = (k1 % triangles_inrow) / 2;
						int col = k1 / triangles_inrow;
						int transformedCol = col / 2;
						int transformedRow = row / 2;
						int transformedIndex = (transformedCol * 2 * RectangleWidth) + (2 * transformedRow);
						triangles2[transformedIndex]->setMark(true);


					}
					k1++;
				}
				cv::Mat Points = ConvertToMatPoints(&grid2);
				Triangle** triangles = grid2.getTriangle();
				int k = 0;
				cv::Vec2d OriginDest[2];
				int m = 0;
				while (triangles[k]) {
					if (triangles[k]->getMark() || triangles[k + 1]->getMark()) {
						int square = k / 2;
						int i = int(square / Points.size().height);
						int j = square % (Points.size().width);
						if (triangles[k]->getMark() && triangles[k + 1]->getMark()) {
							OriginDest[m++] = Vec2d(i, j);
							OriginDest[m++] = Vec2d(i, j);

						}
						if (triangles[k]->getMark() && !triangles[k + 1]->getMark()) {
							OriginDest[m++] = Vec2d(i, j);
						}
						if (!triangles[k]->getMark() && triangles[k + 1]->getMark()) {
							OriginDest[m++] = Vec2d(i, j);
						}
						if (m == 2) {
							break;
						}
					}
					k = k + 2;

				}

				int numofVertices = Points.size().height * Points.size().width;
				//std::priority_queue<Vec3d,std::vector<Vec3d>,Compare> queue1;
				//queue1.push(OriginDest[0]);




				float* dist = new float[numofVertices];
				int* prev = new int[numofVertices];
				bool* visited = new bool[numofVertices];
				//std::priority_queue<Vec2d , std::vector<Vec2d>, Compare> queue1({Compare{dist, Points.size().width}});
				std::vector<Vec2d> queue1;
				Vec2d origin = Vec2d(OriginDest[0]);
				//bool * visited = new bool[numofVertices];
				int index_origin = translate2Dto1D(origin, Points.size().width);
				int l = 0;
				while (l < numofVertices) {
					if (l == index_origin)
					{
						dist[l] = 0;
					}
					else {
						dist[l] = 1000000;

					}
					l++;

				}
				int l2 = 0;
				while (l2 < numofVertices) {
					queue1.push_back(translate1Dto2D(l2, Points.size().width));
					prev[l2] = -1;
					l2++;

				}
				int l3 = 0;
				while (l3 < numofVertices) {
					visited[l3] = false;
					l3++;

				}


				while (!queue1.empty()) {
					int mindist = FindMin(dist, numofVertices, visited);
					Vec2d u = translate1Dto2D(mindist, Points.size().width);
					//Vec2d u = queue1.top();
					int GridI = u[0];
					int GridJ = u[1];
					queue1.erase(std::find(queue1.begin(), queue1.end(), u));
					visited[translate2Dto1D(u, Points.size().width)] = true;
					if (u == OriginDest[1]) {
						break;
					}
					for (int k1 = -1; k1 <= 1; k1++) {
						for (int k2 = -1; k2 <= 1; k2++) {
							int a = 1;
							if (inBoundaries(Points.size().height, Points.size().width, GridI + k1, GridJ + k2) && (k1 != 0 || k2 != 0) && !visited[translate2Dto1D(Vec2d(GridI + k1, GridJ + k2), Points.size().width)]) {
								Vec3f N1 = Points.at<Vec3f>(GridI + k1, GridJ + k2);
								Vec3f curr = Points.at<Vec3f>(GridI, GridJ);
								float distanceToNeighbor = distance3d(N1, curr);
								float alt = dist[translate2Dto1D(u, Points.size().width)] + distanceToNeighbor;
								if (alt < dist[translate2Dto1D(Vec2d(GridI + k1, GridJ + k2), Points.size().width)]) {
									dist[translate2Dto1D(Vec2d(GridI + k1, GridJ + k2), Points.size().width)] = alt;
									prev[translate2Dto1D(Vec2d(GridI + k1, GridJ + k2), Points.size().width)] = translate2Dto1D(u, Points.size().width);
								}



							}
						}
					}


				}



				Vec3f temp = Points.at<Vec3f>(OriginDest[1][0], OriginDest[1][1]);
				Vec2d currIndices = Vec2d(OriginDest[1][0], OriginDest[1][1]);
				Vec3f source = Points.at<Vec3f>(OriginDest[0][0], OriginDest[0][1]);
				ShortestPath.push(temp);
				while (temp != source) {
					//ShortestPath.push(temp);
					int GridI = currIndices[0];
					int GridJ = currIndices[1];
					int previous = prev[(GridI * Points.size().width) + (GridJ)];
					currIndices = Vec2d(int(previous / Points.size().width), previous % Points.size().width);
					temp = Points.at<Vec3f>(currIndices[0], currIndices[1]);
					ShortestPath.push(temp);


				}
				CalcPath = true;
				delete prev;
				delete dist;
				delete visited;



			}

		 }

    }
	
	// Shutdown
	KillGLWindow();									// Kill The Window
	_CrtMemCheckpoint(&sNew);
	if (_CrtMemDifference(&sDiff, &sOld, &sNew)) // if there is a difference
	{
		OutputDebugString("-----------_CrtMemDumpStatistics ---------");
		_CrtMemDumpStatistics(&sDiff);
		OutputDebugString("-----------_CrtMemDumpAllObjectsSince ---------");
		_CrtMemDumpAllObjectsSince(&sOld);
		OutputDebugString("-----------_CrtDumpMemoryLeaks ---------");
		_CrtDumpMemoryLeaks();
	}
	return (msg.wParam);							// Exit The Program
}