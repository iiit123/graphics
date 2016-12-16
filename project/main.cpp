#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>   //  To use functions with variables arguments
#include <unistd.h>
#include <math.h>
#include <vector>

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>


#include "libraries/body.h"
#include "libraries/head.h"
#include "libraries/left_hand_down.h"
#include "libraries/left_hand_up.h"
#include "libraries/left_leg_down.h"
#include "libraries/left_leg_up.h"
#include "libraries/right_hand_down.h"
#include "libraries/right_hand_up.h"
#include "libraries/right_leg_down.h"
#include "libraries/right_leg_up.h"
#include "libraries/my_object_config.h"
#include "functions/globals.h"
#include "functions/blender.h"
#include "functions/addons.h"
#include "functions/headers.h"


int main(int argc, char** argv)
{
	SetupWorld();


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);

	w = glutGet(GLUT_SCREEN_WIDTH);
	h = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowSize(w, h);
	glutCreateWindow("Final Project");
	initRendering();

	for(int i=0;i<10;i++) {
      InitMesh(i);
    }

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(keyPressed);


	glutSpecialUpFunc(specialKeyRelease);

	glutSpecialFunc(specialKeyPressed);

	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);


	glutMainLoop();
	return 0;
}

void drawRobo(int robo_type, int rotate_robo) {
	glScalef(0.18, 0.18, 0.18);

	for(int i=0;i<10;i++){ 
      glPushMatrix();
      	 glShadeModel(GL_FLAT);
      	 glEnable(GL_TEXTURE_2D); // Enable texture mapping.
		 glBindTexture(GL_TEXTURE_2D, texture_robo[robo_type][2]); //
      	 glScalef(0.2,0.2,0.2);
         glRotatef(-90,1,0,0);
         glRotatef(rotate_robo,0,0,1);
         glMultMatrixd(origins[i]);
         DrawMesh(vbo[i], vinx[i], i);  
      glPopMatrix();
  	} 
}
 

void chooseCharacter() {

	rotate_robo = rotate_robo +1;
	glPushMatrix();
	glTranslatef(0.7,0,0);
	drawRobo(0, rotate_robo);
	glPopMatrix();

	glPushMatrix();
	drawRobo(1, rotate_robo);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.7,0,0);
	drawRobo(2, rotate_robo);
	glPopMatrix();
}


void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);

	if(start_flag == 0) {
		float x1, z1, d=1.3;
		int i, j;
		float xleft, zleft;

		z1 = -d * (cos(-_angle * PI / 180.0));
		x1 = d * sin(-_angle * PI / 180.0);
		gluLookAt(-xtrans-x1, 0.5+ytrans, -ztrans-z1, -xtrans, 0.09+ytrans, -ztrans, 0, 1, 0);
		chooseCharacter();
	}

	else if(start_flag == 1) {
	float x1, z1, d=1.3;
	int i, j;
	float xleft, zleft;

	z1 = -d * (cos(-_angle * PI / 180.0));
	x1 = d * sin(-_angle * PI / 180.0);


	if(view_mode==0)	// First Person Mode
		gluLookAt(-xtrans, ytrans+0.2, -ztrans, -xtrans+x1, ytrans+0.2+lookup, -ztrans+z1, 0, 1, 0);
	else if(view_mode==1)	// Third Person Mode
	{
		gluLookAt(-xtrans-x1, 0.5+ytrans, -ztrans-z1, -xtrans, 0.09+ytrans, -ztrans, 0, 1, 0);
	}
	else if(view_mode==2)	// Tower Mode
		gluLookAt(-xcenter, 3 , -zcenter, -xtrans, ytrans, -ztrans, 0, 1, 0);
	else if(view_mode==3) 	// Helicopter Mode
		gluLookAt(-xcenter + zoom_x, 3 , -zcenter + zoom_z, 0, z_look_helicopter/20 + z_helicopter/10.0, x_look_helicopter/20 + x_helicopter/10.0, 0, 1, 0);

	glPushMatrix();



	if(ztrans == 0)
		ztrans = d;

	glRotatef(180.0, 0, 1, 0);
	glTranslatef(xtrans, ytrans, ztrans);
	glRotatef(_angle, 0.0, 1.0, 0.0);

	drawRobo(robo_type, 0);

	glPopMatrix();

	DrawGLScene();
	}

	glutSwapBuffers();
}

