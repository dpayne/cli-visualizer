# ====================
#  LIBRARY DEFINITIONS
# ====================

TARGET= vis
TEST_TARGET= run_tests
PERF_TEST_TARGET= run_perf_tests

###############################################################################
##  SETTINGS                                                                 ##
###############################################################################

CLANG := $(shell which clang)
CCACHE := $(shell which ccache)
OS= $(shell uname)

#prefer clang over g++
ifdef VIS_COMPILER
CXX=$(VIS_COMPILER)
endif

ifndef PREFIX
PREFIX=/bin/
endif

ifndef VIS_COMPILER_ARCH
VIS_COMPILER_ARCH=native
endif

DIR=$(shell pwd)
BUILD_DIR = $(DIR)/build
BUILD_TEST_DIR = $(DIR)/build_tests
BUILD_PERF_TEST_DIR = $(DIR)/build_perf_tests

# Override optimizations via: make OPT_LEVEL=n
OPT_LEVEL = 3

# Make-local Compiler Flags
CXX_FLAGS = -std=c++14
CXX_FLAGS += -O$(OPT_LEVEL)
CXX_FLAGS += -march=$(VIS_COMPILER_ARCH)
CXX_FLAGS += -ffast-math
CXX_FLAGS += -fno-omit-frame-pointer
CXX_FLAGS += -D__extern_always_inline=inline

TEST_CCACHE_CLANG=ccache clang++
TEST_CLANG=clang++

ALL_WARNINGS=-Werror -Weverything -Wno-variadic-macros -Wno-format-nonliteral -Wno-global-constructors -Wno-exit-time-destructors -Wno-padded -Wno-reserved-id-macro -Wno-gnu-zero-variadic-macro-arguments -Wno-c++98-compat -Wno-documentation-unknown-command
# Only turn on extra warnings for clang since g++ does not support -Weverything
ifeq ($(CXX),$(TEST_CLANG))
CXX_FLAGS += $(ALL_WARNINGS)
else
ifeq ($(CXX),$(TEST_CCACHE_CLANG))
CXX_FLAGS += $(ALL_WARNINGS)
endif
endif

#perf tests should not have many warnings or error out on warning
PERF_TEST_CXX_FLAGS = -std=c++14
PERF_TEST_CXX_FLAGS += -O$(OPT_LEVEL)
PERF_TEST_CXX_FLAGS += -march=$(VIS_COMPILER_ARCH)
PERF_TEST_CXX_FLAGS += -ffast-math
PERF_TEST_CXX_FLAGS += -fno-omit-frame-pointer
PERF_TEST_CXX_FLAGS += -ggdb -g2

ifeq ($(OS),Darwin)
CXX_FLAGS += -dynamic -D_OS_OSX -D_XOPEN_SOURCE_EXTENDED

else
CXX_FLAGS += -D_LINUX
endif

# Linker flags
LD_FLAGS = $(LDFLAGS) -fno-omit-frame-pointer
PERF_TEST_LD_FLAGS = -fno-omit-frame-pointer

ifeq ($(OS),Darwin)
LD_FLAGS += -D_XOPEN_SOURCE_EXTENDED
#El Capitan has SIP, so things need to live in usr/local/bin now.
PREFIX=/usr/local/bin/
endif

# DEBUG Settings
ifdef DEBUG
OPT_LEVEL=0
CXX_FLAGS += -ggdb -g2 -DVIS_LOG_DEBUG
LD_FLAGS += -DVIS_LOG_DEBUG
endif

# Clang sanitize options
ifdef SANITIZE
#to get symbols from clang sanitize, run "export ASAN_SYMBOLIZER_PATH=<path_to_llvm_symbolizer>" on Arch Linux symbolizer is usually at"/usr/bin/llvm-symbolizer"
CXX_FLAGS += -fsanitize=$(SANITIZE)
LD_FLAGS += -fsanitize=$(SANITIZE)
endif

# Include Paths
INCLUDE_PATH = -I/usr/local/include -I$(DIR)/include -I$(DIR)/src
TEST_INCLUDE_PATH = -I/usr/include
PERF_TEST_INCLUDE_PATH = -I/usr/include

# Lib Paths
LIB_PATH = -L/usr/local/lib

# Libs
LIBS = -lfftw3 -lm -lstdc++
TEST_LIBS = -lgtest -lpthread
PERF_TEST_LIBS = -lbenchmark -lpthread

ifneq ("$(wildcard /usr/lib/libpulse.so)","")
ENABLE_PULSE=1
endif

ifdef ENABLE_PULSE
CXX_FLAGS += -D_ENABLE_PULSE
LD_FLAGS += -D_ENABLE_PULSE
LIBS += -lpulse -lpulse-simple
endif

#if this box has an older version of ncurses
ifneq ("$(wildcard /usr/include/ncursesw/ncurses.h)","")
	LIBS += -lncursesw
	CXX_FLAGS += -DNCURSESW
	LD_FLAGS += -DNCURSESW
