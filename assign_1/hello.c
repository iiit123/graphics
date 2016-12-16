#include<stdio.h>
#include <iostream>
#include<stdio.h>
#include<math.h>
#include <cmath>
#include <GLUT/glut.h>
#include<stdlib.h>

int start_x, start_y, current_x = -1, current_y = -1, x_points[1000][1000], y_points[1000][1000], num_of_objects=0, left_click_counts[1000] = {0};
int w, h, windowHeight, windowWidth, condition = 1;
float smile = 10, eyes_width = 0.1;


void swap_values(int *a, int *b) {
	int t = *a;
	*a = *b;
	*b = t;
}

void reflectPointsParabola(int x, int y, int x_center, int y_center) {

	glBegin(GL_POINTS); 
	glPointSize(1);
	glVertex2f(x_center + x, y_center - y);
	glVertex2f(x_center - x, y_center - y);
	glEnd();
}

//Drawing a parabola x*x = ay ... ie symetric about y axis;
void ownParabolaDraw(int x_center, int y_center, float a, int height) {
	float delta_r1 = 1 - (a/2);
	int x =0, y=0;   //both of them start from origin.

	// we know that slope at the point is (x/2*a)
	// need to check for slope > 1 and < 1
	while(x < 2*a) {   // slope less than 1 increment x;

		reflectPointsParabola(x, y, x_center, y_center);

		if(delta_r1 > 0) {    // Midpoint is outside the parabola .. go for top point
			delta_r1 = delta_r1 + 2*x + 3 - 4*a;
			y = y + 1;
		}   
		else {
			delta_r1 = delta_r1 + 2*x + 3;
		}   
		x++;
	}   

	float delta_r2 = 0.25 - 4*a;

	//slope greater than 1 increment y;
	while(y < height) { //Limiting the upper end of the parabola to particular ht 

		reflectPointsParabola(x, y, x_center, y_center);

		if(delta_r2 > 0) {
			delta_r2 = delta_r2 - 4*a;
		}   
		else {
			delta_r2 = delta_r2 + 2 + 2*x - 4*a;
			x++; 
		}   
		y++;
	}   
}

void reflectPointsEllipse(int x, int y, int x_center, int y_center) {

	glBegin(GL_POINTS); 
	glPointSize(1);
	glVertex2f(x_center + x, y_center + y);
	glVertex2f(x_center + x, y_center - y);
	glVertex2f(x_center - x, y_center - y);
	glVertex2f(x_center - x, y_center + y);
	glEnd();
}

void ownElipseDraw(int x_center, int y_center, int a,  int b) {
	int sq_a = a*a , sq_b = b*b;
	float delta_r1 = sq_b - sq_a*b + (1/4)*sq_a;
	int x = 0 , y=b;
	int r1_cond = sq_a*(y - 0.5) - sq_b*(x + 1);

	//We are checking for region one where x increases more rapidly than y.
	while(r1_cond > 0) {

		reflectPointsEllipse(x ,y, x_center, y_center);

		if(delta_r1 < 0) {
			// we are selecting the east point since the midpoint is present inside the ellipse.
			delta_r1 = delta_r1 + sq_b*(2*x + 3);
		}
		else {
			delta_r1 = delta_r1 + sq_b*(2*x + 3) + sq_a*(2- 2*y);
			y = y-1;
		}
		x = x + 1; 
		r1_cond = sq_a*(y - 0.5) - sq_b*(x + 1);		
	}


	//We are checking for region two where y increases more rapidly than x.
	float delta_r2 = sq_b * pow(x+0.5,2) + sq_a * pow(y-1,2) - (sq_a*sq_b);

	//Now we have to traverse through the end of the co-ordinate ==> y=0 ie x axis.
	//In this case we are decementing the y value and selection south or south east point
	while(y > 0) {

		reflectPointsEllipse(x, y, x_center, y_center);
		if(delta_r2 < 0) { 
			// We are selecting the east point in this case.
			delta_r2 = delta_r2 + sq_b*(2+2*x) + sq_a*(3-2*y);
			x++;
		} 
		else {
			// we are selecting the south east point in this case.
			delta_r2 = delta_r2 + sq_a*(3-2*y); 
		}
		y--;
	}

}

