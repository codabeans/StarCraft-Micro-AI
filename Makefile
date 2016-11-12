#version
MAJOR_VERSION=0
MINOR_VERSION=1
MICRO_VERSION=0
VERSION=$(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)

BWAPI=/home/faust/Documents/starcraft-ai/bwapi/bwapi

CAFFE=/home/faust/Documents/caffe

BUILD		:=	build
SOURCES		:=	source source/main source/gui
BWAPI_SOURCES	:= 	$(BWAPI)/BWAPILIB $(BWAPI)/BWAPILIB/Source
INCLUDES	:=	include $(BWAPI)/include $(BWAPI)/include/BWAPI $(CAFFE_DIR)/include

TARGET		:=	$(CURDIR)/bin/$(notdir $(CURDIR))

FIND_CPP_FILES 	= 	$(notdir $(wildcard $(dir)/*.cpp))

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CPPFILES	:=	$(foreach dir,$(SOURCES),$(FIND_CPP_FILES))
BWAPICPPFILES	:=	$(foreach dir,$(BWAPI_SOURCES),$(FIND_CPP_FILES)))

export OFILES		:=	$(addprefix $(BUILD)/,$(CPPFILES:.cpp=.o))
export BWAPIOFILES	:=	$(addprefix $(BWAPI)/$(BUILD)/,$(BWAPICPPFILES:.cpp=.o))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------

LD		 = 	$(CXX)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------

export INCLUDE	:=	$(foreach dir,$(INCLUDES), \
					-I$(CURDIR)/$(dir)) \
					-I$(CURDIR)/$(BUILD)	
# Flags
SDL_CFLAGS=`sdl2-config --cflags` 

CPPFLAGS=-Wall -g -W -w -pedantic -ansi -lutil -std=c++11 $(SDL_CFLAGS)

# Linking flags
INCLUDEPATH +=-I$(BWAPI)/include -I$(BWAPI)/include/BWAPI -I$(BWAPI) -I$(CAFFE)/include -I /usr/local/cuda/include

SDL_LDFLAGS=`sdl2-config --libs` 

LIBS += -lGL -lGLU -lSDL2_image -lopencv_core -lboost_system $(SDL_LDFLAGS)

LDFLAGS= $(LIBS)

#---------------------------------------------------------------------------------
# Our make rules
# TODO: Find a way to use fewer rules
#---------------------------------------------------------------------------------

$(BUILD)/%.o:source/%.cpp
	@mkdir -p $(BUILD)
	@echo building $^ ...
	@$(LD) $(INCLUDEPATH) $(CPPFLAGS) -c $^ -o $@ $(INCLUDE)

$(BUILD)/%.o:source/main/%.cpp
	@mkdir -p $(BUILD)
	@echo building $^ ...
	@$(LD) $(INCLUDEPATH) $(CPPFLAGS) -c $^ -o $@ $(INCLUDE)

$(BUILD)/%.o:source/gui/%.cpp
	@mkdir -p $(BUILD)
	@echo building $^ ...
	@$(LD) $(INCLUDEPATH) $(CPPFLAGS) -c $^ -o $@ $(INCLUDE)

$(BWAPI)/$(BUILD)/%.o:$(BWAPI)/BWAPILIB/%.cpp
	@mkdir -p $(BWAPI)/$(BUILD)
	@echo building BWAPILIB/$* ... $(BWAPIO)
	@$(LD) $(INCLUDEPATH) $(CPPFLAGS) -c $^ -o $@ $(INCLUDE)

$(BWAPI)/$(BUILD)/%.o:$(BWAPI)/BWAPILIB/Source/%.cpp
	@mkdir -p $(BWAPI)/$(BUILD)
	@echo building BWAPILIB/Source/$* ...
	@$(LD) $(INCLUDEPATH) $(CPPFLAGS) $(LDFLAGS) -o $(TARGET)

#---------------------------------------------------------------------------------
# and finally our make commands
#---------------------------------------------------------------------------------

sparcraft:$(OFILES)
	@echo linking ...
	@$(CC) $(OBJECTS) -o bin/$@  $(LDFLAGS)

bwapi: $(BWAPIOFILES)

all: bwapi sparcraft

clean:
	@echo clean ...
	@rm -rf $(BUILD)
	@rm -f $(TARGET)
	@rm -rf $(BWAPI)/$(BUILD)

run:
	@echo Running $(TARGET)
	@./$(TARGET)

install:
	@sudo mkdir -p /opt/$(TARGET)
	@sudo cp -f $(TARGET) /opt/$(TARGET)/$(TARGET)
	@sudo ln -sf /opt/$(TARGET)/$(TARGET) /usr/local/bin/$(TARGET)

uninstall:
	@sudo rm -rf /opt/$(TARGET)
	@sudo rm -f /usr/local/bin/$(TARGET)
