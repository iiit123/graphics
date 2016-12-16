#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <fstream>

#include "imageloader.h"

#define KEY_ESCAPE 27
#define XINCREASE 'x'
#define YINCREASE 'y'
#define ZINCREASE 'z'

#define XDECREASE 'X'
#define YDECREASE 'Y'
#define ZDECREASE 'Z'
#define ROTATE_X 'c'
#define ROTATE_Y 'v'
#define ROTATE_Z 'b'
#define M_PI 3.14159265358979323846
#define SMOOTH_SHADING_KEY 's'
#define FLAT_SHADING_KEY 'f'

#define INCREASE_SHININESS 'u'
#define DECREASE_SHININESS 'i'

#define LINE_DRAWING_KEY 'l'
#define SOLID_DRAWING_KEY 'k'

#define SMOOTH_SHADING 1
#define FLAT_SHADING 2

#define LINE_DRAWING 1
#define SOLID_DRAWING 2

#define ENABLE_TEXTURE 't'
#define DISABLE_TEXTURE 'r'



#define LOOKATX 'q'
#define LOOKATY 'w'
#define LOOKATZ 'e'

#define SPLINE_2D_MODE 's'
#define SPLINE_3D_MODE 'S'

using namespace std;

int key_index_x=0, key_index_y=0, rotate_angle   = 0, key_index = 0;
int x_direction = 0, y_direction = 0, z_direction = 0;
int shading_flag = FLAT_SHADING;
int drawing_flag = LINE_DRAWING;
int shininess = 50;

char look_at_flag = LOOKATZ;
char rotate_flag;
int condition = 1, texture_flag = 0;

int windowWidth, windowHeight, ind=1;

typedef struct bezierPoints{
    float x;
    float y;
    float z;
}bezierPoints;

bezierPoints bezier_points[20][20];
bezierPoints spline_2d_points[400];

GLfloat spline_ctrlPoints[4][3] = {
  {-3, 3, 3}, 
  {3, 1, 1}, 
  {-3, -1, -1}, 
  {3, -3, -3}
};

GLfloat ctrlpoints_x[4][4] = {
    {-1.5,-0.5,0.5,1.5},
    {-1.5,-0.5,0.5,1.5},
    {-1.5,-0.5,0.5,1.5},
    {-1.5,-0.5,0.5,1.5}
}; 


GLfloat ctrlpoints_y[4][4] = {
    {-1.5,-1.5,-1.5,-1.5},
    {-0.5,-0.5,-0.5,-0.5},
    {0.5,0.5,0.5,0.5},
    {1.5,1.5,1.5,1.5}
}; 

GLfloat ctrlpoints_z[4][4] = {
    {4,2,-1,2},
    {1,3,0,-1},
    {4,0,3,4},
    {-2,-2,0,-1},
}; 

GLfloat muliplier_x[4][4] = {{0.0}}, muliplier_y[4][4] = {{0.0}}, muliplier_z[4][4] = {{0.0}};

GLfloat standard_bezier[4][4] = {
    {-1, 3, -3, 1},
    {3, -6, 3, 0},
    {-3, 3, 0, 0},
    {1, 0, 0, 0},
};


void displayControlPoints() {
   glColor3f(1,0,0);
    for(int i=0; i< 4; i++) {
      for(int j=0; j<4; j++) {

          glPushMatrix();
            glTranslatef(ctrlpoints_x[i][j], ctrlpoints_y[i][j], ctrlpoints_z[i][j]);
            glutSolidSphere(0.05,20,20);
          glPopMatrix();
      }
    }
}

void printMatrix(float matrix[][4]) {

    int i,j;

    for(i=0; i<4; i++) {
      for(j=0; j<4; j++) {
        printf("%f ", matrix[i][j]);
      }
      printf("\n");
    }

}


void displayBsplineControlPoints(int condition_flag) {
    glColor3f(1,0,0);
    for(int i=0; i< 4; i++) {
          float z;
          if(condition_flag == 3) {
              z=spline_ctrlPoints[i][2];
          }
          else {
             z  = 0;
          } 
          glPushMatrix();
            glTranslatef(spline_ctrlPoints[i][0], spline_ctrlPoints[i][1], z);
            glutSolidSphere(0.05,20,20);
          glPopMatrix();
      }
}


