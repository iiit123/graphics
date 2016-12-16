#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>
#include <stdlib.h>


int windowHeight, windowWidth;
int x_min = 0;
int y_min = 0;
int x_max = 200;
int y_max = 200;
int x_center = 0;
int y_center = 0;
int angle = 0;


long getImageInfo(FILE*, long, int);

typedef struct {
  int rows; 
  int cols; 
  int data[100000][3];
} sImage;

sImage originalImage;


void init(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, windowWidth, windowHeight, 0, -1, 1 );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glutSwapBuffers();
}


void MapTexture(int x1, int x2, int y1, int y2) {
    int width = x2 - x1;
    int height = y2 - y1; 
    int i,j,k=0,l=0;

    int count = 0,r,c;  


    int center_x = ( x1 !=0 ) ? (width/2 * originalImage.cols)/(x2-x1) : 0;
    int center_y = ( y1 !=0 )  ? (height/2 * originalImage.rows)/(y2-y1) : 0;

    for(i=0; i<=height; i++) {

      int y_point = (i* originalImage.rows/2)/height;

      for(j=0; j<=width; j++) {

        int x_point = (j * originalImage.cols/2)/width;

        int array_index = (center_y * originalImage.cols) + center_x + y_point * originalImage.cols + x_point;

        float red = originalImage.data[array_index][0] * 1.0/255;

        float green = originalImage.data[array_index][1] * 1.0/255;
        
        float blue = originalImage.data[array_index][2] * 1.0/255;

        glBegin(GL_POINTS); 
        glPointSize(1);
          glColor3f(red, green, blue);
          glVertex3f(x1+j, y1+i, 0.0f);
        glEnd();
      }
    }
}

void display(void)
{  

  glClear(GL_COLOR_BUFFER_BIT);
  
  glPushMatrix();
    glTranslatef(windowWidth/3, windowHeight/3, 0.0);
    glRotatef(angle, 0, 0, 1);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
       MapTexture(x_min, x_max/2 + x_center, y_min, y_max/2 + y_center);
       MapTexture(x_max/2 + x_center, x_max, y_min, y_max/2 + y_center);
       MapTexture(x_min, x_max/2 + x_center, y_max/2 + y_center, y_max);
       MapTexture(x_max/2 + x_center, x_max, y_max/2 + y_center,y_max);
  glPopMatrix();
  glFlush();

}

void handleKeypress(unsigned char key, int x, int y) {

  if (key == 27) {
      exit(0);     // escape key is pressed
    }   

    if(key == 'x') {
       x_center = x_center + 10;
    }
    if(key == 'X') {
       x_center = x_center - 10;
    }   
    if(key == 'Y') {
       y_center = y_center - 10;
    }   

    if(key == 'y'){
      y_center = y_center + 10;
    }

    if(key =='r') {
        angle = angle + 30;
    }

     if(key =='R') {
        angle = angle - 30;
    }
    glutPostRedisplay();      

}



int main(int argc, char* argv[])
{
  FILE        *bmpInput, *rasterOutput;
  unsigned char     someChar;
  unsigned char     *pChar;
  long        fileSize;
  int       vectorSize, nColors;
  int       r, c;

  /*--------INITIALIZE POINTER----------*/
  someChar = '0';
  pChar = &someChar;

  if(argc < 2)
  {
    printf("Usage: %s bmpInput.bmp\n", argv[0]);
    exit(0);
  }

  printf("Reading file %s\n", argv[1]);

  /*----DECLARE INPUT AND OUTPUT FILES----*/
  bmpInput = fopen(argv[1], "rb");
  rasterOutput = fopen("data24.html", "w");

  fseek(bmpInput, 0L, SEEK_END);

  /*-----GET BMP INFO-----*/
  originalImage.cols = (int)getImageInfo(bmpInput, 18, 4);
  originalImage.rows = (int)getImageInfo(bmpInput, 22, 4);
  fileSize = getImageInfo(bmpInput, 2, 4);
  nColors = getImageInfo(bmpInput, 46, 4);

  /*----PRINT BMP INFO TO SCREEN-----*/
  printf("Width: %d\n", originalImage.cols);
  printf("Height: %d\n", originalImage.rows);
  printf("File size: %ld\n", fileSize);
  printf("Bits/pixel: %d\n", getImageInfo(bmpInput, 28, 4));
  printf("No. colors: %d\n", nColors);


  /*----FOR 24-BIT BMP, THERE IS NO COLOR TABLE-----*/
  fseek(bmpInput, 54, SEEK_SET);

  /*-----------READ RASTER DATA-----------*/
  for(r=0; r<=originalImage.rows-1; r++)
  {
    for(c=0; c<=originalImage.cols-1; c++)
    {
      
      /*----READ FIRST BYTE TO GET BLUE VALUE-----*/
      fread(pChar, sizeof(char), 1, bmpInput);
      int blueValue = *pChar;
      originalImage.data[r*originalImage.cols+c][2] = blueValue;

      /*-----READ NEXT BYTE TO GET GREEN VALUE-----*/
      fread(pChar, sizeof(char), 1, bmpInput);
      int greenValue = *pChar;
      originalImage.data[r*originalImage.cols+c][1] = greenValue;

      /*-----READ NEXT BYTE TO GET RED VALUE-----*/
      fread(pChar, sizeof(char), 1, bmpInput);
      int redValue = *pChar;
      originalImage.data[r*originalImage.cols+c][0] = redValue;

    }
  }

  fclose(bmpInput);
  fclose(rasterOutput);


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

    glutKeyboardFunc(handleKeypress);

    glutDisplayFunc(display);
    glutMainLoop();

  return 0;
}

long getImageInfo(FILE* inputFile, long offset, int numberOfChars)
{
  unsigned char     *ptrC;
  long        value=0L;
  int       i;
  unsigned char     dummy;


  dummy = '0';
  ptrC = &dummy;

  fseek(inputFile, offset, SEEK_SET);

  for(i=1; i<=numberOfChars; i++)
  {
    fread(ptrC, sizeof(char), 1, inputFile);
    /* calculate value based on adding bytes */
    value = (long)(value + (*ptrC)*(pow(256, (i-1))));
  }

  return(value);
}
