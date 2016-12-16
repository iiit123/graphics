#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <string>
#include <fstream>

#define SIZE 1024

// // define 16 control points
// static GLfloat ctrlPoints[4][4][3] = {
// 	{{-3, 0, 3}, {-1, 0, 3}, {1, 0, 3}, {3, 0, 3}},
// 	{{-3, 0, 1}, {-1, 0, 1}, {1, 0, 1}, {3, 0, 1}},
// 	{{-3, 0, -1}, {-1, 0 , -1}, {1, 0, -1}, {3, 0, -1}},
// 	{{-3, 0, -3}, {-1, 0 , -3}, {1, 0, -3}, {3, 0, -3}}
// };

static GLfloat ctrlPoints[4][4][3] = {
   { {-1.5, -1.5, 4.0},
     {-0.5, -1.5, 2.0},
     {0.5, -1.5, -1.0},
     {1.5, -1.5, 2.0}},
   { {-1.5, -0.5, 1.0},
     {-0.5, -0.5, 3.0},
     {0.5, -0.5, 0.0},
     {1.5, -0.5, -1.0}},
   { {-1.5, 0.5, 4.0},
     {-0.5, 0.5, 0.0},
     {0.5, 0.5, 3.0},
     {1.5, 0.5, 4.0}},
   { {-1.5, 1.5, -2.0},
     {-0.5, 1.5, -2.0},
     {0.5, 1.5, 0.0},
     {1.5, 1.5, -1.0}}
};

// controlling matrix from interpolation to Bezier
GLfloat Interp2BezierMatrix[4][4]={
	{1.0, 0.0, 0.0, 0.0},
	{-5.0/6.0, 3.0, -3.0/2.0, 1.0/3.0},
	{1.0/3.0, -3.0/2.0, 3.0, -5.0/6.0},
	{0.0, 0.0, 0.0, 1.0}
};

// window size
int ww, wh;

GLint viewport[4];
GLdouble modelViewMatrix[16], projectionMatrix[16];

// z value when picking one of the control points
float z = 0;

// define look-at positioning
static GLdouble eye[] = {0.0, 3.5, 1.0};
static GLdouble at[] = {0.0, 0.0, 0.0};
static GLdouble up[] = {0.0, 1.0, 0.0};

GLdouble theta = atan(eye[1] / eye[2]) * 180 / 3.1415926;
GLdouble phi = 270.0;
GLdouble radius = sqrt(eye[1] * eye[1] + eye[2] * eye[2]);

// find how many hits when selection
int pointHits = 0;

// remember the moving control name
int MOVENAME = 0;

int patchMode = 1; // 1 - GL_LINE;
                   // 2 - GL_FILL;
                   // 3 - GL_POINT;

int typeMode = 1; // 1 - Bezier;
                  // 2 - Interpolating Cubic;

// Interpolating control points with Interpolating to Bezier matrix
void interpolatingPoints(GLfloat ctrlPnts_org[4][4][3], GLfloat ctrlPnts_new[4][4][3])
{
	GLfloat tmp[4][4][3];
	int i, j, l, m;
	for(l = 0; l < 3; l++) {
		for(i = 0; i < 4; i++) {
			for(j = 0; j < 4; j++) {
				tmp[i][j][l] = 0.0;
				for(m = 0; m < 4; m++) {
					tmp[i][j][l] += Interp2BezierMatrix[i][m]*ctrlPnts_org[m][j][l];
				}
			}
		}
	}

	for(l = 0; l < 3; l++) {
		for(i = 0; i < 4; i++) {
			for(j = 0; j < 4; j++) {
				ctrlPnts_new[i][j][l] = 0.0;
				for(m = 0; m < 4; m++){
					ctrlPnts_new[i][j][l] += tmp[i][m][l]*Interp2BezierMatrix[j][m];
				}
			}
		}
	}
}

// set up the position of a camera
void setupLookAtPosition()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(eye[0], eye[1], eye[2],
              at[0], at[1], at[2],
			  up[0], up[1], up[2]);
}