else
	LIBS += -lncurses
endif

#use jemalloc if available
ifneq ("$(wildcard /usr/lib/libjemalloc.so)","")
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

PERF_TEST_SOURCES= $(wildcard perf_tests/*.cpp) $(wildcard perf_tests/*/*.cpp) $(wildcard perf_tests/*/*/*.cpp)

PERF_TEST_OBJECTS= $(addprefix $(BUILD_PERF_TEST_DIR)/,$(notdir $(PERF_TEST_SOURCES:.cpp=.o))) $(filter-out $(BUILD_DIR)/$(TARGET).o,$(OBJECTS))

VPATH= $(dir $(wildcard src/*/ src/*/*/)) $(dir $(wildcard tests/*/ tests/*/*/)) $(dir $(wildcard perf_tests/*/ perf_tests/*/*/))

###############################################################################
##  MAIN TARGETS                                                             ##
###############################################################################

all: prepare build safe_fifo

test: tests

tests: prepare_tests build_tests

perf_tests: prepare_perf_tests build_perf_tests

.PHONY: prepare
prepare:
	mkdir -p $(BUILD_DIR)
	rm -f $(BUILD_DIR)/$(TARGET)

.PHONY: prepare_tests
prepare_tests:
	mkdir -p $(BUILD_TEST_DIR)
	rm -f $(BUILD_TEST_DIR)/$(TEST_TARGET)

.PHONY: prepare_perf_tests
prepare_perf_tests:
	mkdir -p $(BUILD_PERF_TEST_DIR)
	rm -f $(BUILD_PERF_TEST_DIR)/$(PERF_TEST_TARGET)

.PHONY: build
build: $(OBJECTS) $(TARGET)

.PHONY: build build_tests run_tests
build_tests: $(TEST_OBJECTS) $(TEST_TARGET)

.PHONY: build build_perf_tests run_perf_tests
build_perf_tests: $(PERF_TEST_OBJECTS) $(PERF_TEST_TARGET)

.PHONY:clean
clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BUILD_TEST_DIR)
	@rm -rf $(BUILD_PERF_TEST_DIR)
	@rm -rf bin/safe_fifo

.PHONY:safe_fifo
safe_fifo:
	gcc bin/safe_fifo.c -o bin/safe_fifo

uninstall:
	@rm -f $(PREFIX)/$(TARGET)
	@rm -f $(PREFIX)/safe_fifo

install:
	cp $(BUILD_DIR)/$(TARGET) $(PREFIX)
#	cp bin/safe_fifo $(PREFIX)

###############################################################################
##  BUILD TARGETS                                                            ##
###############################################################################

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(INCLUDE_PATH) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $(LDFLAGS) $(INCLUDE_PATH) $(LIB_PATH) -o $(BUILD_DIR)/$(TARGET) $(OBJECTS) $(LIBS)

$(BUILD_TEST_DIR)/%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(INCLUDE_PATH) $(TEST_INCLUDE_PATH) -c $< -o $@

$(TEST_TARGET): $(OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(CXX_FLAGS) $(LDFLAGS) $(INCLUDE_PATH) $(TEST_INCLUDE_PATH) $(LIB_PATH) -o $(BUILD_TEST_DIR)/$(TEST_TARGET) $(TEST_OBJECTS) $(LIBS) $(TEST_LIBS)
	$(BUILD_TEST_DIR)/$(TEST_TARGET)

$(BUILD_PERF_TEST_DIR)/%.o: %.cpp
	$(CXX) $(PERF_TEST_CXX_FLAGS) $(PERF_TEST_LD_FLAGS) $(INCLUDE_PATH) $(PERF_TEST_INCLUDE_PATH) -c $< -o $@

$(PERF_TEST_TARGET): $(OBJECTS) $(PERF_TEST_OBJECTS)
	$(CXX) $(PERF_TEST_CXX_FLAGS) $(PERF_TEST_LD_FLAGS) $(INCLUDE_PATH) $(PERF_TEST_INCLUDE_PATH) $(LIB_PATH) -o $(BUILD_PERF_TEST_DIR)/$(PERF_TEST_TARGET) $(PERF_TEST_OBJECTS) $(LIBS) $(PERF_TEST_LIBS)
	$(BUILD_PERF_TEST_DIR)/$(PERF_TEST_TARGET)

clang_tidy: $(HEADERS) $(SOURCES) $(TEST_SOURCES) $(PERF_TEST_SOURCES)
	clang-tidy -checks=${CLANG_TIDY_CHECKS} $? -- -x c++ -std=c++14 -I$(INCLUDE_PATH)

clang_format: $(HEADERS) $(SOURCES) $(TEST_SOURCES) $(PERF_TEST_SOURCES)
	clang-format -i $?