void initlights(void)
{


  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);

  GLfloat pos0[] = {4,0,8,1};
  GLfloat LightAmbient[] = { 0.1, 0.1, 0.1, 1.0 };
  GLfloat LightDiffuse[] = { 0.7, 0.7, 0.7, 1.0 };
  GLfloat LightSpecular[] = { 0.4, 0.4, 0.4, 1.0 };

  glLightfv(GL_LIGHT0, GL_POSITION, pos0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);

  GLfloat ambient[] = { 0.0, 1.0, 0.0, 1.0 };
  GLfloat diffuse[] = { 0.0, 0.7, 0.0, 1.0 };
  GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

  
   if(shading_flag == FLAT_SHADING) {
      glShadeModel(GL_FLAT);
   }
   else if(shading_flag == SMOOTH_SHADING) {
     glShadeModel(GL_SMOOTH);
   }
}

void disablelights(void)
{

   glDisable(GL_LIGHTING);
   glDisable(GL_LIGHT0);
   glDisable(GL_NORMALIZE);
}

void drawBspline(int condition_flag)
{
  glLineWidth(4.0);

  GLfloat fu[4];

  float t;
  int i = 0;
  for(t=0; t<=1.05; t = t+0.05) {
    float x = (pow((1-t),3)*spline_ctrlPoints[0][0]) + (3 * pow((1-t), 2) * t * spline_ctrlPoints[1][0]) + (3 * (1-t) * pow(t, 2) * spline_ctrlPoints[2][0]) + (pow(t,3) * spline_ctrlPoints[3][0]);
    float y = (pow((1-t),3)*spline_ctrlPoints[0][1]) + (3 * pow((1-t), 2) * t * spline_ctrlPoints[1][1]) + (3 * (1-t) * pow(t, 2) * spline_ctrlPoints[2][1]) + (pow(t,3) * spline_ctrlPoints[3][1]);
    float z;
    if(condition_flag == 3) {
       z = (pow((1-t),3)*spline_ctrlPoints[0][2]) + (3 * pow((1-t), 2) * t * spline_ctrlPoints[1][2]) + (3 * (1-t) * pow(t, 2) * spline_ctrlPoints[2][2]) + (pow(t,3) * spline_ctrlPoints[3][2]);
    }
    else {
      z = 0;
    }
    spline_2d_points[i].x = x;
    spline_2d_points[i].y = y;
    spline_2d_points[i].z = z;
    i++;
  } 


  glPushMatrix();
  glRotatef(180, 0, 1, 0);
  displayBsplineControlPoints(condition_flag);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_LINES);
  for(i = 0; i<=19;i++) {
    glVertex3f(spline_2d_points[i].x, spline_2d_points[i].y, spline_2d_points[i].z);    
    glVertex3f(spline_2d_points[i+1].x, spline_2d_points[i+1].y, spline_2d_points[i+1].z);
  }
  glEnd();
  glPopMatrix();
}


float matrixMul_second (float u[], float multiplier[][4] ,float v[]) {
    int i,j;

    float sum = 0.0;
    float result[4];

    for(i=0;i<4;i++) {
      for(j=0;j<4;j++) {
        sum = sum + u[j] * multiplier[j][i]; 
      }
      result[i] = sum;
      sum = 0.0;
    }

    for(i=0;i<4;i++) {
      sum = sum + result[i]*v[i];
    }

    return sum;
}

GLuint _textureId; 

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image) {
  GLuint textureId;
  glGenTextures(1, &textureId); //Make room for our texture
  glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
  //Map the image to the texture
  glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
         0,                            //0 for now
         GL_RGB,                       //Format OpenGL uses for image
         image->width, image->height,  //Width and height
         0,                            //The border of the image
         GL_RGB, //GL_RGB, because pixels are stored in RGB format
         GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                           //as unsigned numbers
         image->pixels);               //The actual pixel data
  return textureId; //Returns the id of the texture
}

