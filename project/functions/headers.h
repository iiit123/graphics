void initRendering();
void handleResize(int w, int h);

void specialKeyRelease (int key, int x, int y);
void specialKeyPressed(int key, int x, int y);
void keyPressed(unsigned char key, int x, int y);

void drawScene();
GLvoid DrawGLScene();
void update(int value);

void SetupWorld();
void create_connecting(int id, int indx, int flag, float offset, int flag2);
void create_support(int id, int indx, int flag2);
int add_to_triangles(float xleft, float zleft, int indx, int id, int flag);


float terrain_collision();
void vert_collisions();
int inside_rectangle_check(int id);
int inside_rectangle_check_x(int id);
int inside_rectangle_check_z(int id);