void update(int value)
{

	if(current_id-2 < 25)
	{
		mapi = (current_id-2)/5;
		mapj = (current_id-2)%5;
	}
	if(current_id == 29)
	{
		xtrans += (sector1.triangle[2*current_id].vertex[0].x - sector1.triangle[2*current_id + 10].vertex[0].x);
		current_id +=5;
	}
	if(current_id == 34)
	{
		xtrans += (sector1.triangle[2*current_id].vertex[0].x - sector1.triangle[2*current_id + 10].vertex[0].x);
		current_id +=5;
	}
	if(current_id == 39)
	{
		xtrans += (sector1.triangle[2*current_id].vertex[0].x - sector1.triangle[2*current_id + 10].vertex[0].x);
		current_id +=5;
	}
	time(&now);

	elapsed = difftime(now, begin);
	int i;
	vert_collisions();


	connecting_dist += connecting_offset;
	if(connecting_dist > tile_dist-1 || connecting_dist < -1.0)
	{
		connecting_offset*=-1;
		disc_angle_offset = 0;
	}

	if(connecting_dist > 0 && connecting_dist < tile_dist-2)
	{
		disc_angle_offset = 0.005;
		create_connecting(id1, indx1, 0, connecting_dist, 1);
		create_connecting(id2, indx2, 1, connecting_dist, 1);
	}
	create_support(id3, indx3, 1);

	if(!jump_flag)
	{
		if(current_id >= id1 && current_id < id2)
		{
			if(connecting_dist > 0 && connecting_dist < tile_dist - 2)
			{
				ztrans -= connecting_offset;
				//	_angle = disc_angle;

			}
		}
		if(current_id >= id2 && current_id < (id2 + (n)*(m)) )
		{
			if(connecting_dist > 0 && connecting_dist < tile_dist - 2)
			{
				xtrans -= connecting_offset;
				//	_angle = disc_angle;

			}
		}
	}
	if(fall_flag==0)
	{
		while(inside_rectangle_check(current_id)==0)
		{
			fall_dist = ytrans - terrain_collision();
			if(fall_dist > 0.001)
			{
				fall_flag = 1;
			}
			else
			{
				fall_flag = 0;
				ytrans = ytrans_old;
			}
			tjump = 0;
			jump_flag = 0;
		}
		// For tiles moving up and down
		if(!fall_flag && inside_rectangle_check(current_id) && !jump_flag )
			ytrans = sector1.triangle[current_id*2].vertex[0].y + 0.2;

	}
	if(fall_flag==1)
	{
		tjump += 0.07;
		ytrans -= 0.05*tjump;
		fall_dist -= 0.05*tjump;
		direction = 1;
		if(fall_dist < 0.3)
		{
			direction = 0;
			fall_flag=0;
			ytrans_old = ytrans;

			if(current_id ==0) // TODO : HANDLE JUMP IN WATER
			{
				ytrans -= 0.2;
				sleep(1);
				exit(0);
			}
		}
	}
	if(jump_flag!=1)
	{
		ytrans_old = ytrans;
	}
	float term;
	if(jump_flag==1)
	{
		tjump += 0.07;
		yjump = 0.4 * tjump - 0.1 * tjump * tjump + ytrans_old;
		ytrans = yjump;
		fall_flag=2;
		term = ytrans_old;

		if(current_id != 0)
		{
			for(i=4; i<sector1.numtriangles; i++)
			{
				if(sector1.triangle[i].id == current_id)
					continue;
				if(inside_rectangle_check(sector1.triangle[i].id))
				{
					if(sector1.triangle[i].vertex[0].y+0.2 > term)
						term = sector1.triangle[i].vertex[0].y+0.2;
				}
			}
		}
		if(yjump < term)
		{
			yjump = 0;
			tjump = 0;
			jump_flag = 0;
			fall_flag=0;
			ytrans = term;
			direction = 0;
			offset = 2;
		}
	}

	if (_angle > 360)
	{
		_angle -= 360;
	}

	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

void SetupWorld()
{

	float x, y, z, u, v;
	int vert,localtex;
	FILE *filein;        
	char oneline[255];

	filein = fopen("Data/world2.txt", "rt");

	readstr(filein, oneline);
	sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);

	sector1.numtriangles = numtriangles;
	sector1.triangle = (TRIANGLE *) malloc(sizeof(TRIANGLE)*10000);


	for (loop = 0; loop < (int)numtriangles; loop++)
	{
		for (vert = 0; vert < 3; vert++) {
			readstr(filein,oneline);
			sscanf(oneline, "%d %f %f %f %f %f",&localtex, &x, &y, &z, &u, &v);
			sector1.triangle[loop].tex=localtex;
			sector1.triangle[loop].vertex[vert].x = x;
			sector1.triangle[loop].vertex[vert].y = y;
			sector1.triangle[loop].vertex[vert].z = z;
			sector1.triangle[loop].vertex[vert].u = u;
			sector1.triangle[loop].vertex[vert].v = v;

			sector1.triangle[loop].id = loop/2;

		}
	}
	fclose(filein);
	int i=0, j=0,id = 2, indx;

	float xleft, zleft;
	// Floors

	for(i=0; i<n; i++)
	{
		for(j=0; j<m; j++)
		{
			y = 0;
			xleft = 1.0 * tile_width * i - tile_width / 2.0 + i * tile_dist;
			zleft = 1.0 * tile_height * j - tile_height / 2.0 + j * tile_dist;

			if(i==n/2 && j==m/2)
			{
				xcenter = -(xleft + tile_width /2.0);
				zcenter = -(zleft + tile_height /2.0);
			}

			if(id==current_id)
			{
				xtrans = -(xleft + tile_width /2.0);
				ztrans = -(zleft + tile_height /2.0);
				ytrans = y+0.2;
				ytrans_old = ytrans;

			}

			indx = 4 + i*2*n + 2*j;
			if(i==7 && j==6)
			{

				sector1.triangle[indx].tex = 7;
			}
			else
				sector1.triangle[indx].tex = 1;

			sector1.triangle[indx].id = id;

			sector1.triangle[indx].vertex[0].x = xleft;
			sector1.triangle[indx].vertex[0].z = zleft;
			sector1.triangle[indx].vertex[0].y = y;

			sector1.triangle[indx].vertex[0].u = 0;
			sector1.triangle[indx].vertex[0].v = 12;


			sector1.triangle[indx].vertex[1].x = xleft;
			sector1.triangle[indx].vertex[1].z = zleft+ tile_height;
			sector1.triangle[indx].vertex[1].y = y;

			sector1.triangle[indx].vertex[1].u = 0;
			sector1.triangle[indx].vertex[1].v = 0;


			sector1.triangle[indx].vertex[2].x = xleft+tile_width;
			sector1.triangle[indx].vertex[2].z = zleft+tile_height;
			sector1.triangle[indx].vertex[2].y = y;

			sector1.triangle[indx].vertex[2].u = 12;
			sector1.triangle[indx].vertex[2].v = 0;

			indx = 4+ i*2*n+2*j+1;

			if(i==7 && j==6)
				sector1.triangle[indx].tex = 7;
			else
				sector1.triangle[indx].tex = 1;
			sector1.triangle[indx].id = id;

			sector1.triangle[indx].vertex[0].x = xleft;
			sector1.triangle[indx].vertex[0].z = zleft;
			sector1.triangle[indx].vertex[0].y = y;

			sector1.triangle[indx].vertex[0].u = 0;
			sector1.triangle[indx].vertex[0].v = 12;


			sector1.triangle[indx].vertex[1].x = xleft+tile_width;
			sector1.triangle[indx].vertex[1].z = zleft;
			sector1.triangle[indx].vertex[1].y = y;

			sector1.triangle[indx].vertex[1].u = 0;
			sector1.triangle[indx].vertex[1].v = 0;


			sector1.triangle[indx].vertex[2].x = xleft+tile_width;
			sector1.triangle[indx].vertex[2].z = zleft+tile_height;
			sector1.triangle[indx].vertex[2].y = y;

			sector1.triangle[indx].vertex[2].u = 12;
			sector1.triangle[indx].vertex[2].v = 0;

			sector1.numtriangles+=2;
			numtriangles+=2;
			id++;


		}
	}
	id1 = id;
	indx1 = indx;

	create_connecting(id1, indx1, 0, 0, 0);
	create_connecting(id2, indx2, 1, 0, 0);

	create_support(id3, indx3, 0);

	filein = fopen("Data/world3.txt", "rt");

	readstr(filein, oneline);
	sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);

	for (loop = sector1.numtriangles; loop < sector1.numtriangles + (int)numtriangles; loop++)
	{
		for (vert = 0; vert < 3; vert++) {
			readstr(filein,oneline);
			sscanf(oneline, "%d %f %f %f %f %f",&localtex, &x, &y, &z, &u, &v);
			sector1.triangle[loop].tex=localtex;
			sector1.triangle[loop].vertex[vert].x = x;
			sector1.triangle[loop].vertex[vert].y = y;
			sector1.triangle[loop].vertex[vert].z = z;
			sector1.triangle[loop].vertex[vert].u = u;
			sector1.triangle[loop].vertex[vert].v = v;

			sector1.triangle[loop].id = loop/2;

		}
	}
	int ref = sector1.numtriangles;
	fclose(filein);
	int start;
	for(i=0; i<n; i++)
	{
		for(j=0; j<m; j++)
		{
			start = sector1.numtriangles;
			xleft = 1.0 * tile_width * i - tile_width / 2.0 + i * tile_dist + tile_height/2;
			zleft = 1.0 * tile_height * j - tile_height / 2.0 + j * tile_dist + tile_width/2;

			for (loop = 0; loop <  (int)numtriangles; loop++)
			{
				for (vert = 0; vert < 3; vert++) {
					sector1.triangle[start + loop].tex=sector1.triangle[ref + loop].tex;
					sector1.triangle[start + loop].vertex[vert].x = sector1.triangle[ref + loop].vertex[vert].x + xleft;
					sector1.triangle[start + loop].vertex[vert].y = sector1.triangle[ref + loop].vertex[vert].y;
					sector1.triangle[start + loop].vertex[vert].z = sector1.triangle[ref + loop].vertex[vert].z + zleft;
					sector1.triangle[start + loop].vertex[vert].u = sector1.triangle[ref + loop].vertex[vert].u;
					sector1.triangle[start + loop].vertex[vert].v = sector1.triangle[ref + loop].vertex[vert].v;
					sector1.triangle[start + loop].id = (loop+ start)/2;

				}
			}
			sector1.numtriangles += numtriangles;
		}
	}
	
	time(&begin);
	return;
}


