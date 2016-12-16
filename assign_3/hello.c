  #include<stdio.h>
  #include <iostream>
  #include<math.h>
  #include <cmath>
  #include <GLUT/glut.h>
  #include<stdlib.h>

  #define ANGLE 10
  
  int windowWidth, windowHeight;
  GLUquadricObj * quadratic;
  int total_angle = 0;
  int global_angle = 0;
  float global_x = 0;
  float global_y = 0;
  float global_z = 0;

double m_xtheta=0;
double x = 0;
double y = 0;
double z = 0;
double angle = 0;
double m_ytheta=0;
double m_ztheta=0;
double m_slide=40;
bool YawBool=false;
bool PitchBool=false;
bool RollBool=false;
bool SlideBool=false;
double mSlide=250.0;

  /* Two cones at end of the rods  along with balls.*/ 
  void coneRotate(int cy_length, int cy_radius, int rotation_flag) {

       glPushMatrix();
         glRotatef(-90 + rotation_flag * angle, 1.0f, 0.0f, 0.0f);
         glPushMatrix();
            glRotatef(90, 1.0f, 0.0f, 0.0f);
            glTranslatef (0, cy_length -200, 0.0);
            glColor4f(1,0,0,1); 
            glutSolidSphere(20, 32,32);
         glPopMatrix();
         glColor4f(1,1,1,1);
         gluCylinder(quadratic, 0, cy_radius, cy_length - 200, 32,32);
      glPopMatrix();
  }

  void build_object(char flag) {
      

      quadratic = gluNewQuadric();
      int cy_length = 300;
      int cy_radius = 10;

     glPushMatrix();

        glTranslatef (windowWidth/2, windowHeight/2, 0.0);   

          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          gluPerspective(90.0f, (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);

         glMatrixMode(GL_MODELVIEW);
         glLoadIdentity();

        if(flag == 'x') {    
          gluLookAt (300.0 + global_x, 0.0 - global_y, 0.0 + global_z, 0, 0, 0.0, 0.0, -1.0, 0.0);
        }
        else if(flag == 'y') {
            gluLookAt (0.0 + global_x, -300.0 - global_y , 0.0 + global_z, 0, 0, 0.0, 0.0, 0.0, -1.0);
        }
        else if(flag == 'z') {
            gluLookAt (0.0 + global_x, 0.0 - global_y, 300.0 + global_z, 0, 0, 0.0, 0.0, -1.0, 0.0);
        }

         glBegin(GL_LINES);
         glColor4f(0,1,0,0);
        for(float i =-5; i<=5; )
        {
          glVertex3f(i*300.f, 10.f, -300.f);
          glVertex3f(i*300.f, 10.f, 300.f);
          i+=0.1;
        }
  
        for(float j =-5; j<=5; )
        {
          glVertex3f(-300.f, 10.f, j*300.f);
          glVertex3f(300.f, 10.f, j*300.f);
          j+=0.1;
        }
        glEnd();

        glRotatef (total_angle, 0, 1, 0);
    
      // Horizontal cylinder
      glPushMatrix();
         glRotatef(90 + angle, 0, 1 , 0);
        
         glPushMatrix();
            glRotatef(-90, 0, 1, 0);
            glTranslatef (- cy_length/2, 0, 0.0);
         
            glPushMatrix();
              coneRotate(cy_length, cy_radius, -1);
            glPopMatrix();
         
            glColor4f(1,0,0,1); 
            glutSolidSphere(16, 32,32);
         glPopMatrix();

         glColor4f(1,1,1,1); 
         gluCylinder(quadratic, cy_radius, cy_radius, cy_length/2, 32,32);
      glPopMatrix();

      glPushMatrix();
         glRotatef(-90 + angle, 0, 1 , 0);

         glPushMatrix();
            glRotatef(90, 0, 1, 0); 
            glTranslatef (cy_length/2, 0, 0.0);

            glPushMatrix();
              coneRotate(cy_length, cy_radius, 1);
            glPopMatrix();

            glColor4f(1,0,0,1); 
            glutSolidSphere(16, 32,32);
          glPopMatrix();

         glColor4f(1,1,1,1); 
         gluCylinder(quadratic, cy_radius, cy_radius, cy_length/2, 32,32);
      glPopMatrix();

      //Vertical cylinder
       glPushMatrix();
         glRotatef(-90, 1.0f, 0.0f, 0.0f);
         glColor4f(1,1,1,1);
         gluCylinder(quadratic, cy_radius, cy_radius, cy_length - 100, 32,32);
      glPopMatrix();

      //Sphere at the intersection of cyclinders
      glPushMatrix();
         glColor4f(1,0,0,1); 
         glutSolidSphere(16, 32,32);
      glPopMatrix();

    glPopMatrix();
  }

  void build() {
    
    glPushMatrix();

        glTranslatef (windowWidth/2, windowHeight/2, 0.0);   

          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          gluPerspective(90.0f, (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);

         glMatrixMode(GL_MODELVIEW);
         glLoadIdentity();

          gluLookAt (mSlide + global_x, 0.0 - global_y, 0.0 + global_z, 0, 0, 0.0, 0.0, -1.0, 0.0);

          glBegin(GL_LINES);
          glColor4f(1,1,0,0);
          for(float i =-5; i<=5; )
          {
            glVertex3f(i*300.f, 10.f, -300.f);
            glVertex3f(i*300.f, 10.f, 300.f);
            i+=0.1;
          }
  
          for(float j =-5; j<=5; )
          {
            glVertex3f(-300.f, 10.f, j*300.f);
            glVertex3f(300.f, 10.f, j*300.f);
            j+=0.1;
          }
          glEnd();

        glPushMatrix();
          glRotatef(m_xtheta, 1, 0, 0);
          glRotatef(m_ytheta, 0, 1, 0);
          glRotatef(m_ztheta, 0, 0, 1);
          glColor4f(1,1,1,1);
          GLUquadricObj* quadObj = gluNewQuadric ();
          gluQuadricDrawStyle (quadObj, GLU_LINE);
          gluCylinder(quadObj, 30 , 30, 300, 10, 10); 
          gluDeleteQuadric(quadObj);
        glPopMatrix();

    glPopMatrix();
  }

  void display(void)
  {   
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, windowWidth/2, windowHeight/2);
    glLoadIdentity();
    build_object('x');


    glViewport(0, windowHeight/2, windowWidth/2, windowHeight/2);
    glLoadIdentity();
    build_object('y');

    glViewport(windowWidth/2, windowHeight/2, windowWidth/2, windowHeight/2);
    glLoadIdentity();
    build_object('z');

    glViewport(windowWidth/2, 0, windowWidth/2, windowHeight/2);
    glLoadIdentity();
    build();


    glFlush(); 

  }

  void init(void)
  {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
;  }

  void handleMouseclick(int button, int state, int x, int y) {

    if (state == GLUT_DOWN)
    {
      if (button == GLUT_LEFT_BUTTON ) {

        

      }
      else if (button == GLUT_RIGHT_BUTTON) {

      }

      glutPostRedisplay();
    }
  }

  void handleKeypress(unsigned char key, int x, int y) {

    if (key == 27) {
      exit(0);     // escape key is pressed
    }

    if(key == 'r') {
       angle = angle + 10;
       glutPostRedisplay();
    }

    if(key == 'c') {
      total_angle = total_angle+10;
      printf("total_angle = %d\n", total_angle);
      glutPostRedisplay();
    }

    if(key == 'x') {
      global_x = global_x + 5;
      glutPostRedisplay();

    }

    if(key == 'y') {
      global_y = global_y + 5;
      glutPostRedisplay();

    }

    if(key == 'z') {
      global_z = global_z + 5;
      glutPostRedisplay();

    }

    if(key == 'X') {
      global_x = global_x - 5;
      glutPostRedisplay();

    }

    if(key == 'Y') {
      global_y = global_y - 5;
      glutPostRedisplay();

    }

    if(key == 'Z') {
      global_z = global_z - 5;
      glutPostRedisplay();

    }
  }

  void handleMouseMove(int x, int y) {

  }

  void RollUp(float n){
    m_ztheta+=n;
    if(m_ztheta > 360) m_ztheta=-360;
  }
  void RollDown(float n){
     m_ztheta-=n;
    if(m_ztheta < -360) m_ztheta=360;
  }
  void PitchUp(float n){
    m_ytheta+=n;
    if(m_ytheta > 360) m_ytheta=-360;
  }
  void PitchDown(float n){
    m_ytheta-=n;
    if(m_ytheta < -360) m_ytheta=360;
  }
  void YawUp(float n){
    m_xtheta+=n;
    if(m_xtheta > 360) m_xtheta=-360;
  }
  void YawDown(float n){
    m_xtheta-=n;
    if(m_xtheta < -360) m_xtheta=360;
  }
  void SlideUp(float n){
     mSlide+=n;
  }
  void SlideDown(float n ){
    mSlide-=n;
  }
  void x_down(float n)
  {
    x = x - n;
  }
  void x_up(float n)
  {
    x = x + n;
  }
  void y_down(float n)
  {
    y = y - n;
  }
  void y_up(float n)
  {
    y = y + n;
  }
  void z_down(float n)
  {
    z = z - n;
  }
  void z_up(float n)
  {
    z = z + n;
  }


  void menu(int value)
  {
    switch(value) {

      case 1:
          YawBool=false;
      PitchBool=false;
      RollBool=true;
      SlideBool=false;
      RollUp(ANGLE);
            break;
    case 2:
            YawBool=false; 
      PitchBool=false;
      RollBool=true;
      SlideBool=false;
      RollDown(ANGLE);
      break;
    case 3:
          YawBool=false; 
      PitchBool=true;
      RollBool=false;
      SlideBool=false;
      PitchUp(ANGLE);
      break;
    case 4:
          YawBool=false; 
      PitchBool=true;
      RollBool=false;
      SlideBool=false;
      PitchDown(ANGLE);
      break;
    case 5:
            YawBool=true;
      PitchBool=false;
      RollBool=false;
      SlideBool=false;
            YawUp(ANGLE);
            break;
      case 6:
            YawBool=true;
      PitchBool=false;
      RollBool=false;
      SlideBool=false;
            YawDown(ANGLE);
      break;
      case 7:
            YawBool=false;
      PitchBool=false;
      RollBool=false;
      SlideBool=true;
            SlideUp(ANGLE);
      break;
      case 8:
            YawBool=false;
      PitchBool=false;
      RollBool=false;
      SlideBool=true;
            SlideDown(ANGLE); 
            break;      
      }   

      glutPostRedisplay();
  }

  void make_menu(void)
  { 
     glutCreateMenu(menu);
     glutAddMenuEntry("Roll+",  1);
  glutAddMenuEntry("Roll-",  2);
  glutAddMenuEntry("Pitch+", 3);
  glutAddMenuEntry("Pitch-", 4);
  glutAddMenuEntry("Yaw+",   5);
  glutAddMenuEntry("Yaw-",   6);
  glutAddMenuEntry("Slide+", 7);
  glutAddMenuEntry("Slide-", 8);

     glutAttachMenu(GLUT_MIDDLE_BUTTON);
  }



  int main(int argc, char** argv)
  { 

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    
    int w = glutGet(GLUT_SCREEN_WIDTH);
    int h = glutGet(GLUT_SCREEN_HEIGHT);
    windowWidth = w * 2 / 3;
    windowHeight = h * 2 / 3;

    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

    glutCreateWindow("Graphics Assing 3");

    init();

    make_menu();

    glutMouseFunc(handleMouseclick);
    glutKeyboardFunc(handleKeypress);

    glutPassiveMotionFunc(handleMouseMove);


    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
  }