// draw 16 control points
void draw16Spheres(GLenum mode) 
{
	int i = 1;
	
	for (int a = 0; a < 4; a++) {
		for (int b = 0; b < 4; b++) {
			glPushMatrix();
			if (MOVENAME == 4 * a + b + 1) { 
				glColor3f(1.0, 1.0, 0.0);
			} else {
				glColor3f(1.0, 0.0, 0.0);
			}
			glTranslatef(ctrlPoints[a][b][0], 
				         ctrlPoints[a][b][1], 
						 ctrlPoints[a][b][2]);
			if (mode == GL_SELECT) glLoadName(i);
			glutSolidSphere(0.1, 8, 8); 
			glPopMatrix();
			++i;
		}
	}
}

// draw a green wireframe (patch)
void drawGrid()
{
	GLfloat ctrlPoints_Interp[4][4][3];
	
	if (typeMode == 2) {
		interpolatingPoints(ctrlPoints, ctrlPoints_Interp);
	}

	glColor3f(0.0, 1.0, 0.0);

	// depend on the choice of the patch type, send in the correnponding control
	// points to Bezier evaluator
	if (typeMode == 1) {
		glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &ctrlPoints[0][0][0]);
	} else {
		glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &ctrlPoints_Interp[0][0][0]);
	}
	switch (patchMode) {
		case 1:
			glEvalMesh2(GL_LINE, 0, 24, 0, 24);
			break;
		case 2: 
			glEvalMesh2(GL_FILL, 0, 24, 0, 24);			
			break;
		default:
			glEvalMesh2(GL_POINT, 0, 24, 0, 24);
	}
}

// initiate the environment before the program starts
void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_MAP2_VERTEX_3);
    glMapGrid2f(24, 0.0, 1.0, 24, 0.0, 1.0);

	GLfloat mat_specular[] = {0.0, 1.0, 0.0, 1.0};
	GLfloat mat_shininess[] = {50.0};
	GLfloat light_position[] = {-2.0, 2.0, 2.0, 1.0};
	GLfloat green_light[] = {0.0, 1.0, 0.0, 1.0};
	GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, green_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, green_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}

// draw 16 control points and a wireframe
void myDisplay() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setupLookAtPosition();
    drawGrid();
    draw16Spheres(GL_RENDER);
	glFlush();
}

// find out the control point information after it is picked
void processHits(GLint hits, GLuint buffer[])
{
	int i, j;
	GLuint names, *ptr;
	float z1, z2;
	ptr =(GLuint *) buffer;
    printf ("hits = %d\n", hits);
	names = *ptr;
	for (i = 0; i < hits; i++) {
		for (j = 0; j < names; j++) {
			z1 = (float) *(ptr+1)/0xffffffff;
			printf("  z1 is %g;", (float) *(ptr+1)/0xffffffff );
			z2 = (float) *(ptr+2)/0xffffffff;
			printf(" z2 is %g\n", (float) *(ptr+2)/0xffffffff);
			z = (z1 + z2) / 2;
			MOVENAME = *(ptr + 3);
		}
	}
}

// setup the  perpective viewing
void proj()
{
	if (ww <= wh)
		glFrustum(-3.5, 3.5, -3.5*(GLfloat)wh/(GLfloat)ww, 3.5*(GLfloat)wh/(GLfloat)ww, 1.8, 20.0);
	else
		glFrustum(-3.5*(GLfloat)ww/(GLfloat)wh, 3.5*(GLfloat)ww/(GLfloat)wh, -3.5, 3.5, 1.8, 20.0);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	ww = w;
	wh = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	proj();
    viewport[0] = 0;
    viewport[1] = 0;
    viewport[2] = ww;
    viewport[3] = wh;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// mouse function
void myPick(int button, int state, int xPosition, int yPosition)
{
	// left mouse button down
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		GLuint selectBuf[SIZE];
		GLint hits;
		glSelectBuffer(SIZE, selectBuf);
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(0);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPickMatrix((GLdouble) xPosition, (GLdouble) (wh - yPosition), 2.0, 2.0, viewport);
		proj();

		glMatrixMode(GL_MODELVIEW);

		draw16Spheres(GL_SELECT);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		hits = glRenderMode(GL_RENDER);
		pointHits = hits;
		printf ("hits0 = %d\n", hits);
		processHits(hits, selectBuf);
	}

	// left mouse button up
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		MOVENAME = 0;
	}
	glutPostRedisplay();
}