void plotTexture() {
  Image* image = loadBMP("flower.bmp");
  _textureId = loadTexture(image);
  delete image;

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, _textureId);
  
  //Bottom
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glColor3f(1.0f, 0.2f, 0.2f);
  
  int i, j;

  for (i = 0; i < 20; i++)
  {
    for (j = 0; j < 20; j++)
    {
      //printf("%f %f %f\n", plot[i][j].x, plot[i][j].y, plot[i][j].z);
      //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glBegin(GL_POLYGON);


      glTexCoord3f(0.0f, 0.0f, 1.0f); 
      glVertex3f(bezier_points[i][j].x, bezier_points[i][j].y, bezier_points[i][j].z);
      glTexCoord3f(1.0f, 0.0f, 1.0f); 
      glVertex3f(bezier_points[i][j+1].x, bezier_points[i][j+1].y, bezier_points[i][j+1].z);
      glTexCoord3f(1.0f, 1.0f, 0.0f); 
      glVertex3f(bezier_points[i+1][j+1].x, bezier_points[i+1][j+1].y, bezier_points[i+1][j+1].z);
      glTexCoord3f(0.0f, 1.0f, 0.0f); 
      glVertex3f(bezier_points[i+1][j].x, bezier_points[i+1][j].y, bezier_points[i+1][j].z);
      glEnd();
    }
  }
    
}

void computeBezier() {
    float u,v;
    int i=0,j=0;
    for(u=0; u<1; u = u+ 0.05) {
        for(v=0; v<1; v = v+0.05) {
            float U[4] = {pow(u,3), pow(u,2), u, 1};
            float V[4] = {pow(v,3), pow(v,2), v, 1};

            bezier_points[i][j].x =  matrixMul_second(U, muliplier_x, V);
            bezier_points[i][j].y =  matrixMul_second(U, muliplier_y, V);
            bezier_points[i][j].z =  matrixMul_second(U, muliplier_z, V);

            j++;
        }
        i++;
        j=0;
    }
}

void matrixMul(float first_matrix[][4], float second_matrix[][4], float result_matrix[][4]) {
    int c,d,k;
    float sum = 0.0;
 
    for (c = 0; c < 4; c++) {
      for (d = 0; d < 4; d++) {
        for (k = 0; k < 4; k++) {
          sum = sum + first_matrix[c][k]*second_matrix[k][d];
        }
 
        result_matrix[c][d] = sum;
        sum = 0;
      }
    }
}



void computeTranpose(float matrix[][4], float result_matrix[][4]) {
    int c,d;
    for (c = 0; c < 4; c++)
      for( d = 0 ; d < 4 ; d++ ) {
         result_matrix[d][c] = matrix[c][d];
      }
}

void computeMultiplier() {
    float result_matrix[4][4], transpose_matrix[4][4];

    computeTranpose(standard_bezier, transpose_matrix);

    matrixMul(standard_bezier, ctrlpoints_x, result_matrix);
    matrixMul(result_matrix, transpose_matrix, muliplier_x);

    matrixMul(standard_bezier, ctrlpoints_y, result_matrix);
    matrixMul(result_matrix, transpose_matrix, muliplier_y);

    matrixMul(standard_bezier, ctrlpoints_z, result_matrix);
    matrixMul(result_matrix, transpose_matrix, muliplier_z);
}

void calculateNormal(bezierPoints point_a, bezierPoints point_b, bezierPoints point_c, bezierPoints * result) {

    bezierPoints a, b;

    a.x =  point_b.x - point_a.x;
    a.y =  point_b.y - point_a.y;
    a.z =  point_b.z - point_a.z;

    b.x = point_c.x - point_a.x;
    b.y = point_c.y - point_a.y;
    b.z = point_c.z - point_a.z;

    float traingle_normal[3];

    result->x = ((a.y * b.z) - (a.z * b.y));
    result->y = ((a.z * b.x) - (a.x * b.z));
    result->z = ((a.x * b.y) - (a.y * b.x));

    float positive_x = (result->x >= 0) ? result->x : result->x * -1.0; 
    float positive_y = (result->y >= 0) ? result->y : result->y * -1.0;
    float positive_z = (result->z >= 0) ? result->z : result->z * -1.0;
    
    float sum = positive_x + positive_y + positive_z;

    result->x = (result->x * 1.0)/sum ;
    result->y = (result->y * 1.0)/sum ;
    result->z = (result->z * 1.0)/sum ;
}