int inside_rectangle_check(int id)
{
	float xleft, xright, zdown, zup;
	int i, j;
	for(i = 2*id; i< 2*id+2; i++)
	{
		for(j=0; j<3; j++)
		{
			if(i==2*id && j==0)
			{
				xleft = sector1.triangle[i].vertex[j].x ;
				zdown = sector1.triangle[i].vertex[j].z;
				xright = sector1.triangle[i].vertex[j].x ;
				zup = sector1.triangle[i].vertex[j].z;
			}
			else
			{
				if(sector1.triangle[i].vertex[j].x < xleft)
				{
					xleft = sector1.triangle[i].vertex[j].x;
				}
				if(sector1.triangle[i].vertex[j].x > xright)
				{
					xright = sector1.triangle[i].vertex[j].x;
				}

				if(sector1.triangle[i].vertex[j].z < zdown)
				{
					zdown = sector1.triangle[i].vertex[j].z;
				}
				if(sector1.triangle[i].vertex[j].z > zup)
				{
					zup = sector1.triangle[i].vertex[j].z;
				}
			}
		}
	}
	if( ((-xtrans) < xright) && ((-xtrans) > xleft) && (-ztrans < zup) && (-ztrans > zdown))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
float terrain_collision()
{
	float maxx, temp;
	ytrans -=0.2;
	int i, vert, flag=0, vflag=0;

	for(i=0; i<sector1.numtriangles; i++)
	{
		vflag=0;
		for (vert = 1; vert < 3; vert++)
		{
			if(sector1.triangle[i].vertex[vert].y != sector1.triangle[i].vertex[vert-1].y)
			{
				vflag=1;
				break;
			}
		}
		if(vflag==1)
		{
			continue;
		}

		if(inside_rectangle_check(sector1.triangle[i].id) == 0)
			continue;

		for (vert = 0; vert < 3; vert++)
		{
			if(sector1.triangle[i].id == current_id)
				continue;
			if( sector1.triangle[i].vertex[vert].y < ytrans + 0.1)
			{
				temp =  sector1.triangle[i].vertex[vert].y;

				if(flag==0)
				{
					maxx = temp;
					flag=1;
					current_id = sector1.triangle[i].id;
				}
				else
				{
					if(temp > maxx)
					{
						maxx = temp;
						current_id = sector1.triangle[i].id;
					}
				}
			}
		}
	}
	if(flag==0)
		maxx=ytrans;
	return maxx+0.2;
}
void vert_collisions()
{
	int xflag = 0, zflag = 0, yflag = 0, i;
	for(i=0; i<sector1.numtriangles; i++)
	{
		xflag = 0; yflag = 0; zflag = 0;

		if(sector1.triangle[i].vertex[0].y == sector1.triangle[i].vertex[1].y)
		{
			if(sector1.triangle[i].vertex[1].y == sector1.triangle[i].vertex[2].y)
				yflag = 1;
		}
		if(sector1.triangle[i].vertex[0].x == sector1.triangle[i].vertex[1].x)
		{
			if(sector1.triangle[i].vertex[1].x == sector1.triangle[i].vertex[2].x)
				xflag = 1;
		}
		if(sector1.triangle[i].vertex[0].z == sector1.triangle[i].vertex[1].z)
		{
			if(sector1.triangle[i].vertex[1].z == sector1.triangle[i].vertex[2].z)
				zflag = 1;
		}
		if(yflag)
			continue;
		if(xflag)
		{
			if(inside_rectangle_check_x(sector1.triangle[i].id))
			{
				if(-sector1.triangle[i].vertex[0].x - xtrans > 0 && -sector1.triangle[i].vertex[0].x - xtrans < 0.2)
				{
					xtrans = -sector1.triangle[i].vertex[0].x - 0.22;
				}
				if(xtrans + sector1.triangle[i].vertex[0].x > 0 && xtrans + sector1.triangle[i].vertex[0].x < 0.2)
				{
					xtrans = -sector1.triangle[i].vertex[0].x + 0.22;
				}
			}
		}
		if(zflag)
		{

			if(inside_rectangle_check_z(sector1.triangle[i].id))
			{
				if(-sector1.triangle[i].vertex[0].z - ztrans > 0 && -sector1.triangle[i].vertex[0].z - ztrans < 0.2)
				{
					ztrans = -sector1.triangle[i].vertex[0].z - 0.22;
				}
				if(ztrans + sector1.triangle[i].vertex[0].z > 0 && ztrans + sector1.triangle[i].vertex[0].z < 0.2)
				{
					ztrans = -sector1.triangle[i].vertex[0].z + 0.22;
				}
			}
		}
	}

}
int inside_rectangle_check_x(int id)
{
	float yleft, yright, zdown, zup;
	int i, j;
	for(i = 2*id; i< 2*id+2; i++)
	{
		for(j=0; j<3; j++)
		{
			if(i==2*id && j==0)
			{
				yleft = sector1.triangle[i].vertex[j].y;
				zdown = sector1.triangle[i].vertex[j].z;
				yright = sector1.triangle[i].vertex[j].y;
				zup = sector1.triangle[i].vertex[j].z;
			}
			else
			{
				if(sector1.triangle[i].vertex[j].y < yleft)
				{
					yleft = sector1.triangle[i].vertex[j].y;
				}
				if(sector1.triangle[i].vertex[j].y > yright)
				{
					yright = sector1.triangle[i].vertex[j].y;
				}

				if(sector1.triangle[i].vertex[j].z < zdown)
				{
					zdown = sector1.triangle[i].vertex[j].z;
				}
				if(sector1.triangle[i].vertex[j].z > zup)
				{
					zup = sector1.triangle[i].vertex[j].z;
				}
			}
		}
	}
	if( ((ytrans) < yright) && ((ytrans) > yleft) && (-ztrans < zup) && (-ztrans > zdown))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int inside_rectangle_check_z(int id)
{
	float yleft, yright, xdown, xup;
	int i, j;
	for(i = 2*id; i< 2*id+2; i++)
	{
		for(j=0; j<3; j++)
		{
			if(i==2*id && j==0)
			{
				yleft = sector1.triangle[i].vertex[j].y;
				xdown = sector1.triangle[i].vertex[j].x;
				yright = sector1.triangle[i].vertex[j].y;
				xup = sector1.triangle[i].vertex[j].x;
			}
			else
			{
				if(sector1.triangle[i].vertex[j].y < yleft)
				{
					yleft = sector1.triangle[i].vertex[j].y;
				}
				if(sector1.triangle[i].vertex[j].y > yright)
				{
					yright = sector1.triangle[i].vertex[j].y;
				}

				if(sector1.triangle[i].vertex[j].x < xdown)
				{
					xdown = sector1.triangle[i].vertex[j].x;
				}
				if(sector1.triangle[i].vertex[j].x > xup)
				{
					xup = sector1.triangle[i].vertex[j].x;
				}
			}

		}

	}
	if( ((ytrans) < yright) && ((ytrans) > yleft) && (-xtrans < xup) && (-xtrans > xdown))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void specialKeyPressed(int key, int x, int y)
{
	/* avoid thrashing this procedure */
	usleep(100);
	switch (key) {
		case GLUT_KEY_UP: // walk forward (bob head)

			direction = 1;
			dist = 0.1f;
			ztrans += dist * cos(_angle * PI / 180.0);
			xtrans += dist * sin(_angle * PI / 180.0);
			roboWalk();
			break;

		case GLUT_KEY_DOWN: // walk back (bob head)
			direction = 1;
			dist = -0.05f;
			ztrans += dist * cos(_angle * PI / 180.0);
			xtrans += dist * sin(_angle * PI / 180.0);
			roboWalk();
			break;

		case GLUT_KEY_LEFT: // look left
			_angle += 3;
			if(_angle >= 360)
				_angle -=360;
			break;

		case GLUT_KEY_RIGHT: // look right
			_angle -= 3;
			if(_angle >= 360)
				_angle -=360;
			break;

		default:
			printf ("Special key %d pressed. No action there yet.\n", key);
			break;
	}
}
void keyPressed(unsigned char key, int x, int y)
{
	/* avoid thrashing this procedure */
	usleep(100);

	if(start_flag == 0) {
		if(key == '1') {
			printf("hello\n");
			robo_type = 0;
			start_flag =1;
			view_mode = 1;
			glutPostRedisplay();
		}
		else if(key == '2'){
			robo_type = 1;
			start_flag = 1;
			view_mode = 1;
			glutPostRedisplay();
		}
		else if(key == '3') {
			robo_type = 2;
			start_flag = 1;
			view_mode = 1;
			glutPostRedisplay();
		}
	}

	switch (key) {
		case ESCAPE: // kill everything.
			/* exit the program...normal termination. */
			exit(1);
			break; // redundant.

		case 'v':
		case 'V':
			view_mode++;
			view_mode%=4;
			break;
		case 'j':
		case 'J':
			if(fall_flag==0)
			{
				jump_flag = 1;
				yjump = 0;
				tjump = 0;
				direction = 5;
				offset = 5;

			}
			break;
		case 'w':
		case 'W':
			lookup += 0.1;
			break;
		case 's':
		case 'S':
			lookup -= 0.1;
			break;
		case 'z':
		case 'Z':
			if(view_mode == 3)
			{
				zoom_z += 0.1;
				zoom_x += 0.1;
			}
			break;
		case 'x':
		case 'X':
			if(view_mode == 3)
			{
				zoom_z -= 0.1;
				zoom_x -= 0.1;
			}
			break;
		
		default:
			break;
	}
}

void specialKeyRelease (int key, int x, int y)
{
	if ( key == GLUT_KEY_UP || key == GLUT_KEY_DOWN )
	{
		direction = 0;
	}
}


void initRendering()
{
	LoadGLTextures();

	glEnable(GL_TEXTURE_2D);                    // Enable texture mapping.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);          // Set the blending function for translucency (note off at init time)
	glClearColor(115.0/255.0, 210.0/255.0, 249.0/255.0, 0.0f);	// This Will Clear The Background Color To Black
	//	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);	// This Will Clear The Background Color To Black
	glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);                       // type of depth test to do.
	glEnable(GL_DEPTH_TEST);                    // enables depth testing.
	glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// Reset The Projection Matrix

	gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_LIGHTING);
	// set up lights.
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	//	glEnable(GL_LIGHT2);

	quad = gluNewQuadric ();

}

void handleResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 0.3, 200.0);
	glMatrixMode(GL_MODELVIEW);
}

