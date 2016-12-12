PROJECT := caffe


BWAPI		=		/home/faust/Documents/starcraft-ai/bwapi
CAFFE		=		/home/faust/Documents/caffe

#If you don't have cudnn installed, set to 0
USE_CUDNN	=		1

SOURCE		:= 		source
BUILD		:=		build
BWAPI_BUILD	:=		$(BWAPI)/bwapi/build/
INCLUDE		:= 		include

#Target name
TARGET		:=		bin/Sparcraft

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

BWAPI_SRC_DIRS	:= 	$(shell find $(BWAPI)/bwapi/BWAPILIB/ -type d -exec bash -c "find {} -maxdepth 1 \
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
export INCLUDE		=		$(foreach dir,$(INCLUDES), -I$(CURDIR)/$(dir)) \
							-I$(CURDIR)/$(BUILD)

# Linking flags
INCLUDEPATH = 		-I$(BWAPI)/bwapi \
					-I$(BWAPI)/bwapi/include \
					-I$(BWAPI)/bwapi/include/BWAPI \
					-I$(CAFFE)/include \
					-I/usr/local/cuda/include

SDL_LDFLAGS	=		`sdl2-config --libs`
SDL_CFLAGS	=		`sdl2-config --cflags`
CXXFLAGS	=		-O3 -w -Wall -std=c++11 $(SDL_CFLAGS)

LIBS		= 		-lGL -lGLU -lSDL2_image \
					-lopencv_core -lopencv_highgui \
					-lopencv_imgproc -lboost_system \
					-lglog -L$(CAFFE)/build/lib \
 					$(SDL_LDFLAGS) -lboost_filesystem \
					-lcaffe



LDFLAGS		=		$(LIBS)

ifeq ($(USE_CUDNN), 1)
	LDFLAGS += 		-lcudnn
endif

all: build_dirs lib link

build_dirs: $(ALL_BUILD_DIRS)

lib: $(BWAPI_OBJS) $(OBJS) $(OBJS)

link:
	@echo linking ...
	@$(LD) $(OBJS) $(BWAPI_OBJS) $(LDFLAGS) -o $(TARGET)

$(ALL_BUILD_DIRS):
	@mkdir -p $@

$(BUILD)/%.o:$(SOURCE)/%.cpp
	@ echo CXX $^
	@$(CXX) $(INCLUDEPATH) $(CXXFLAGS) -c $^ -o $@ $(INCLUDE)

$(BWAPI_BUILD)%.o:$(BWAPI)/bwapi/BWAPILIB/%.cpp
	@ echo CXX BWAPI/$*
	@$(CXX) $(INCLUDEPATH) $(CXXFLAGS) -c $^ -o $@ $(INCLUDE)


# Cleanup
clean:
	@echo clean ...
	@rm -rf $(BUILD)
	@rm -rf $(BWAPI_BUILD)
	@rm -f $(TARGET)