void calculateVertexNormal(int i , int j, bezierPoints * normal) {
    bezierPoints normal1, normal2, normal3, normal4;
    int normal1_flag = 0, normal2_flag = 0, normal3_flag =0, normal4_flag =0;
    
    if(i-1 >= 0 && j-1 >=0) {
      calculateNormal(bezier_points[i][j-1], bezier_points[i][j], bezier_points[i-1][j], &normal1);
      normal1_flag = 1;
    }

    if(j-1>=0 && i+1<20) {
      calculateNormal(bezier_points[i][j-1], bezier_points[i+1][j], bezier_points[i][j], &normal2);
      normal2_flag = 1;
    }

    if(i+1< 20 && j+1<20) {
      calculateNormal(bezier_points[i+1][j], bezier_points[i][j+1], bezier_points[i][j], &normal3);
      normal3_flag = 1;
    }

    if(j+1 < 20 && i-1 >=0) {
      calculateNormal(bezier_points[i][j], bezier_points[i][j+1], bezier_points[i-1][j], &normal4);
      normal4_flag = 1;
    }

    normal->x = (normal1.x*normal1_flag + normal2.x*normal2_flag + normal3.x*normal3_flag + normal4.x*normal4_flag);

    normal->y = (normal1.y*normal1_flag + normal2.y*normal2_flag + normal3.y*normal3_flag + normal4.y*normal4_flag);
;
    normal->z = (normal1.z*normal1_flag + normal2.z*normal2_flag + normal3.z*normal3_flag + normal4.z*normal4_flag);
;

   float sum_normals = normal->x + normal->y + normal->z;


   normal->x = normal->x/sum_normals;
   normal->y = normal->y/sum_normals;
   normal->z = normal->z/sum_normals;

}

int backFaceCulling(bezierPoints point_a, bezierPoints point_b, bezierPoints point_c) {
    float look_normal[3] = {0};
    
    if(look_at_flag == LOOKATX) {
      look_normal[0] = 1;
    }
    else if(look_at_flag == LOOKATY) {
      look_normal[1] = 1;
    }
    else if(look_at_flag == LOOKATZ) {
      look_normal[2] = 1;
    }
    
    bezierPoints normal;

    calculateNormal(point_a, point_b, point_c, &normal);

    float result = (look_normal[0]*normal.x) + (look_normal[1]*normal.y) + (look_normal[2]*normal.z); 

    if( result > 0) {
        return 1;
    }
    else {
      return 0;
    }

}

