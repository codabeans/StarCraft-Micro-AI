#version
MAJOR_VERSION=0
MINOR_VERSION=1
MICRO_VERSION=0
VERSION=$(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)

BWAPI=/home/faust/Documents/starcraft-ai/bwapi/bwapi

CAFFE=/home/faust/Documents/caffe

BUILD		:=	build
SOURCES		:=	source source/main source/gui $(BWAPI)/BWAPILIB $(BWAPI)/BWAPILIB/Source
BWAPI_SOURCES	:= 	$(BWAPI)/BWAPILIB $(BWAPI)/BWAPILIB/Source
INCLUDES	:=	include $(BWAPI)/include $(BWAPI)/include/BWAPI $(CAFFE)/include

#Target name
TARGET		:=	$(notdir $(CURDIR))

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
BWAPIFILES	:=	$(foreach dir,$(BWAPI_SOURCES),$(notdir $(wildcard $(dir)/*.cpp))))

# Object files
OFILES		:=	$(addprefix $(BUILD)/,$(CPPFILES:.cpp=.o))
BWAPIOFILES	:=	$(addprefix $(BWAPI)/$(BUILD)/,$(BWAPIFILES:.cpp=.o))

OBJECTS		:= 	$(OFILES) $(BWAPIOFILES)

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------

LD		 = 	$(CXX)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -I$(dir)\
					-I$(CURDIR)/$(dir)) \
					-I$(CURDIR)/$(BUILD)	
# Flags
SDL_CFLAGS=`sdl2-config --cflags` 

CPPFLAGS=-Wall -g -W -w -pedantic -ansi -lutil -std=c++11 $(SDL_CFLAGS)

# Linking flags
INCLUDEPATH += -I/usr/local/cuda/include

SDL_LDFLAGS=`sdl2-config --libs` 

LIBS += -lGL -lGLU -lSDL2_image -lopencv_core -lboost_system $(SDL_LDFLAGS)

LDFLAGS= $(LIBS)	

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

$(BUILD)/%.o:$(BWAPI)/BWAPILIB/%.cpp
	@mkdir -p $(BUILD)
	@echo building $^ ...
	@$(LD) $(INCLUDEPATH) $(CPPFLAGS) -c $^ -o $@ $(INCLUDE)

$(BUILD)/%.o:$(BWAPI)/BWAPILIB/Source/%.cpp
	@mkdir -p $(BUILD)
	@echo building $^ ...
	@$(LD) $(INCLUDEPATH) $(CPPFLAGS) -c $^ -o $@ $(INCLUDE)

$(BWAPI)/$(BUILD)/%.o:$(BWAPI)/BWAPILIB/%.cpp
	@mkdir -p $(BWAPI)/$(BUILD)
	@echo building BWAPILIB/$* ...
	@$(LD) $(INCLUDEPATH) $(CPPFLAGS) -c $^ -o $@ $(INCLUDE)


$(BWAPI)/$(BUILD)/%.o:$(BWAPI)/BWAPILIB/Source/%.cpp
	@mkdir -p $(BWAPI)/$(BUILD)
	@echo building BWAPILIB/Source/$* ...
	@$(LD) $(INCLUDEPATH) $(CPPFLAGS) -c $^ -o $@ $(INCLUDE)

sparcraft:$(OFILES)
	@mkdir -p $(BUILD)
	@echo linking ...
	@$(LD) $(OFILES) -o bin/$@  $(LDFLAGS)
	@echo "$(TARGET)-$(VERSION)"

bwapi:$(BWAPIOFILES)

all: $(OFILES) $(BWAPIOFILES)

clean:
	@echo clean ...
	@rm -rf $(BUILD)
	@rm -f $(TARGET)

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

