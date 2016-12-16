CC = g++

ifeq ($(shell uname),Darwin)
	CFLAGS = -Wall  $(COMPILERFLAGS) $(INCLUDE) $(LIBDIR)
else
	CFLAGS = -Wall -g
endif

PROG = game

SRCS = main.cpp

ifeq ($(shell uname),Darwin)
	LIBS =  -framework OpenGL -framework GLUT
else
	LIBS = -lglut -lGL -lGLU 
endif



all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)

# run for other codes as well , read README