void displayBezier() {
  int i,j;
  glColor3f(1,1,1);
  glLineWidth(1.0);

  
  if(drawing_flag == LINE_DRAWING) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
 
  for(i=0; i<19; i++) {
    for(j=0; j<19; j++) {

      int result_1 = backFaceCulling(bezier_points[i][j], bezier_points[i][j+1], bezier_points[i+1][j+1]);
      int result_2 = backFaceCulling(bezier_points[i][j], bezier_points[i+1][j+1], bezier_points[i+1][j]);

      bezierPoints normal, normal1, normal2, normal3, normal4;
        
      if(result_1 && result_2) {

        if(shading_flag == FLAT_SHADING) {

        calculateNormal(bezier_points[i][j], bezier_points[i][j+1], bezier_points[i+1][j+1], &normal);
        
        glBegin(GL_QUADS);
          glNormal3f(normal.x, normal.y, normal.z);
          glVertex3f(bezier_points[i][j].x, bezier_points[i][j].y, bezier_points[i][j].z);
          glVertex3f(bezier_points[i][j+1].x, bezier_points[i][j+1].y, bezier_points[i][j+1].z);
          glVertex3f(bezier_points[i+1][j+1].x, bezier_points[i+1][j+1].y, bezier_points[i+1][j+1].z);
          glVertex3f(bezier_points[i+1][j].x, bezier_points[i+1][j].y, bezier_points[i+1][j].z);
        glEnd();
        }


        else if(shading_flag == SMOOTH_SHADING) {
        
        calculateVertexNormal(i,j,&normal1);
        calculateVertexNormal(i,j+1, &normal2);
        calculateVertexNormal(i+1, j+1, &normal3);
        calculateVertexNormal(i+1, j, &normal4);

        glBegin(GL_QUADS);
          glNormal3f(normal1.x, normal1.y, normal1.z);
          glVertex3f(bezier_points[i][j].x, bezier_points[i][j].y, bezier_points[i][j].z);          

          glNormal3f(normal2.x, normal2.y, normal2.z);
          glVertex3f(bezier_points[i][j+1].x, bezier_points[i][j+1].y, bezier_points[i][j+1].z);

          glNormal3f(normal3.x, normal3.y, normal3.z);
          glVertex3f(bezier_points[i+1][j+1].x, bezier_points[i+1][j+1].y, bezier_points[i+1][j+1].z);

          glNormal3f(normal4.x, normal4.y, normal4.z);
          glVertex3f(bezier_points[i+1][j].x, bezier_points[i+1][j].y, bezier_points[i+1][j].z);
        glEnd();
        }
      }
      else if(result_1) {
       
        if(shading_flag == FLAT_SHADING) {
        calculateNormal(bezier_points[i][j], bezier_points[i][j+1], bezier_points[i+1][j+1], &normal);
        glBegin(GL_TRIANGLES);
         glNormal3f(normal.x, normal.y, normal.z);
         glVertex3f(bezier_points[i][j].x, bezier_points[i][j].y, bezier_points[i][j].z);
         glVertex3f(bezier_points[i][j+1].x, bezier_points[i][j+1].y, bezier_points[i][j+1].z);
         glVertex3f(bezier_points[i+1][j+1].x, bezier_points[i+1][j+1].y, bezier_points[i+1][j+1].z);
        glEnd();
        }


        else if(shading_flag == SMOOTH_SHADING) {
        calculateVertexNormal(i,j, &normal1);
        calculateVertexNormal(i,j+1, &normal2);
        calculateVertexNormal(i+1, j+1, &normal3);

        glBegin(GL_TRIANGLES);
         glNormal3f(normal1.x, normal1.y, normal1.z);
         glVertex3f(bezier_points[i][j].x, bezier_points[i][j].y, bezier_points[i][j].z);

         glNormal3f(normal2.x, normal2.y, normal2.z);
         glVertex3f(bezier_points[i][j+1].x, bezier_points[i][j+1].y, bezier_points[i][j+1].z);

         glNormal3f(normal3.x, normal3.y, normal3.z);
         glVertex3f(bezier_points[i+1][j+1].x, bezier_points[i+1][j+1].y, bezier_points[i+1][j+1].z);
        glEnd();
       }

      }
      else if(result_2) {
        if( shading_flag == FLAT_SHADING) {
         calculateNormal(bezier_points[i][j], bezier_points[i+1][j+1], bezier_points[i+1][j], &normal);
         glBegin(GL_TRIANGLES);
          glNormal3f(normal.x, normal.y, normal.z);
          glVertex3f(bezier_points[i][j].x, bezier_points[i][j].y, bezier_points[i][j].z);
          glVertex3f(bezier_points[i+1][j+1].x, bezier_points[i+1][j+1].y, bezier_points[i+1][j+1].z);
          glVertex3f(bezier_points[i+1][j].x, bezier_points[i+1][j].y, bezier_points[i+1][j].z);
         glEnd();
       }
       else if(shading_flag == SMOOTH_SHADING) {
        calculateVertexNormal(i, j,&normal1);
        calculateVertexNormal(i+1, j+1, &normal2);
        calculateVertexNormal(i+1, j, &normal3);

        glBegin(GL_TRIANGLES);
         glNormal3f(normal1.x, normal1.y, normal1.z);
         glVertex3f(bezier_points[i][j].x, bezier_points[i][j].y, bezier_points[i][j].z);
       
         glNormal3f(normal2.x, normal2.y, normal2.z);
         glVertex3f(bezier_points[i+1][j+1].x, bezier_points[i+1][j+1].y, bezier_points[i+1][j+1].z);
       
         glNormal3f(normal3.x, normal3.y, normal3.z);
         glVertex3f(bezier_points[i+1][j].x, bezier_points[i+1][j].y, bezier_points[i+1][j].z);
        glEnd();
       }
      }
    }
  }
  
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();
   if(look_at_flag == LOOKATX) {
      gluLookAt (10, 0, 0, 0, 0, 0, 0, 1, 0);
   
   }
   else if(look_at_flag == LOOKATY) {
      gluLookAt (0, 10, 0, 0, 0, 0, 1, 0, 0);
   
   }
   else if(look_at_flag == LOOKATZ) {
      gluLookAt (0, 0, 10, 0, 0, 0, 0, 1, 0);
   } 

    glRotatef(rotate_angle, x_direction, y_direction, z_direction);
   
    glDisable(GL_TEXTURE_2D);
    if(condition == 1) {
      initlights();
      computeMultiplier();
      computeBezier();
      displayBezier();
      displayControlPoints();
      if(texture_flag) {
        plotTexture();
      }
    }
    else if(condition == 2 || condition == 3) {
      disablelights();
      drawBspline(condition);
    }
    glPopMatrix();
   glFlush();

   if(rotate_flag == ROTATE_X || rotate_flag == ROTATE_Y || rotate_flag == ROTATE_Z) {
      rotate_angle = rotate_angle + 1;
      x_direction = (rotate_flag == ROTATE_X) ? 1 : 0;
      y_direction = (rotate_flag == ROTATE_Y) ? 1 : 0;
      z_direction = (rotate_flag == ROTATE_Z) ? 1 : 0;
      glutPostRedisplay();
    }
}

