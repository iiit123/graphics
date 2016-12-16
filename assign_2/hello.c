#include<stdio.h>
#include <iostream>
#include<math.h>
#include <cmath>
#include <GLUT/glut.h>
#include<stdlib.h>

int start_x, start_y, current_x = -1, current_y = -1, x_points[1000], y_points[1000], num_of_objects=0, left_click_counts=0;
int w, h, windowHeight, windowWidth, condition = 1, ymax=0, xmax=0, ymin=0, xmin=0, polygon_flag = 0, cliping_x[1000], cliping_y[1000];
int c_count = 0, zoom_flag=0, dotted_window_ht=150, dotted_window_w = 150;
int vp_x[1000], vp_y[1000], temp_x[1000], temp_y[1000], temp_left_count=0, strech_right = 0;
int vpxmax, vpxmin, vpymax, vpymin, spanx=0, spany=0, a[10000][10000] = {0};

void initViewport() {
  vpymax = windowHeight/2 + dotted_window_ht/2;
  vpymin = windowHeight/2 - dotted_window_ht/2;
  vpxmax = windowWidth/2 + dotted_window_w/2;
  vpxmin = windowWidth/2 - dotted_window_w/2;
}

void changeWindowPoints() {

  ymax = windowHeight/2 + dotted_window_ht/2 + spany;
  ymin = windowHeight/2 - dotted_window_ht/2 + spany;
  xmax = windowWidth/2 + dotted_window_w/2 + strech_right + spanx;
  xmin = windowWidth/2 - dotted_window_w/2 + spanx;
}

void viewPort(int vp_xmin, int vp_ymin, int vp_xmax, int vp_ymax) {
   float scale_x, scale_y;

   float vp_x_diff = vp_xmax - vp_xmin;
   float vp_y_diff = vp_ymax - vp_ymin;
   float x_diff = xmax - xmin;
   float y_diff= ymax - ymin;
  

   scale_x = (vp_x_diff*1.0)/ x_diff;
   scale_y = (vp_y_diff*1.0)/ y_diff;

 //  printf("scale_x = %f scale_y = %f\n", scale_x, scale_y);


    for(int i=0; i<left_click_counts-1; i++) {
        vp_x[i] = vp_xmin+((x_points[i]-xmin)*scale_x);
        vp_y[i] = vp_ymin+((y_points[i]-ymin)*scale_y); 
    }

    vp_x[left_click_counts-1] = vp_x[0];
    vp_y[left_click_counts-1] = vp_y[0];
}

void createWindow() {

  changeWindowPoints();
  
	glPushAttrib(GL_ENABLE_BIT);
	glLineStipple(1, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);

	glBegin(GL_LINES);
		glVertex2f(xmin, ymin);
		glVertex2f(xmax, ymin);
	glEnd();

	glBegin(GL_LINES);
		glVertex2f(xmax, ymin);
		glVertex2f(xmax, ymax);
	glEnd();

	glBegin(GL_LINES);
		glVertex2f(xmin, ymin);
		glVertex2f(xmin, ymax);

	glEnd();
	
	glBegin(GL_LINES);
		glVertex2f(xmin, ymax);
		glVertex2f(xmax, ymax);
	glEnd();

	glPopAttrib();
}


void createWindowViewport(int left, int decrease) {

  glPushAttrib(GL_ENABLE_BIT);
  glLineStipple(1, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);

  glBegin(GL_LINES);
    glVertex2f(vpxmin+left+decrease, vpymin+decrease);
    glVertex2f(vpxmax+left-decrease, vpymin+decrease);
  glEnd();

  glBegin(GL_LINES);
    glVertex2f(vpxmax+left - decrease, vpymin+decrease);
    glVertex2f(vpxmax+left-decrease, vpymax-decrease);
  glEnd();

  glBegin(GL_LINES);
    glVertex2f(vpxmin+left+decrease, vpymin+decrease);
    glVertex2f(vpxmin+left+decrease, vpymax-decrease);

  glEnd();
  
  glBegin(GL_LINES);
    glVertex2f(vpxmin+left+decrease, vpymax-decrease);
    glVertex2f(vpxmax+left-decrease, vpymax-decrease);
  glEnd();

  glPopAttrib();
}

//check whether point is inside the rect or not.
int check_point(int x, int y) {

		if(x< xmax && x>xmin && y > ymin && y < ymax) {
			return 1;
		}	
		return 0;
}	