void create_support(int id, int indx, int flag2)
{
	int i, j;
	float xleft, zleft;
	for(i=0; i<n; i++)
	{
		for(j=0; j<m; j++)
		{
			xleft = 1.0 * tile_width * i - tile_width / 2.0 + i * tile_dist;
			zleft = 1.0 * tile_height * j - tile_height / 2.0 + j * tile_dist;



			indx = add_to_triangles(xleft, zleft, indx, id-1, 0);
			id++;

			indx = add_to_triangles(xleft + tile_height, zleft, indx, id-1, 0);
			id++;

			indx = add_to_triangles(xleft, zleft, indx, id-1, 1);
			id++;

			indx = add_to_triangles(xleft , zleft + tile_width, indx, id-1, 1);


			if(flag2==0)
			{
				sector1.numtriangles+=8;
				numtriangles+=8;
			}

		}
	}
}

int add_to_triangles(float xleft, float zleft, int indx, int id, int flag)
{
	float offset1, offset2;
	if(flag==0)
	{
		offset1 = xleft;
		offset2 = zleft + tile_width;
	}
	else if(flag==1)
	{
		offset1 = xleft + tile_width;
		offset2 = zleft;

	}
	indx++;
	sector1.triangle[indx].tex = 6;
	sector1.triangle[indx].id = id;

	sector1.triangle[indx].vertex[0].x = xleft;
	sector1.triangle[indx].vertex[0].z = zleft;
	sector1.triangle[indx].vertex[0].y = 0;
	sector1.triangle[indx].vertex[0].u = 0;
	sector1.triangle[indx].vertex[0].v = 6;

	sector1.triangle[indx].vertex[1].x = offset1;
	sector1.triangle[indx].vertex[1].z = offset2;
	sector1.triangle[indx].vertex[1].y = 0;
	sector1.triangle[indx].vertex[1].u = 0;
	sector1.triangle[indx].vertex[1].v = 0;

	sector1.triangle[indx].vertex[2].x = xleft;
	sector1.triangle[indx].vertex[2].z = zleft;
	sector1.triangle[indx].vertex[2].y = -10;
	sector1.triangle[indx].vertex[2].u = 6;
	sector1.triangle[indx].vertex[2].v = 0;

	indx++;
	sector1.triangle[indx].tex = 6;
	sector1.triangle[indx].id = id;

	sector1.triangle[indx].vertex[0].x = offset1;
	sector1.triangle[indx].vertex[0].z = offset2;
	sector1.triangle[indx].vertex[0].y = 0;
	sector1.triangle[indx].vertex[0].u = 0;
	sector1.triangle[indx].vertex[0].v = 6;

	sector1.triangle[indx].vertex[1].x = xleft;
	sector1.triangle[indx].vertex[1].z = zleft;
	sector1.triangle[indx].vertex[1].y = -10;
	sector1.triangle[indx].vertex[1].u = 0;
	sector1.triangle[indx].vertex[1].v = 0;

	sector1.triangle[indx].vertex[2].x = offset1;
	sector1.triangle[indx].vertex[2].z = offset2;
	sector1.triangle[indx].vertex[2].y = -10;
	sector1.triangle[indx].vertex[2].u = 6;
	sector1.triangle[indx].vertex[2].v = 0;
	return indx;

}