void ownLineDraw(int strt_x, int strt_y, int final_x, int final_y, int flag) {

	int dx = abs(final_x - strt_x); 
	int dy = abs(final_y - strt_y);	
	float inv_sqrt_slopes = 1.0/sqrt(dx*dx + dy*dy);
	int slope_flag = ((dy>dx)? 1:0);
	float v_dx = 0;                                   


	if(dy>dx)
	{
		swap_values(&strt_x, &strt_y);
		swap_values(&final_x, &final_y);

		//swap the values and compute for new dx and dy
		dx = abs(final_x - strt_x);
		dy = abs(final_y - strt_y);	
	}

	if(strt_x > final_x)
	{
		swap_values(&strt_x, &final_x);
		swap_values(&strt_y, &final_y);
	}

	int increment_y = ((strt_y>final_y) ? -1*flag :1*flag);

	int delta = (2 * dy) - dx;
	int delta_east = 2 * dy;
	int delta_north_east = 2 * (dy-dx);

    float factor = 0.5 * inv_sqrt_slopes; 
    float dx_factor = dx * inv_sqrt_slopes; 

	while(strt_x < final_x) {

		//checking for the delta value for next point (xp+1, yp+1) 
		if(delta <= 0) { 
			//Indicates the midpoint is above the line, choose the east point.
			delta = delta + delta_east;
			v_dx = delta + dx; 
			
			strt_x = strt_x + flag;
		}

		else {
			//Midpoint is below the line. Choose the above point.
			delta = delta + delta_north_east;
			v_dx = delta - dx; 

			strt_x = strt_x + flag;
			strt_y = strt_y + increment_y;
		}

		float lower_color_factor = dx_factor - v_dx*factor;

		float upper_color_factor =  dx_factor + v_dx*factor;

		lower_color_factor = ((lower_color_factor<0)? -1*lower_color_factor : lower_color_factor);
		lower_color_factor = ((lower_color_factor>1)? 1 : lower_color_factor);

		upper_color_factor = ((upper_color_factor<0)? -1*upper_color_factor : upper_color_factor);
		upper_color_factor = ((upper_color_factor>1)? 1 : upper_color_factor);


		//Ploting the Line
		//For anti aliasing ploting the 3 points at a time and increasing the thinckness of the line.
		//I also have to change the intensity depending on the distance. 
		//How to pass the color ... m struck :/ ...
		glBegin(GL_POINTS); 
		glPointSize(1);
		glColor3f(1.0f, 0.0f, 0.0f);

		if(slope_flag == 1) {
			glVertex3f(strt_y, strt_x, 0);


			glColor3f(lower_color_factor, 0.0f, 0.0f);
			glVertex3f(strt_y + 1, strt_x, 0);

			glColor3f(upper_color_factor, 0.0f, 0.0f);
			glVertex3f(strt_y - 1, strt_x, 0);
		}
		else {
			glVertex3f(strt_x, strt_y, 0);

			glColor3f(lower_color_factor, 0.0f, 0.0f);
			glVertex3f(strt_x, strt_y + 1, 0);
			
			glColor3f(upper_color_factor, 0.0f, 0.0f);
			glVertex3f(strt_x, strt_y - 1, 0);
	
		}
		glEnd();
	}
}

void reflectPointsCircle(int x, int y, int x_center, int y_center, float circle_size) {

	glBegin(GL_POINTS); 
	glPointSize(1);
	glVertex3f(x_center+x, y_center+y, 0);
	glVertex3f(x_center+y, y_center+x, 0);
	
	//For drawing semi circle we are eliminating this part.
	if(circle_size != 1/2) {

		glVertex3f(x_center-y, y_center+x, 0);
		glVertex3f(x_center-x, y_center+y, 0);
	
		glVertex3f(x_center-x, y_center-y, 0);
		glVertex3f(x_center-y, y_center-x, 0);
	}
	
	glVertex3f(x_center+y, y_center-x, 0);
	glVertex3f(x_center+x, y_center-y, 0);
	
	glEnd();
}

void ownCircleDraw(int x_center, int y_center, float radius, float full_circle) {

	int delta = 1 - radius;
	int x = 0 , y = radius;

	while(y>=x) {

		reflectPointsCircle(x ,y, x_center , y_center, full_circle);
		if(delta < 0) {
			delta = delta + ((2 * x)+3);
		}
		else {
			delta = delta + ((2*x)-(2*y)+5);
			y = y - 1;
		}
		x++;
	}

}

void ownFaceDraw(int x_center, int y_center) {


	if(start_x == 0 ||  start_y ==0) {

		start_x = windowWidth*1/2;
		start_y = windowHeight*1/2;
	}

	ownCircleDraw(start_x, start_y, 200, 1);
	
	//eyes
	ownCircleDraw(start_x + 70, start_y - 80, eyes_width, 1);
	ownCircleDraw(start_x - 70, start_y - 80, eyes_width, 1);
	
	//smile
	ownParabolaDraw(start_x , start_y + 130, smile, 10);

	smile = smile + 0.4;

	eyes_width = eyes_width + 0.16;

	if(smile >= 50){
		smile = 10;
	}

	if(eyes_width >= 17) {
		eyes_width = 1;
	}

	glutPostRedisplay();
}

