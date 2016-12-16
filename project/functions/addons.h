float rad(float angle)
{
	return angle * PI / 180.0;
}

void readstr(FILE *f, char *string)
{
	do {
		fgets(string, 255, f); // read the line
	} while ((string[0] == '/') || (string[0] == '\n'));
	return;
}

static unsigned int getint(FILE *fp)
{
	int c, c1, c2, c3;

	// get 4 bytes
	c = getc(fp);
	c1 = getc(fp);
	c2 = getc(fp);
	c3 = getc(fp);

	return ((unsigned int) c) +
		(((unsigned int) c1) << 8) +
		(((unsigned int) c2) << 16) +
		(((unsigned int) c3) << 24);
}
static unsigned int getshort(FILE *fp)
{
	int c, c1;

	//get 2 bytes
	c = getc(fp);
	c1 = getc(fp);

	return ((unsigned int) c) + (((unsigned int) c1) << 8);
}


int ImageLoad(char *filename, Image *image)
{
	FILE *file;
	unsigned long size;                 // size of the image in bytes.
	unsigned long i;                    // standard counter.
	unsigned short int planes;          // number of planes in image (must be 1)
	unsigned short int bpp;             // number of bits per pixel (must be 24)
	char temp;                          // used to convert bgr to rgb color.

	// make sure the file is there.
	if ((file = fopen(filename, "rb"))==NULL) {
		printf("File Not Found : %s\n",filename);
		return 0;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);

	// No 100% errorchecking anymore!!!

	// read the width
	image->sizeX = getint (file);

	// read the height
	image->sizeY = getint (file);

	// calculate the size (assuming 24 bits or 3 bytes per pixel).
	size = image->sizeX * image->sizeY * 3;

	// read the planes
	planes = getshort(file);
	if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}

	// read the bpp
	bpp = getshort(file);
	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}

	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);

	// read the data.
	image->data = (char *) malloc(size);
	if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		return 0;
	}

	if ((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}

	for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
		temp = image->data[i];
		image->data[i] = image->data[i+2];
		image->data[i+2] = temp;
	}
	return 1;
}

void generate_filter(Image* image1, GLuint* texture, char *name)
{
	image1 = (Image *) malloc(sizeof(Image));
	if (image1 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}
	if (!ImageLoad(name, image1)) {
		exit(1);
	}
	glGenTextures(3, &texture[0]);

	// nearest filtered texture
	glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // scale cheaply when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // scale cheaply when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);

	// linear filtered texture
	glBindTexture(GL_TEXTURE_2D, texture[1]);   // 2d texture (x and y size)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);

	// mipmapped texture
	glBindTexture(GL_TEXTURE_2D, texture[2]);   // 2d texture (x and y size)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale mipmap when image smalled than texture
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image1->sizeX, image1->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image1->data);

}

GLvoid LoadGLTextures()
{
	Image *image1;
	char arr[100];
	strcpy(arr,"Data/grass.bmp");
	generate_filter(image1, texture, arr);

	Image *image2;
	strcpy(arr,"Data/sky.bmp");
	generate_filter(image2, texture2, arr);

	Image *image3;
	strcpy(arr,"Data/ground.bmp");
	generate_filter(image3, texture3, arr);

	Image *image4;
	strcpy(arr,"Data/water4.bmp");
	generate_filter(image4, texture4, arr);

	Image *image5;
	strcpy(arr,"Data/ground.bmp");
	generate_filter(image5, texture5, arr);

	Image *image6;
	strcpy(arr,"Data/mud3.bmp");
	generate_filter(image6, texture6, arr);

	Image *image7;
	strcpy(arr,"Data/cap.bmp");
	generate_filter(image7, texture_robo[0], arr);	


	Image *image8;
	strcpy(arr,"Data/minidoor.bmp");
	generate_filter(image8, texture_robo[1], arr);	


	Image *image9;
	strcpy(arr,"Data/robo_3.bmp");
	generate_filter(image9, texture_robo[2], arr);	

};