void reinit() {
	int i;

	left_click_counts = c_count;

	for(int i=0;i<c_count;i++) {
		x_points[i] = cliping_x[i];
		y_points[i] = cliping_y[i];
	}

	x_points[c_count] = x_points[0];
	y_points[c_count] = y_points[0];

  left_click_counts++;
  c_count=0;

}

void clip_l() {

	float slope = 1000;
  int i;

	for(i=0; i<left_click_counts-1; i++) {
		
		if(x_points[i+1]-x_points[i]) {
        	slope = (y_points[i+1]-y_points[i])*1.0/(x_points[i+1]-x_points[i]);
    }
		
		int intercept = y_points[i] + slope*(xmin-x_points[i]);

        //Right of the line. so we have to consider the points
    	if(x_points[i] >= xmin && x_points[i+1] >= xmin)
    	{
       	 	cliping_x[c_count] = x_points[i+1];
        	cliping_y[c_count] = y_points[i+1];
       	 	c_count++;
    	}
   		
   		//either side of the right boundry. get intersect and store the second point.
   		if(x_points[i] < xmin && x_points[i+1] >= xmin)
   		 {
        	cliping_x[c_count] = xmin;    // the x co-ordinate remains same for right boundry.
        	cliping_y[c_count] = intercept;
        	
        	cliping_x[c_count + 1] = x_points[i+1];
        	cliping_y[c_count + 1] = y_points[i+1];
        	
        	c_count = c_count + 2;
    	}
   
    	//either side of right boundry.  second point outisde the righr region so save only intersect region.
   		if(x_points[i] >= xmin  && x_points[i+1] < xmin)
    	{
        	cliping_x[c_count] = xmin; 			// the x co-ordinate remains same for right boundry.
        	cliping_y[c_count] = intercept;
        	c_count++;
    	}

	}

	reinit();
}	


void clip_t() {

	float slope = 1000;
  int i;

  for(i=0; i<left_click_counts-1; i++) {
    
    if(y_points[i+1]-y_points[i]) {
          slope = (x_points[i+1]-x_points[i])*1.0/(y_points[i+1]-y_points[i]);
    }
    
    int intercept = x_points[i] + slope*(ymax - y_points[i]);

        // printf("slope=%d i=%d intercept = %d, xmin=%d, xmax=%d ymin=%d ymax=%d x_points[i]=%d x_points[i+1]=%d y_points[i]=%d y_points[i+1]=%d\n",slope,i,intercept, xmin, xmax, ymin, ymax, x_points[i], x_points[i+1], y_points[i], y_points[i+1]);
        //Right of the line. so we have to consider the points
      if(y_points[i] <= ymax && y_points[i+1] <= ymax)
      {
          cliping_x[c_count] = x_points[i+1];
          cliping_y[c_count] = y_points[i+1];
          c_count++;
      }
      
      //either side of the right boundry. get intersect and store the second point.
      if(y_points[i] > ymax && y_points[i+1] <= ymax)
       {
          cliping_x[c_count] = intercept;    // the x co-ordinate remains same for right boundry.
          cliping_y[c_count] = ymax;
          
          cliping_x[c_count + 1] = x_points[i+1];
          cliping_y[c_count + 1] = y_points[i+1];
          
          c_count = c_count + 2;
      }
   
      //either side of right boundry.  second point outisde the righr region so save only intersect region.
      if(y_points[i] <= ymax  && y_points[i+1] > ymax)
      {
          cliping_x[c_count] = intercept;      // the x co-ordinate remains same for right boundry.
          cliping_y[c_count] = ymax;
          c_count++;
      }

  }

  reinit();
}

void clip_r() {
	float slope = 1000;
  int i;
	for(i=0; i<left_click_counts-1; i++) {
		
		if(x_points[i+1]-x_points[i]) {
        	slope = (y_points[i+1]-y_points[i])*1.0/(x_points[i+1]-x_points[i]);
        }

        int intercept = y_points[i] + slope*(xmax-x_points[i]);
        //Right of the line. so we have to consider the points
    	if(x_points[i] <= xmax && x_points[i+1] <= xmax)
    	{
       	 	cliping_x[c_count] = x_points[i+1];
        	cliping_y[c_count] = y_points[i+1];
       	 	c_count++;
    	}
   		
   		//either side of the right boundry. get intersect and store the second point.
   		if(x_points[i] > xmax && x_points[i+1] <= xmax)
   		 {
        	cliping_x[c_count] = xmax;    // the x co-ordinate remains same for right boundry.
        	cliping_y[c_count] = intercept;
        	
        	cliping_x[c_count + 1] = x_points[i+1];
        	cliping_y[c_count + 1] = y_points[i+1];
        	
        	c_count = c_count + 2;
    	}
   
    	//either side of right boundry.  second point outisde the righr region so save only intersect region.
   		if(x_points[i] <= xmax  && x_points[i+1] > xmax)
    	{
        	cliping_x[c_count] = xmax; 			// the x co-ordinate remains same for right boundry.
        	cliping_y[c_count] = intercept;
        	c_count+=1;
    	}

	}

	reinit();
}