// mouse motion function
void myMouseMove(int xPosition, int yPosition)
{
	GLdouble pointX, pointY, pointZ;

	if (pointHits > 0) {		
		glGetIntegerv(GL_VIEWPORT, viewport);
	    glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

		gluUnProject((GLdouble) xPosition, (GLdouble) (wh - yPosition) , (GLdouble) z, 
			modelViewMatrix, projectionMatrix, viewport, 
			&pointX, &pointY, &pointZ);

		ctrlPoints[MOVENAME / 4][MOVENAME % 4 - 1][0] = pointX;
		ctrlPoints[MOVENAME / 4][MOVENAME % 4 - 1][1] = pointY;
		ctrlPoints[MOVENAME / 4][MOVENAME % 4 - 1][2] = pointZ;
	
		glutPostRedisplay();
	}
}

// keyboard function for key :"i", "I", "o", "O" 
void myKeyboard(unsigned char key, int x, int y)
{
	if (key == 'o' || key == 'O' || key == 'i' ||  key == 'I') {
		if(key == 'o' || key == 'O') radius += 0.01;
		if(key == 'i' || key == 'I') radius -= 0.01;
		eye[0] = radius * cos(theta * 3.14 / 180) * cos(phi * 3.14 / 180);
		eye[1] = radius * sin(theta * 3.14 / 180);
		eye[2] = -radius * cos(theta * 3.14 / 180) * sin(phi * 3.14 / 180);
	}
	if(key == 27)  exit(0);
	glutPostRedisplay();
}

// spectial key function for up arrow, down arrow, left arrow and right arrow
void specialKeys(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_DOWN:			
			theta -= 1;
			break;
		case GLUT_KEY_UP:		
			theta += 1;
			break;
		case GLUT_KEY_RIGHT:			
			phi -= 1;
			break;
		case GLUT_KEY_LEFT:		
			phi += 1;
			break;
		case GLUT_KEY_PAGE_UP:		// turn and move camera up 
			radius += 0.01;
			break;
		case GLUT_KEY_PAGE_DOWN:	// turn and move camera down
			radius -= 0.01;
			break;
	}
	eye[0] = radius * cos(theta * 3.14 / 180) * cos(phi * 3.14 / 180);
	eye[1] = radius * sin(theta * 3.14 / 180);
	eye[2] = -radius * cos(theta * 3.14 / 180) * sin(phi * 3.14 / 180);

	glutPostRedisplay();
}

// menu
void mainMenu(int id)
{
	if (id == 1) {
		exit(0);
	}  else {
       ;;		 
	}
}

// patch mode (line, polygon and point) menu
void patchModeMenu(int id)
{
	patchMode = id;
	switch (patchMode) {
		case 1:
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			break;
		case 2: {
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			break;
		}
		default:
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
	}
	glutPostRedisplay();
}

// patch type (Bezier, Interpolating Cubic) menu
void patchTypeMenu(int id)
{
	typeMode = id;
	glutPostRedisplay();
}

// create menus
void creatMenus()
{
	int pMenu, typeMenu;
	pMenu = glutCreateMenu(patchModeMenu);
	glutAddMenuEntry("LINE", 1);
	glutAddMenuEntry("POLYGON", 2);
	glutAddMenuEntry("POINT", 3);

	typeMenu = glutCreateMenu(patchTypeMenu);
	glutAddMenuEntry("Bezier Patch", 1);
	glutAddMenuEntry("Interpolating Cubic Patch", 2);

	glutCreateMenu(mainMenu);
	glutAddSubMenu("Patch Mode...", pMenu);
	glutAddSubMenu("Patch Type...", typeMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// main function
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Interactive Bezier Surface");
	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(myDisplay);

	creatMenus();
    glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(specialKeys);

	glutMouseFunc(myPick);
	glutMotionFunc(myMouseMove);

	glutMainLoop();
	return 0;
}