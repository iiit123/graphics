all: texture object

texture: main.c
	g++ -Wall -o texture main.c -framework OpenGL -framework GLUT

object: object.cpp
	g++ -Wall -o object object.cpp -framework OpenGL -framework GLUT

clean:
	rm -f texture object	