void clip_b() {

	float slope = 1000;
  int i;

  for(i=0; i<left_click_counts-1; i++) {
    
    if(y_points[i+1]-y_points[i]) {
          slope = (x_points[i+1]-x_points[i])*1.0/(y_points[i+1]-y_points[i]);
    }
    
    int intercept = x_points[i] + slope*(ymin - y_points[i]);

//    printf("slope=%d i=%d intercept = %d, xmin=%d, xmax=%d ymin=%d ymax=%d x_points[i]=%d x_points[i+1]=%d y_points[i]=%d y_points[i+1]=%d\n",slope,i,intercept, xmin, xmax, ymin, ymax, x_points[i], x_points[i+1], y_points[i], y_points[i+1]);


        //Right of the line. so we have to consider the points
      if(y_points[i] >= ymin && y_points[i+1] >= ymin)
      {
          cliping_x[c_count] = x_points[i+1];
          cliping_y[c_count] = y_points[i+1];
          c_count++;
      }
      
      //either side of the right boundry. get intersect and store the second point.
      if(y_points[i] < ymin && y_points[i+1] >= ymin)
       {
          cliping_x[c_count] = intercept;    // the x co-ordinate remains same for right boundry.
          cliping_y[c_count] = ymin;
          
          cliping_x[c_count + 1] = x_points[i+1];
          cliping_y[c_count + 1] = y_points[i+1];
          
          c_count = c_count + 2;
      }
   
      //either side of right boundry.  second point outisde the righr region so save only intersect region.
      if(y_points[i] >= ymin  && y_points[i+1] < ymin)
      {
          cliping_x[c_count] = intercept;      // the x co-ordinate remains same for right boundry.
          cliping_y[c_count] = ymin;
          c_count++;
      }

  }

  reinit();
}

void boundry_fill(int x, int y) {
   
  unsigned char pixel[4];
  glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);


  if(((int)pixel[0] == 255 && (int)pixel[1] == 255 && (int)pixel[2] == 255) ||  a[x][y] == 1) {
    return;
  }
  else {
    
    a[x][y] = 1;
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
      glVertex2f(x,y);
    glEnd();
    glFlush();

    boundry_fill(x + 1,  y);
    boundry_fill(x - 1, y);
    boundry_fill(x, y + 1);
    boundry_fill(x, y - 1);
  }
}

void drawViewPortLine() {
  int i,j;
  for(i=0; i<left_click_counts-1; i++) {
          glBegin(GL_LINES);
              glVertex3f(vp_x[i], vp_y[i], 0.0);
              glVertex3f(vp_x[i+1],vp_y[i+1], 0.0);
          glEnd();
      }
}

void drawLine() {
  int i,j;

    
  for(i=0; i<left_click_counts-1; i++) {
          glBegin(GL_LINES);
              glVertex3f(x_points[i], y_points[i], 0.0);
              glVertex3f(x_points[i+1], y_points[i+1], 0.0);
          glEnd();
      }
   


    if(current_x!= -1 && current_y != -1) {
        glBegin(GL_LINES);
           glVertex3f(start_x, start_y , 0.0);
           glVertex3f(current_x, current_y, 0.0);
        glEnd();
    }
}

void init_points() {

    for(int i=0; i<=temp_left_count-1; i++) {
        x_points[i] = temp_x[i];
        y_points[i] = temp_y[i];
    }

    left_click_counts = temp_left_count;
}

void temp_storage() {
  temp_left_count = left_click_counts;

  for(int i=0; i<=left_click_counts-1; i++) {
     temp_x[i] = x_points[i];
     temp_y[i] = y_points[i];
  } 

}

void polygon_clipping() { 
	//making it circurlar

	clip_l();
	clip_t();
	clip_r();
	clip_b();

} 

