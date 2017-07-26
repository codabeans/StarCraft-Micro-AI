PROJECT := 	Micro-AI

BWAPI		=		/home/faust/Documents/StarCraft-AI/bwapi

SOURCE		:= 		source
BUILD		:=		build
BWAPI_BUILD	:=		$(BWAPI)/bwapi/build/
INCLUDE		:= 		include
CAFFE		:=		/home/faust/Documents/caffe

#Target name
TARGET		:=		bin/StarCraft-Micro-AI

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
SRCS 		:= 		$(shell find $(SOURCE)/ -name "*.cpp")
OBJS 		:= 		$(addprefix $(BUILD)/, ${SRCS:$(SOURCE)/%.cpp=%.o})

BWAPI_SRCS 	:= 		$(shell find $(BWAPI)/bwapi/BWAPILIB/ -name "*.cpp" -printf '%P\n')
BWAPI_OBJS 	:= 		$(addprefix $(BWAPI)/bwapi/build/, ${BWAPI_SRCS:%.cpp=%.o})
BWAPISRCS 	:=		$(addprefix $(BWAPI)/bwapi/BWAPILIB/, ${BWAPI_SRCS})

#---------------------------------------------------------------------------------
# make a list of all the build dirs to make the folders
#---------------------------------------------------------------------------------
SRC_DIRS 	:= 		$(shell find $(SOURCE)/ -type d -exec bash -c "find {} -maxdepth 1 \
				\( -name '*.cpp'  \) | grep -q ." \; -printf '%P\n')

BUILD_DIRS	=		$(BUILD)
BUILD_DIRS 	+= 		$(addprefix $(BUILD)/, $(SRC_DIRS))

SRCDIRS 	:=		$(addprefix $(SOURCE)/, ${SRC_DIRS})

BWAPI_SRC_DIRS	:= 		$(shell find $(BWAPI)/bwapi/BWAPILIB/ -type d -exec bash -c "find {} -maxdepth 1 \
				\( -name '*.cpp' \) | grep -q ." \; -printf '%P\n')

BWAPI_BUILD_DIRS	=		$(BWAPI)/bwapi/build/
BWAPI_BUILD_DIRS	+=		$(addprefix $(BWAPI)/bwapi/build/, ${BWAPI_SRC_DIRS})

ALL_BUILD_DIRS		:=		$(BUILD_DIRS) $(BWAPI_BUILD_DIRS)

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------

export LD	:=		$(CXX)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
# Linking flags
INCLUDEPATH 		= 		-I$(BWAPI)/bwapi \
					-I$(BWAPI)/bwapi/include \
					-I$(BWAPI)/bwapi/include/BWAPI \
					-I/usr/include/SDL2 \
					-I/usr/local/cuda/include \
					-I/usr/local/include
					

SDL_LDFLAGS	=		`sdl2-config --libs`
SDL_CFLAGS	=		`sdl2-config --cflags`
CXXFLAGS		=		-w -Wall -g -std=c++11 $(SDL_CFLAGS)

LIBS		= 			-lGL -lm -lGLU -lSDL2 -lSDL2_image -lstdc++ $(SDL_LDFLAGS)

LDFLAGS		=		$(LIBS)

all: build_dirs lib link

build_dirs: $(ALL_BUILD_DIRS)

lib: $(BWAPI_OBJS) $(OBJS)

link:
	@echo linking ...
	@mkdir -p bin
	@$(LD) $(BWAPI_OBJS) $(OBJS) $(LDFLAGS) -o $(TARGET)

$(ALL_BUILD_DIRS):
	@mkdir -p $@

$(BUILD)/%.o:$(SOURCE)/%.cpp
	@ echo CXX $^
	@$(CXX) $(INCLUDEPATH) $(CXXFLAGS) -c $^ -o $@

$(BWAPI_BUILD)%.o:$(BWAPI)/bwapi/BWAPILIB/%.cpp
	@ echo CXX BWAPI/$*
	@$(CXX) $(INCLUDEPATH) $(CXXFLAGS) -c $^ -o $@

# Cleanup
clean:
	@echo clean ...
	@rm -rf $(BUILD)
	@rm -f $(TARGET)
