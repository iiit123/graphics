
#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define ESCAPE 27
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

GLuint vbo[10];
GLuint vinx[10];


typedef struct point
{
	float x, y, z;
}point;
typedef struct box
{
	point min, max;
}box;
typedef struct boxed_object
{
	char *name;
	std::vector<box> boxes;
}boxed_object;


typedef struct
{
	GLfloat x, y, z;     // 3d coords.
	GLfloat u, v;        // texture coords.
} VERTEX;

typedef struct
{
	VERTEX vertex[3];    // 3 vertices array
	int tex;
	int id;
} TRIANGLE;

typedef struct
{
	int numtriangles;    // number of triangles in the sector
	TRIANGLE* triangle;  // pointer to array of triangles.
} SECTOR;

SECTOR sector1;
SECTOR sector2;

typedef struct
{
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
} Image;

int numtriangles;
/***********************************************************************************/


/****************************  VARIABLES FOR GAME MODE  ****************************/

int w, h;
int view_mode = 0;	// 0: 1st Person , 1: Third Person , 2: Tower View
float xcenter, zcenter;
float dhelicopter = 10;
float x_look_helicopter=0, z_look_helicopter=0;
float x_helicopter=0, z_helicopter=0;
int tile_index = 5;
float  tile_width=6, tile_height=6, tile_dist=10;
float connecting_dist = 0, connecting_offset = 0.06;
int n = 5, m = 5;
// Starting index for connecting tiles
int indx1, id1, indx2, id2, indx3, id3;
float zoom_x, zoom_z;
int start_flag = 0;


/***********************************************************************************/


/****************************  VARIABLES FOR STEVE ********************************/

int direction; 		// 0 : Still  1: Moving
float _angle = 180;
float leg_angle = 0;
float arm_angle = 0;
float lookup =0;
float offset = 2;	// Speed of arm, leg rotations
float xtrans = 0, ztrans = 0, ytrans = 0.2, ytrans_old = 0.2;
int rotationParity=1, rotationCount=0;
int rotate_robo = 0, robo_type = 0;


/***********************************************************************************/


/*************************** VARIABLES FOR GAME LOGIC *******************************/

float dist = 0, fall_dist = 0;
int jump_flag = 0, fall_flag = 0;
float yjump=0, tjump=0;

int current_id = 50; 	// to store the current position of steve

std::vector<boxed_object> boxed_obj;		// To store all boxed objs.
int boxindex = 0;			// To store the number of objects.
time_t now, begin;
float elapsed, time_left;
int mapi, mapj;
char pos_map[11][11];

/***********************************************************************************/


/********** GLOBAL VARIABLES FOR TEXTURES,  LIGHTING *******************************/

int loop;             // general loop variable
GLuint texture[3];       // storage for 3 textures;
GLuint texture2[3];
GLuint texture3[3];
GLuint texture4[3];
GLuint texture5[3];
GLuint texture6[3];
GLuint texture7[3];

GLuint texture_robo[3][3];


int light = 0;           // lighting on/off
int blend = 0;        // blending on/off


GLfloat LightAmbient[]  = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat LightDiffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat LightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};

GLuint filter = 0;       // texture filtering method to use (nearest, linear, linear + mipmaps)
GLUquadric *quad;
float disc_color = 50, disc_offset = 0.1;
float disc_angle = 0, disc_angle_offset = 0.0025;

