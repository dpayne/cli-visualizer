# ====================
#  LIBRARY DEFINITIONS
# ====================

TARGET= vis
TEST_TARGET= run_tests

###############################################################################
##  SETTINGS                                                                 ##
###############################################################################

CLANG := $(shell which clang)
CCACHE := $(shell which ccache)
OS= $(shell uname)

#prefer clang over g++
ifneq ($(CLANG),)
COMPILER=clang++
else
COMPILER=g++
endif

#use ccache if available
ifneq ($(CCACHE),)
CC=ccache $(COMPILER)
else
CC=$(COMPILER)
endif

DIR=$(shell pwd)
BUILD_DIR = $(DIR)/build
BUILD_TEST_DIR = $(DIR)/build_tests

# Override optimizations via: make OPT_LEVEL=n
OPT_LEVEL = 3

# Make-local Compiler Flags
CC_FLAGS = -std=c++14
CC_FLAGS += -Weverything -Wno-variadic-macros -Wno-format-nonliteral -Wno-global-constructors -Wno-exit-time-destructors -Wno-padded -Wno-reserved-id-macro -Wno-gnu-zero-variadic-macro-arguments -Wno-c++98-compat
CC_FLAGS += -O$(OPT_LEVEL)
CC_FLAGS += -march=native
CC_FLAGS += -ffast-math
CC_FLAGS += -fno-omit-frame-pointer -ggdb -g2

ifeq ($(OS),Darwin)
CC_FLAGS += -dynamic -D_OS_OSX -D_XOPEN_SOURCE_EXTENDED
else
CC_FLAGS += -D_LINUX -Werror
endif

# Linker flags
LD_FLAGS = $(LDFLAGS) -fno-omit-frame-pointer

ifeq ($(OS),Darwin)
LD_FLAGS += -D_XOPEN_SOURCE_EXTENDED
else
LD_FLAGS += -Werror
endif

# DEBUG Settings
ifdef DEBUG
OPT_LEVEL=0
CC_FLAGS += -g2 -DVIS_LOG_DEBUG
LD_FLAGS += -DVIS_LOG_DEBUG
endif

# Clang sanitize options
ifdef SANITIZE
CC_FLAGS += -fsanitize=$(SANITIZE)
LD_FLAGS += -fsanitize=$(SANITIZE)
endif

# Include Paths
INCLUDE_PATH = -I/usr/local/include -I$(DIR)/include -I$(DIR)/src
TEST_INCLUDE_PATH = -I/usr/include

# Lib Paths
LIB_PATH = -L/usr/local/lib

# Libs
LIBS = -lncurses -lfftw3
TEST_LIBS = -lgtest

ifneq ($(OS),Darwin)
LIBS += -ljemalloc
endif

#clang tidy checks
CLANG_TIDY_CHECKS=clang-analyzer-*,modernize-*,readability-*,misc-*,cppcoreguidelines-*,google-*,-google-readability-namespace-comments

###############################################################################
##  OBJECTS                                                                  ##
###############################################################################

SOURCES= $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)

HEADERS= $(wildcard include/*.h) $(wildcard include/*/*.h) $(wildcard src/*.h) $(wildcard src/*/*.h) $(wildcard src/*/*/*.h)

OBJECTS= $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.cpp=.o)))

TEST_SOURCES= $(wildcard tests/*.cpp) $(wildcard tests/*/*.cpp) $(wildcard tests/*/*/*.cpp)

TEST_OBJECTS= $(addprefix $(BUILD_TEST_DIR)/,$(notdir $(TEST_SOURCES:.cpp=.o))) $(filter-out $(BUILD_DIR)/$(TARGET).o,$(OBJECTS))

VPATH= $(dir $(wildcard src/*/ src/*/*/)) $(dir $(wildcard tests/*/ tests/*/*/))

###############################################################################
##  MAIN TARGETS                                                             ##
###############################################################################

all: prepare build

tests: prepare_tests build_tests

.PHONY: prepare
prepare:
	mkdir -p $(BUILD_DIR)
	rm -f $(BUILD_DIR)/$(TARGET)

.PHONY: prepare_tests
prepare_tests:
	mkdir -p $(BUILD_TEST_DIR)
	rm -f $(BUILD_TEST_DIR)/$(TEST_TARGET)

.PHONY: build
build: $(OBJECTS) $(TARGET)

.PHONY: build build_tests run_tests
build_tests: $(TEST_OBJECTS) $(TEST_TARGET)

.PHONY:clean
clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BUILD_TEST_DIR)

install:
	cp $(BUILD_DIR)/$(TARGET) /usr/local/bin/

###############################################################################
##  BUILD TARGETS                                                            ##
###############################################################################

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CC_FLAGS) $(LD_FLAGS) $(INCLUDE_PATH) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CC_FLAGS) $(LDFLAGS) $(INCLUDE_PATH) $(LIB_PATH) -o $(BUILD_DIR)/$(TARGET) $(OBJECTS) $(LIBS)

$(BUILD_TEST_DIR)/%.o: %.cpp
	$(CC) $(CC_FLAGS) $(LD_FLAGS) $(INCLUDE_PATH) $(TEST_INCLUDE_PATH) -c $< -o $@

$(TEST_TARGET): $(OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CC_FLAGS) $(LDFLAGS) $(INCLUDE_PATH) $(TEST_INCLUDE_PATH) $(LIB_PATH) -o $(BUILD_TEST_DIR)/$(TEST_TARGET) $(TEST_OBJECTS) $(LIBS) $(TEST_LIBS)
	$(BUILD_TEST_DIR)/$(TEST_TARGET)

clang_tidy: $(HEADERS) $(SOURCES) $(TEST_SOURCES)
	clang-tidy -checks=${CLANG_TIDY_CHECKS} $? -- -x c++ -std=c++14 -I$(INCLUDE_PATH)

clang_format: $(HEADERS) $(SOURCES) $(TEST_SOURCES)
	clang-format -i $?
