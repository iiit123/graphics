#define BUFFER_OFFSET(x)((char *)NULL+(x))

void DrawMesh(GLuint _vbo, GLuint _inx, int index) {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _inx);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(0));

        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(3 * sizeof (float)));

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(6 * sizeof (float)));

        glDrawElements(GL_TRIANGLES, FACES_COUNT[index] * 3, INX_TYPE, BUFFER_OFFSET(0));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
}

void InitMesh(int index) {
        glGenBuffers(1, &vbo[index]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[index]);
        glBufferData(GL_ARRAY_BUFFER, sizeof (struct vertex_struct) * VERTEX_COUNT[index], vertexs[index], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vinx[index]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx[index]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (indexes[index][0]) * FACES_COUNT[index] * 3, indexes[index], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void rotateMe(float angle, float x, float y, float z, GLdouble *roboPart){
  glPushMatrix();
  glLoadMatrixd(roboPart);
  glRotatef(angle,x,y,z);
  glGetDoublev(GL_MODELVIEW_MATRIX, roboPart);
  glPopMatrix();
}

void roboWalk(){

   rotateMe(2*rotationParity,0,0,1,origins[7]);     //Right Hand
   rotateMe(2*rotationParity,0,0,1,origins[8]);    /*Left Hand*/


  rotateMe(0.9*rotationParity,0,0,1,origins[1]); //Right upper arm
  rotateMe(0.9*rotationParity,0,0,1,origins[2]); //Left  upper arm

  rotateMe(-0.4*rotationParity,1,0,0,origins[3]); //Right upper leg
  rotateMe(0.4*rotationParity,1,0,0,origins[4]);  //Left  upper leg

  rotateMe(-1.3*rotationParity,1,0,0,origins[5]); /*Right Leg*/
  rotateMe(1.3*rotationParity,1,0,0,origins[6]);  /*Left Leg*/

  rotationCount++;
  if(!(rotationCount%6))
    if(((rotationCount/6)%2))
      rotationParity=rotationParity*-1;
}