void create_connecting(int id, int indx, int flag, float offset, int flag2)
{
	int i=0, j=0, rand1, rand2;
	float xleft, zleft, y;
	for(i=0; i<n; i++)
	{
		for(j=0; j<m; j++)
		{
			if(flag2==0)
			{
				rand1 = rand()%100;
				rand2 = rand()%2;
				if(rand2 == 0)
					rand1*=-1;
				y = rand1/250.0;
			}
			xleft = 1.0 * tile_width * i - tile_width / 2.0 + i * tile_dist ;
			zleft = 1.0 * tile_height * j - tile_height / 2.0 + j * tile_dist ;
			if(flag2==0)
			{
				offset = rand() % 400;
				offset/=100;
			}
			if(flag==0)
			{
				xleft = xleft + tile_width/2 - 1;
				zleft = zleft + tile_height + offset;
			}
			else if(flag==1)
			{
				xleft = xleft + tile_width + offset;
				zleft = zleft + tile_height/2 - 1;
			}

			indx++;

			sector1.triangle[indx].tex = 5;
			sector1.triangle[indx].id = id;

			sector1.triangle[indx].vertex[0].x = xleft;
			sector1.triangle[indx].vertex[0].z = zleft;
			if(!flag2)
				sector1.triangle[indx].vertex[0].y = y;

			sector1.triangle[indx].vertex[0].u = 0;
			sector1.triangle[indx].vertex[0].v = 6;

			sector1.triangle[indx].vertex[1].x = xleft;
			sector1.triangle[indx].vertex[1].z = zleft+ tile_height/3;
			if(!flag2)
				sector1.triangle[indx].vertex[1].y = y;

			sector1.triangle[indx].vertex[1].u = 0;
			sector1.triangle[indx].vertex[1].v = 0;

			sector1.triangle[indx].vertex[2].x = xleft+tile_width/3;
			sector1.triangle[indx].vertex[2].z = zleft+tile_height/3;
			if(!flag2)
				sector1.triangle[indx].vertex[2].y = y;

			sector1.triangle[indx].vertex[2].u = 6;
			sector1.triangle[indx].vertex[2].v = 0;

			indx++;

			sector1.triangle[indx].tex = 5;
			sector1.triangle[indx].id = id;

			sector1.triangle[indx].vertex[0].x = xleft;
			sector1.triangle[indx].vertex[0].z = zleft;
			if(!flag2)
				sector1.triangle[indx].vertex[0].y = y;

			sector1.triangle[indx].vertex[0].u = 0;
			sector1.triangle[indx].vertex[0].v = 6;

			sector1.triangle[indx].vertex[1].x = xleft+tile_width/3;
			sector1.triangle[indx].vertex[1].z = zleft;
			if(!flag2)
				sector1.triangle[indx].vertex[1].y = y;

			sector1.triangle[indx].vertex[1].u = 0;
			sector1.triangle[indx].vertex[1].v = 0;

			sector1.triangle[indx].vertex[2].x = xleft+tile_width/3;
			sector1.triangle[indx].vertex[2].z = zleft+tile_height/3;
			if(!flag2)
				sector1.triangle[indx].vertex[2].y = y;

			sector1.triangle[indx].vertex[2].u = 6;
			sector1.triangle[indx].vertex[2].v = 0;

			if(flag2==0)
			{

				sector1.numtriangles+=2;
				numtriangles+=2;
			}
			id++;
		}
	}
	if(!flag)
	{
		indx2 = indx;
		id2 = id;
	}
	else
	{
		indx3 = indx;
		id3 = id;
	}
}