void ownUPDraw(int x_center , int y_center) {
	ownParabolaDraw(x_center - 70, y_center + 100, 9 , 200);
	ownLineDraw(x_center + 70, y_center+100, x_center+70, y_center -100, 1);
	ownCircleDraw(x_center+70, y_center-50, 50, 1/2);
}

void drawLine(int flag) {

	int i,j;

	for(i=0; i<=num_of_objects; i++) {
			for(j=0; j<left_click_counts[i]-1; j++) {

				glViewport(0,0, windowWidth, windowHeight);
				ownLineDraw(x_points[i][j], y_points[i][j], x_points[i][j+1], y_points[i][j+1], flag);

				glViewport( windowWidth/2, 0, windowWidth, windowHeight);
				glBegin(GL_LINES);
				glVertex3f(x_points[i][j], y_points[i][j], 0.0);
				glVertex3f(x_points[i][j+1], y_points[i][j+1], 0.0);
				glEnd();
			}
		}	

		if(current_x!= -1 && current_y != -1) {

			glViewport(0, 0, windowWidth, windowHeight);
			ownLineDraw(start_x, start_y, current_x, current_y, flag);

			glViewport(windowWidth/2, 0 , windowWidth, windowHeight);
			glBegin(GL_LINES);
			glVertex3f(start_x, start_y , 0.0);
			glVertex3f(current_x, current_y, 0.0);
			glEnd(); 
		}
}

void display(void)
{   
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glViewport(0,0, windowWidth, windowHeight);

	if(condition == 1) {
		drawLine(1);
	}
	else if(condition == 2) {
		ownCircleDraw(start_x, start_y, 100, 1);
	}
	else if(condition == 3) {
		ownElipseDraw(start_x, start_y, 200, 100);
	}
	else if(condition ==4) {
		ownParabolaDraw(start_x, start_y, 60 , 300);
	}
	else if(condition == 5) {
		ownUPDraw(start_x, start_y);
	}
	else if(condition == 6) {
		ownFaceDraw(start_x, start_y);
	}
	else if(condition == 7 ){
		drawLine(3);
	}

	glFlush(); 
}



void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, windowHeight, 0, -1, 1 );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void handleMouseclick(int button, int state, int x, int y) {

	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON) {

			left_click_counts[num_of_objects]++;

			int count = left_click_counts[num_of_objects];

			//Even number of clicks we need to plot the line
			if(count > 1) {

				x_points[num_of_objects][count-2] = start_x;
				x_points[num_of_objects][count-1] = x;
				
				y_points[num_of_objects][count-2] = start_y;
				y_points[num_of_objects][count-1] = y;

				start_x = x;
				start_y = y;

			}
			//Odd number of clicks we have to store the line
			else {
				start_x = x;
				start_y = y;
			}
		}
		else if (button == GLUT_RIGHT_BUTTON) {
			num_of_objects++;
			current_x = -1;
			current_y = -1;
		}

		glutPostRedisplay();
	}
}

void handleKeypress(unsigned char key, int x, int y) {

	if (key == 27) {
		exit(0);     // escape key is pressed
	}
}

void handleMouseMove(int x, int y) {

	if(left_click_counts[num_of_objects] > 0) {
		current_x = x; 
		current_y = y;
		glutPostRedisplay();
	}
}

void menu(int value)
{
  switch(value) {

    case 1:
    	   condition = 1;
           break;
    case 2:
    	   condition = 2;
    	   break;
    case 3:
 		   condition = 3;
    	   break;
   	case 4: 
   		   condition = 4;
   	       break;
    case 5:
 		   condition = 5;
    	   break;
   	case 6:
 		   condition = 6;
    	   break;
   	case 7: 
   		   condition = 7;
   		   break;
    }   

    glutPostRedisplay();
}


void make_menu(void)
{ 
 
   /*MainMenu*/
  // main_menu 
   glutCreateMenu(menu);
   glutAddMenuEntry("Draw Dynamic Lines", 1); 
   glutAddMenuEntry("Draw Dotted Lines", 7);
   glutAddMenuEntry("Draw Circle", 2);
   glutAddMenuEntry("Draw Ellipse", 3);
   glutAddMenuEntry("Draw Parabola", 4);
   glutAddMenuEntry("Draw UP", 5);
   glutAddMenuEntry("Draw Face", 6);
    
   glutAttachMenu(GLUT_MIDDLE_BUTTON);
}



int main(int argc, char** argv)
{ 

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	
	w = glutGet(GLUT_SCREEN_WIDTH);
	h = glutGet(GLUT_SCREEN_HEIGHT);
	windowWidth = w * 2 / 3;
	windowHeight = h * 2 / 3;

	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

	glutCreateWindow("Graphics Assing 1");

	init();

	make_menu();

	glutMouseFunc(handleMouseclick);
	glutKeyboardFunc(handleKeypress);

	glutPassiveMotionFunc(handleMouseMove);

	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