void init(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glutSwapBuffers();
}

 
void handleKeypress ( unsigned char key, int x, int y ) 
{

    if(key == 27)
        exit(0);
      
    if(key == '1') {
        if(condition == 1) {
          key_index_x=0;
          key_index_y=0;
        }
        else if(condition == 2 || condition == 3) {
          key_index = 0;
        }
    }
    else if(key == '2') {
      if(condition == 1) {
        key_index_x = 0;
        key_index_y = 1;
      }
      else if(condition == 2 || condition == 3) {
        key_index = 1;
      }  
    }
    else if(key == '3') {
      if(condition == 1) {
        key_index_x = 0;
        key_index_y = 2;
      }
      else if(condition == 2 || condition == 3) {
        key_index = 2;
      }
    }
    else if(key == '4') {
      if(condition == 1) {
        key_index_x = 0;
        key_index_y = 3;
      }
      else if(condition == 2 || condition == 3) {
          key_index = 3;
      }
    }
    else if(key == '5') {
      key_index_x = 1;
      key_index_y = 0;
    }

    else if(key == '6') {
      key_index_x = 1;
      key_index_y = 1;
    }
    else if(key == '7') {
      key_index_x = 1;
      key_index_y = 2;
    }
    else if(key == '8') {
      key_index_x = 1;
      key_index_y = 3;
    }
    else if(key == '9') {
      key_index_x = 2;
      key_index_y = 0;
    }
    else if(key == '!') {
      key_index_x = 2;
      key_index_y = 1;
    }
    else if(key == '@') {
      key_index_x = 2;
      key_index_y = 2;
    }
    else if(key == '#') {
      key_index_x = 2;
      key_index_y = 3;
    }
    else if(key == '$') {
      key_index_x = 3;
      key_index_y = 0;
    }
    else if(key == '%') {
      key_index_x = 3;
      key_index_y = 1;
    }
    else if(key == '^') {
      key_index_x = 3;
      key_index_y = 2;
    }
    else if(key == '&') {
      key_index_x = 3;
      key_index_y = 3;
    }

    if(key == LOOKATX || key == LOOKATY || key == LOOKATZ) {
        look_at_flag = key;
        glutPostRedisplay();
    }

    if(key == ROTATE_X || key == ROTATE_Y || key == ROTATE_Z) {
      rotate_flag = key;
      glutPostRedisplay();
    }
    else {
      rotate_flag = '\0';
    }

    if(key == FLAT_SHADING_KEY) {
      shading_flag = FLAT_SHADING;
      glutPostRedisplay();
    }
    else if(key == SMOOTH_SHADING_KEY) {
      shading_flag = SMOOTH_SHADING;
      glutPostRedisplay();
    }

    if(key == LINE_DRAWING_KEY) {
      drawing_flag = LINE_DRAWING;
      glutPostRedisplay();
    }
    else if(key == SOLID_DRAWING_KEY) {
      drawing_flag = SOLID_DRAWING;
      glutPostRedisplay();
    }
    
    if(key == INCREASE_SHININESS && condition == 1){
      shininess = shininess + 5;
      glutPostRedisplay();
    }
    else if(key == DECREASE_SHININESS && condition == 1){
      shininess = shininess - 5;
      glutPostRedisplay();
    }

    if(key == ENABLE_TEXTURE) {
      texture_flag = 1;
      glutPostRedisplay();
    }
    else if(key == DISABLE_TEXTURE) {
      texture_flag = 0;
      glutPostRedisplay();
    }

    if( key == XINCREASE) {
        if(condition == 1) {
          ctrlpoints_x[key_index_x][key_index_y] = ctrlpoints_x[key_index_x][key_index_y] + 0.1;
        }
        else if(condition == 2 || condition == 3) {
           printf("hello\n");
            spline_ctrlPoints[key_index][0] = spline_ctrlPoints[key_index][0] + 0.1;
        }
        glutPostRedisplay();
    }
    else if( key == XDECREASE) {
        if(condition == 1) {
          ctrlpoints_x[key_index_x][key_index_y] = ctrlpoints_x[key_index_x][key_index_y] - 0.1;
        }
        else if(condition == 2 || condition == 3) {
            spline_ctrlPoints[key_index][0] = spline_ctrlPoints[key_index][0] - 0.1;
        }
        glutPostRedisplay();
    }

    else if(key == YINCREASE) {
        if(condition == 1) {
          ctrlpoints_y[key_index_x][key_index_y] = ctrlpoints_y[key_index_x][key_index_y] + 0.1;
        }
        else if(condition == 2 || condition == 3) {
          spline_ctrlPoints[key_index][1] = spline_ctrlPoints[key_index][1] + 0.1;
        }
        glutPostRedisplay(); 
    }
    else if(key == YDECREASE) {
        if(condition == 1) {
          ctrlpoints_y[key_index_x][key_index_y] = ctrlpoints_y[key_index_x][key_index_y] - 0.1;
        }
        else if(condition == 2 || condition == 3) {
          spline_ctrlPoints[key_index][1] = spline_ctrlPoints[key_index][1] - 0.1;
        }
        glutPostRedisplay();
    }

    else if(key == ZINCREASE) {
        if(condition == 1) {
          ctrlpoints_z[key_index_x][key_index_y] = ctrlpoints_z[key_index_x][key_index_y] + 0.1;
        }
        else if(condition == 3) {
          spline_ctrlPoints[key_index][2] = spline_ctrlPoints[key_index][2] + 0.1;
        }
        glutPostRedisplay();
    }
    else if(key == ZDECREASE) {
        if(condition == 1) {
          ctrlpoints_z[key_index_x][key_index_y] = ctrlpoints_z[key_index_x][key_index_y] - 0.1;
        }
        else if(condition == 3){
          spline_ctrlPoints[key_index][2] = spline_ctrlPoints[key_index][2] - 0.1;
        }
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

   glutCreateMenu(menu);
   glutAddMenuEntry("Bezier Curve", 1);
   glutAddMenuEntry("Spline 2D", 2);
   glutAddMenuEntry("Spline 3D", 3);

   glutAttachMenu(GLUT_MIDDLE_BUTTON);
}
 
int main(int argc, char **argv) 
{


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    int w = glutGet(GLUT_SCREEN_WIDTH);
    int h = glutGet(GLUT_SCREEN_HEIGHT);
    windowWidth = w * 2 / 3;
    windowHeight = h * 2 / 3;


    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

    glutCreateWindow("Graphics Assing 5");

    init();
    make_menu();

    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeypress);
    glutSpecialFunc(handleKeypress2);
    
    glutMainLoop();                                             
    return 0;
}