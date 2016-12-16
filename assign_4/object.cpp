#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <string>
#include <fstream>

using namespace std;
 
int windowWidth, windowHeight;

typedef struct Object{
  
    float* Faces_Triangles;                 // Stores the triangles
    float* vertexBuffer;                    // Stores the points which make the object
    long TotalConnectedPoints;              // Stores the total number of connected verteces
    long TotalConnectedTriangles;           // Stores the total number of connected triangles
 
}Object;
 

Object obj;
float g_rotation;
int y_angle = 0;
int zoom = -30;
 
#define POINTS_PER_VERTEX 3
#define TOTAL_FLOATS_IN_TRIANGLE 9
using namespace std; 
 
int loadObject(char* filename)
{
    string line;
    ifstream objFile (filename);    
    if (objFile.is_open())                                                  // If obj file is open, continue
    {
        objFile.seekg (0, ios::end);                                        // Go to end of the file, 
        long fileSize = objFile.tellg();                                    // get file size
        objFile.seekg (0, ios::beg);                                        // we'll use this to register memory for our 3d model
 
        printf("file size is %ld\n", fileSize);

        obj.vertexBuffer = (float*) malloc (fileSize);                          // Allocate memory for the verteces
        obj.Faces_Triangles = (float*) malloc(fileSize*sizeof(float));          // Allocate memory for the triangles
 
        int triangle_index = 0;                                             // Set triangle index to zero
        int normal_index = 0;                                               // Set normal index to zero
 
        while (! objFile.eof() )                                            // Start reading file data
        {       
            getline (objFile,line);                                         // Get line from file
 
            if (line.c_str()[0] == 'v')                                     // The first character is a v: on this line is a vertex stored.
            {
                line[0] = ' ';                                              // Set first character to 0. This will allow us to use sscanf
 
                sscanf(line.c_str(),"%f %f %f ",                            // Read floats from the line: v X Y Z
                    &obj.vertexBuffer[obj.TotalConnectedPoints],
                    &obj.vertexBuffer[obj.TotalConnectedPoints+1], 
                    &obj.vertexBuffer[obj.TotalConnectedPoints+2]);
 
                obj.TotalConnectedPoints += POINTS_PER_VERTEX;                  // Add 3 to the total connected points
            }
            if (line.c_str()[0] == 'f')                                     // The first character is an 'f': on this line is a point stored
            {
                line[0] = ' ';                                              // Set first character to 0. This will allow us to use sscanf
 
                int vertexNumber[4] = { 0, 0, 0 };
                sscanf(line.c_str(),"%i%i%i",                               // Read integers from the line:  f 1 2 3
                    &vertexNumber[0],                                       // First point of our triangle. This is an 
                    &vertexNumber[1],                                       // pointer to our vertexBuffer list
                    &vertexNumber[2] );                                     // each point represents an X,Y,Z.
 
                vertexNumber[0] -= 1;                                       // OBJ file starts counting from 1
                vertexNumber[1] -= 1;                                       // OBJ file starts counting from 1
                vertexNumber[2] -= 1;                                       // OBJ file starts counting from 1
 
 
 
                int tCounter = 0;
                for (int i = 0; i < POINTS_PER_VERTEX; i++)                 
                {
                    obj.Faces_Triangles[triangle_index + tCounter ] = obj.vertexBuffer[3*vertexNumber[i] ];
                    obj.Faces_Triangles[triangle_index + tCounter +1 ] = obj.vertexBuffer[3*vertexNumber[i]+1 ];
                    obj.Faces_Triangles[triangle_index + tCounter +2 ] = obj.vertexBuffer[3*vertexNumber[i]+2 ];
                    tCounter += POINTS_PER_VERTEX;
                }

                triangle_index += TOTAL_FLOATS_IN_TRIANGLE;
                obj.TotalConnectedTriangles += TOTAL_FLOATS_IN_TRIANGLE; 
            }   
        }
        objFile.close();                                                        // Close OBJ file
    }
    else 
    {
        printf("Unable to open file\n");                              
    }
    return 0;
}
 
void release()
{
    free(obj.Faces_Triangles);
    free(obj.vertexBuffer);
}
 
void draw()
 {

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnableClientState(GL_VERTEX_ARRAY);                       
    glVertexPointer(3,GL_FLOAT, 0, obj.Faces_Triangles);        
    glDrawArrays(GL_TRIANGLES, 0, obj.TotalConnectedTriangles); 
    glDisableClientState(GL_VERTEX_ARRAY);                      
}
 

 
void display() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
            
             glMatrixMode(GL_PROJECTION);
                 glLoadIdentity();
                 gluPerspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);
            
            glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                    gluLookAt (0, 0, zoom, 0, 0, 0, 0, 1, 0.0);
                    glRotatef(y_angle,0,1,0);
                    draw();
    
    glPopMatrix();
    glutSwapBuffers();
}
 
 
void init () {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
}
 
 

void handleKeypress(unsigned char key, int x, int y) {

    if (key == 27) {
      exit(0);     // escape key is pressed
    }

    if(key == 'r') {
       y_angle = y_angle + 10;
       glutPostRedisplay();
    }

    if(key == 'z') {
        zoom = zoom + 1;
        glutPostRedisplay();
    }
    if(key == 'x') {
        zoom = zoom - 1;
        glutPostRedisplay();
    }
}
 
int main(int argc, char **argv) 
{


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    int w = glutGet(GLUT_SCREEN_WIDTH);
    int h = glutGet(GLUT_SCREEN_HEIGHT);
    windowWidth = w * 2 / 3;
    windowHeight = h * 2 / 3;


    obj.TotalConnectedTriangles = 0; 
    obj.TotalConnectedPoints = 0;


    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

    glutCreateWindow("Graphics Assing 4");


    loadObject("teapot.obj");

    init();

    glutKeyboardFunc(handleKeypress);

    glutDisplayFunc(display);
    
    glutMainLoop();                                             
    return 0;
}