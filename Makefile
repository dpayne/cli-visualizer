# ====================
#  LIBRARY DEFINITIONS
# ====================

TARGET= vis

###############################################################################
##  SETTINGS                                                                 ##
###############################################################################

CC = g++
DIR=$(shell pwd)
BUILD_DIR = $(DIR)/build

# Override optimizations via: make OPT_LEVEL=n
OPT_LEVEL = 3

# Make-local Compiler Flags
CC_FLAGS = -std=c++14 -Wall -Werror -O$(OPT_LEVEL)
CC_FLAGS += -march=native
CC_FLAGS += -fno-common -fno-strict-aliasing -fno-omit-frame-pointer -finline-functions

ifeq ($(OS),Darwin)
CC_FLAGS += -D_OS_OSX
else
CC_FLAGS += -D_LINUX
endif

# Linker flags
LD_FLAGS = $(LDFLAGS) -fno-omit-frame-pointer -ljemalloc

ifeq ($(OS),Darwin)
LD_FLAGS += -undefined dynamic_lookup
endif

# DEBUG Settings
ifdef DEBUG
OPT_LEVEL=0
CC_FLAGS += -pg -fprofile-arcs -ftest-coverage -g2
LD_FLAGS += -pg -fprofile-arcs -lgcov
endif

# Include Paths
INCLUDE_PATH = -I/usr/local/include -I$(DIR)/include -I$(DIR)/src

# Lib Paths
LIB_PATH = -L/usr/local/lib

# Libs
LIBS = -ljemalloc


###############################################################################
##  OBJECTS                                                                  ##
###############################################################################

SOURCES= $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)

HEADERS= $(wildcard include/*.h) $(wildcard include/*/*.h) $(wildcard src/*.h) $(wildcard src/*/*.h) $(wildcard src/*/*/*.h)

OBJECTS= $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.cpp=.o)))

###############################################################################
##  MAIN TARGETS                                                             ##
###############################################################################

all: prepare build

.PHONY: prepare
prepare:
	mkdir -p $(BUILD_DIR)

.PHONY: build
build: $(TARGET)

.PHONY:clean
clean:
	@rm -rf $(BUILD_DIR)

install:
	cp $(BUILD_DIR)/$(TARGET) /usr/local/bin/

###############################################################################
##  BUILD TARGETS                                                            ##
###############################################################################

$(TARGET): $(SOURCES)
	$(CC) $(CC_FLAGS) $(LDFLAGS) $(INCLUDE_PATH) $(LIB_PATH) -o $(BUILD_DIR)/$(TARGET) $(SOURCES) $(LIBS)