void display(void)
{   
	
  glClear(GL_COLOR_BUFFER_BIT);

	if(condition == 1) {
    createWindow();
		drawLine();
	}
	else if(condition == 2) {  
     createWindow();
     temp_storage();
     polygon_clipping();
     drawLine();
     init_points();
	}
	else if(condition == 3) {
     createWindow();
     temp_storage();
     polygon_clipping();
     drawLine();
     boundry_fill((xmin+xmax)/2, (ymin+ymax)/2);
     init_points();
	}

  else if(condition == 4) {

      dotted_window_w = dotted_window_w - (zoom_flag)*10;
      dotted_window_ht = dotted_window_ht - (zoom_flag)*10;

      createWindow();
      temp_storage();
      polygon_clipping();
      drawLine();
      init_points();

      createWindowViewport(windowWidth/4, 30);
      temp_storage();
      polygon_clipping();
      viewPort(vpxmin + windowWidth/4 + 30, vpymin + 30, vpxmax+windowWidth/4 - 30, vpymax - 30);
      drawViewPortLine();
      init_points();
  }
  else if(condition == 5) {
      createWindow();
      temp_storage();
      polygon_clipping();
      drawLine();
      init_points();

      createWindowViewport(windowWidth/4, 30);
      temp_storage();
      polygon_clipping();
      viewPort(vpxmin + windowWidth/4 + 30, vpymin + 30, vpxmax+windowWidth/4 - 30, vpymax - 30);
      drawViewPortLine();
      init_points();
  }
  else if(condition == 6) {

      createWindow();
      temp_storage();
      polygon_clipping();
      drawLine();
      init_points();

      createWindowViewport(windowWidth/4, 30);
      temp_storage();
      polygon_clipping();
      viewPort(vpxmin + windowWidth/4 + 30, vpymin + 30, vpxmax+windowWidth/4 - 30, vpymax - 30);
      drawViewPortLine();
      init_points();
  }

	glFlush(); 
}



void init(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, windowHeight, 0, -1, 1 );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  glutSwapBuffers();

}

void handleMouseclick(int button, int state, int x, int y) {

	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON ) {

      if(polygon_flag == 0) {
    
      			left_click_counts++;

            int count = left_click_counts;

            //Even number of clicks we need to plot the line
           	if(count > 1) {

                x_points[count-2] = start_x;
                x_points[count-1] = x;
    
                y_points[count-2] = start_y;
                y_points[count-1] = y;

                start_x = x;
                start_y = y;

            }   
            //Odd number of clicks we have to store the line
            else {
                    start_x = x;
                    start_y = y;
            }
        }

        else{ 
              start_x = x;
              start_y = y;
        }

		}
		else if (button == GLUT_RIGHT_BUTTON) {

			polygon_flag = 1;

			x_points[left_click_counts] = x_points[0];
			y_points[left_click_counts] = y_points[0];
			left_click_counts++;


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

  if(key == 'z' && condition == 4) {

      zoom_flag = 0;
      zoom_flag++;

      glutPostRedisplay();

  }
  if(key == 'x' && condition == 4) {

      zoom_flag = 0;
      zoom_flag --;
      glutPostRedisplay();
  }

  if(key == 'c' && condition == 5) {

    strech_right = strech_right + 30;
    glutPostRedisplay();
  }
  
  if(key == 'v' && condition == 5) {
    strech_right = strech_right - 30;
    glutPostRedisplay();
  }


  if(key == 'a' && condition == 6) {
    spanx = spanx-30;
    glutPostRedisplay();
  }
  if(key == 'd' && condition == 6) {
    spanx = spanx + 30;
    glutPostRedisplay();
  }
  if(key == 'w' && condition == 6) {
     spany = spany - 30;
    glutPostRedisplay();
  }
  if(key == 's' && condition == 6) {
    spany = spany+30;
    glutPostRedisplay();
  }

}

void handleMouseMove(int x, int y) {
	if(left_click_counts > 0 && polygon_flag!=1) {
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
    }   

    glutPostRedisplay();
}


void make_menu(void)
{ 

   /*MainMenu*/
   // main_menu 
   glutCreateMenu(menu);
   glutAddMenuEntry("Polygon clipping", 2); 
   glutAddMenuEntry("Region filling", 3);
   glutAddMenuEntry("Window-to-Viewport Mapping along with zooming", 4);
   glutAddMenuEntry("stretch Right", 5);
   glutAddMenuEntry("span effect", 6);


    
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

  initViewport();

	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

	glutCreateWindow("Graphics Assing 2");

	init();

	make_menu();

	glutMouseFunc(handleMouseclick);
	glutKeyboardFunc(handleKeypress);

	glutPassiveMotionFunc(handleMouseMove);


	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

