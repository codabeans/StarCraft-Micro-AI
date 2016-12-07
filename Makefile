#Talked to the creator of sparcraft and he said expect major changes
#Reverting this as it will just get replaced by David's when he pushes the changes.

BWAPI		=	/home/faust/Documents/starcraft-ai/bwapi/bwapi
CAFFE		=	/home/faust/Documents/caffe
BUILD		= 	build

CC		=	g++

SDL_LDFLAGS=`sdl2-config --libs` 
SDL_CFLAGS=`sdl2-config --cflags` 
CFLAGS=-O3 -std=c++11 $(SDL_CFLAGS)
LDFLAGS=-lGL -lGLU -lSDL2_image -lopencv_core -lopencv_highgui -lopencv_imgproc -lboost_system -lglog -lcudnn -L$(CAFFE)/build/lib -lcaffe $(SDL_LDFLAGS)
INCLUDES=-I$(BWAPI)/include -I$(BWAPI)/include/BWAPI -I$(BWAPI) -I$(CAFFE)/include -I/usr/local/cuda/include
SOURCES=$(wildcard $(BWAPI)/BWAPILIB/Source/*.cpp) $(wildcard $(BWAPI)/BWAPILIB/*.cpp) $(wildcard source/*.cpp) $(wildcard source/main/*.cpp) $(wildcard source/gui/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

all:SparCraft 

SparCraft:$(OBJECTS)
	@echo linking ...
	@$(CC) $(OBJECTS) -o bin/$@  $(LDFLAGS)
	@echo Done

.cpp.o:
	@echo building $< ...
	@$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 
.cc.o:
	@echo building $< ...
	@$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