GLvoid DrawGLScene()
{
	glPushMatrix();
	//glDisable(GL_LIGHTING);
	//glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
	glTranslatef(-50, 6, -90);
	glColor3f(1, 1, 1);
	glColor3f(255.0/255.0, 255.0/255.0, 0.0/255.0);

	gluDisk (quad, 0.0, 3, 18, 1);

	//	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1, 1, 1);
	//glDisable(GL_COLOR_MATERIAL);
	//	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();

	GLfloat x_m, y_m, z_m, u_m, v_m;
	int numtriangles;

	numtriangles = sector1.numtriangles;

	for (loop=0; loop<(int)numtriangles; loop++) {        // loop through all the triangles
		if(sector1.triangle[loop].tex==1)
		{
			glEnable(GL_TEXTURE_2D); // Enable texture mapping.
			glBindTexture(GL_TEXTURE_2D, texture[filter]); // 2d texture (x and y size)
		}
		else if(sector1.triangle[loop].tex==2)
		{
			glEnable(GL_TEXTURE_2D); // Enable texture mapping.
			glBindTexture(GL_TEXTURE_2D, texture2[2]); // 2d texture (x and y size)
		}
		else if(sector1.triangle[loop].tex==3)
		{
			glEnable(GL_TEXTURE_2D); // Enable texture mapping.
			glBindTexture(GL_TEXTURE_2D, texture3[filter]); // pick the texture.
		}
		else if(sector1.triangle[loop].tex==4)
		{
			glEnable(GL_TEXTURE_2D); // Enable texture mapping.
			glBindTexture(GL_TEXTURE_2D, texture4[2]); // 2d texture (x and y size)
		}

		else if(sector1.triangle[loop].tex==5)
		{
			//	glEnable(GL_TEXTURE_2D); // Enable texture mapping.
			//	glBindTexture(GL_TEXTURE_2D, texture5[2]); // 2d texture (x and y size)
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_COLOR_MATERIAL);
			glColor3f(85/255.0, 85/255.0, 85/255.0);

		}

		else if(sector1.triangle[loop].tex==6)
		{
			glEnable(GL_TEXTURE_2D); // Enable texture mapping.
			if((sector1.triangle[loop].id)%2==0)
				glBindTexture(GL_TEXTURE_2D, texture6[2]); // 2d texture (x and y size)
			else
				glBindTexture(GL_TEXTURE_2D, texture6[0]); // 2d texture (x and y size)
		}

		else if(sector1.triangle[loop].tex==7)
		{
			glEnable(GL_TEXTURE_2D); // Enable texture mapping.
			glBindTexture(GL_TEXTURE_2D, texture7[filter]); // 2d texture (x and y size)
		}

		if(sector1.triangle[loop].tex!=5)
		{
			glBegin(GL_TRIANGLES);
			glNormal3f( 0.0f, 0.0f, 1.0f);

			x_m = sector1.triangle[loop].vertex[0].x;
			y_m = sector1.triangle[loop].vertex[0].y;
			z_m = sector1.triangle[loop].vertex[0].z;
			u_m = sector1.triangle[loop].vertex[0].u;
			v_m = sector1.triangle[loop].vertex[0].v;
			glTexCoord2f(u_m,v_m);
			glVertex3f(x_m,y_m,z_m);

			x_m = sector1.triangle[loop].vertex[1].x;
			y_m = sector1.triangle[loop].vertex[1].y;
			z_m = sector1.triangle[loop].vertex[1].z;
			u_m = sector1.triangle[loop].vertex[1].u;
			v_m = sector1.triangle[loop].vertex[1].v;
			glTexCoord2f(u_m,v_m);
			glVertex3f(x_m,y_m,z_m);

			x_m = sector1.triangle[loop].vertex[2].x;
			y_m = sector1.triangle[loop].vertex[2].y;
			z_m = sector1.triangle[loop].vertex[2].z;
			u_m = sector1.triangle[loop].vertex[2].u;
			v_m = sector1.triangle[loop].vertex[2].v;
			glTexCoord2f(u_m,v_m);
			glVertex3f(x_m,y_m,z_m);

			glEnd();
		}
		else
		{
			glPushMatrix();
			glTranslatef(sector1.triangle[loop].vertex[0].x, sector1.triangle[loop].vertex[0].y, sector1.triangle[loop].vertex[0].z);
			glTranslatef(tile_width/6, 0, tile_height/6);
			glRotatef(90, 1, 0, 0);
			disc_angle+= disc_angle_offset;
			if(disc_angle > 360)
				disc_angle -= 360;
			glRotatef(disc_angle, 0, 0, 1);	
			glRectf(-tile_width/6,tile_width/6, tile_height/6, -tile_height/6);
			glPopMatrix();
		}

		if(sector1.triangle[loop].tex==5)
		{
			glPushMatrix();
			glTranslatef(sector1.triangle[loop].vertex[0].x, sector1.triangle[loop].vertex[0].y, sector1.triangle[loop].vertex[0].z);
			glTranslatef(tile_width/6, 0.01, tile_height/6);
			glRotatef(90, 1, 0, 0);

			disc_color+= disc_offset;
			if(disc_color > 250)
				disc_offset = disc_offset * -1;
			if(disc_color < 50)
				disc_offset = disc_offset * -1;
			if(sector1.triangle[loop].id == 29)
				glColor3f(0, 0, disc_color/ 255.0);
			else if(sector1.triangle[loop].id == 34)
				glColor3f(0, 0, disc_color/ 255.0);
			else if(sector1.triangle[loop].id == 39)
				glColor3f(0, 0, disc_color/ 255.0);
			else
				glColor3f(disc_color/ 255.0, 0, 0);

			gluDisk (quad, 0.3, 0.5, 28, 2);

			glEnable(GL_COLOR_MATERIAL);
			glColor3f(1, 1, 1);
			//glDisable(GL_COLOR_MATERIAL);

			glPopMatrix();	
		}
	}
}

