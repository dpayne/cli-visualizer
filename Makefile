# ====================
#  LIBRARY DEFINITIONS
# ====================

$(warning Building with Makefile has been deprecated!)
$(warning Please use the cmake build process instead.)

TARGET= vis
TEST_TARGET= run_tests
PERF_TEST_TARGET= run_perf_tests

###############################################################################
##  SETTINGS                                                                 ##
###############################################################################

OS= $(shell uname)

PREFIX ?= /usr/local

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
CXX_FLAGS += -D_XOPEN_SOURCE_EXTENDED

#perf tests should not have many warnings or error out on warning
PERF_TEST_CXX_FLAGS = -std=c++14
PERF_TEST_CXX_FLAGS += -O$(OPT_LEVEL)
PERF_TEST_CXX_FLAGS += -march=$(VIS_COMPILER_ARCH)
PERF_TEST_CXX_FLAGS += -ffast-math
PERF_TEST_CXX_FLAGS += -fno-omit-frame-pointer
PERF_TEST_CXX_FLAGS += -ggdb -g2

# Libs
LIBS = -lfftw3 -lm -lstdc++
TEST_LIBS = -lgtest -lpthread
PERF_TEST_LIBS = -lbenchmark -lpthread

# Mac OS
ifeq ($(OS),Darwin)
CXX_FLAGS += -dynamic -D_OS_OSX

# Linux
else
CXX_FLAGS += -D_LINUX

CHECK_LTINFOW=$(shell ldconfig -p | grep tinfow)
CHECK_LTINFO=$(shell ldconfig -p | grep tinfo)
ifeq ($(strip $(CHECK_LTINFOW)),)
ifeq ($(strip $(CHECK_LTINFO)),)
else
$(info Using ltinfo)
LIBS += -ltinfo
endif
else
$(info Using ltinfow)
LIBS += -ltinfow
endif

ifndef ENABLE_PULSE
CHECK_PULSE=$(shell ldconfig -p | grep libpulse-simple)
ifeq ($(strip $(CHECK_PULSE)),)
$(info Could not find libpulse-simple, disabling pulseaudio support)
else
$(info Found libpulse-simple, enabling pulseaudio support)
ENABLE_PULSE=1
endif
endif

endif

# Linker flags
LD_FLAGS = $(LDFLAGS) -fno-omit-frame-pointer
PERF_TEST_LD_FLAGS = -fno-omit-frame-pointer

ifeq ($(OS),Darwin)
LD_FLAGS += -D_XOPEN_SOURCE_EXTENDED
endif

# DEBUG Settings
ifdef DEBUG
OPT_LEVEL=1
CXX_FLAGS += -ggdb -g2 -DVIS_LOG_DEBUG
LD_FLAGS += -DVIS_LOG_DEBUG
endif

# Clang sanitize options
ifdef SANITIZE
#to get symbols from clang sanitize, run "export ASAN_SYMBOLIZER_PATH=<path_to_llvm_symbolizer>" on Arch Linux symbolizer is usually at"/usr/bin/llvm-symbolizer"
#to enable stricter checks set ASAN_OPTIONS, for example "export ASAN_OPTIONS='detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1:detect_leaks=1:color=always'"
CXX_FLAGS += -fsanitize=$(SANITIZE)
LD_FLAGS += -fsanitize=$(SANITIZE)
endif

# Include Paths

# Lib Paths
ifdef VIS_NCURSES_LIB_PATH
	LIB_PATH = -L"${VIS_NCURSES_LIB_PATH}" -L/usr/local/lib
	INCLUDE_PATH = -I"${VIS_NCURSES_INCLUDE_PATH}" -I"$(DIR)/include" -I"$(DIR)/src"
else
	INCLUDE_PATH = -I/usr/local/include -I"$(DIR)/include" -I"$(DIR)/src"
	LIB_PATH = -L/usr/local/lib
endif

TEST_INCLUDE_PATH = ${INCLUDE_PATH}
PERF_TEST_INCLUDE_PATH = ${INCLUDE_PATH}

ifeq ($(ENABLE_PULSE),1)
CXX_FLAGS += -D_ENABLE_PULSE
LD_FLAGS += -D_ENABLE_PULSE
LIBS += -lpulse -lpulse-simple
endif

ifdef VIS_NCURSES_LIB_PATH
	LIBS += -lncursesw -ldl
	CXX_FLAGS += -DNCURSESW
	LD_FLAGS += -DNCURSESW
else
#if this box has an older version of ncurses
ifneq ("$(wildcard /usr/include/ncursesw/ncurses.h)","")
	LIBS += -lncursesw
	CXX_FLAGS += -DNCURSESW
	LD_FLAGS += -DNCURSESW
else
ifdef VIS_NCURSESW
	LIBS += -lncursesw
	CXX_FLAGS += -DNCURSESW
	LD_FLAGS += -DNCURSESW
else
	LIBS += -lncurses
endif
endif
endif

#use jemalloc if available
ifneq ("$(wildcard /usr/lib/libjemalloc.so)","")
LIBS += -ljemalloc
endif

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
	mkdir -p "$(BUILD_DIR)"
	rm -f "$(BUILD_DIR)/$(TARGET)"

.PHONY: prepare_tests
prepare_tests:
	mkdir -p "$(BUILD_TEST_DIR)"
	rm -f "$(BUILD_TEST_DIR)/$(TEST_TARGET)"

.PHONY: prepare_perf_tests
prepare_perf_tests:
	mkdir -p "$(BUILD_PERF_TEST_DIR)"
	rm -f "$(BUILD_PERF_TEST_DIR)/$(PERF_TEST_TARGET)"

.PHONY: build
build: $(OBJECTS) $(TARGET)

.PHONY: build build_tests run_tests
build_tests: $(TEST_OBJECTS) $(TEST_TARGET)

.PHONY: build build_perf_tests run_perf_tests
build_perf_tests: $(PERF_TEST_OBJECTS) $(PERF_TEST_TARGET)

.PHONY:clean
clean:
	@rm -rf "$(BUILD_DIR)"
	@rm -rf "$(BUILD_TEST_DIR)"
	@rm -rf "$(BUILD_PERF_TEST_DIR)"
	@rm -rf bin/safe_fifo

.PHONY:safe_fifo
safe_fifo:
	gcc bin/safe_fifo.c -o bin/safe_fifo

uninstall:
	@rm -f $(PREFIX)/$(TARGET)
	@rm -f $(PREFIX)/safe_fifo

install:
	install -Dm0755 $(BUILD_DIR)/vis $(DESTDIR)/$(PREFIX)/bin/$(TARGET)
#	cp bin/safe_fifo "$(PREFIX)"

###############################################################################
##  BUILD TARGETS                                                            ##
###############################################################################

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(INCLUDE_PATH) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $(LDFLAGS) $(INCLUDE_PATH) $(LIB_PATH) -o "$(BUILD_DIR)/$(TARGET)" $(OBJECTS) $(LIBS)

$(BUILD_TEST_DIR)/%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(INCLUDE_PATH) $(TEST_INCLUDE_PATH) -c $< -o $@

$(TEST_TARGET): $(OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(CXX_FLAGS) $(LDFLAGS) $(INCLUDE_PATH) $(TEST_INCLUDE_PATH) $(LIB_PATH) -o "$(BUILD_TEST_DIR)/$(TEST_TARGET)" $(TEST_OBJECTS) $(LIBS) $(TEST_LIBS)
	"$(BUILD_TEST_DIR)/$(TEST_TARGET)"

$(BUILD_PERF_TEST_DIR)/%.o: %.cpp
	$(CXX) $(PERF_TEST_CXX_FLAGS) $(PERF_TEST_LD_FLAGS) $(INCLUDE_PATH) $(PERF_TEST_INCLUDE_PATH) -c $< -o $@

$(PERF_TEST_TARGET): $(OBJECTS) $(PERF_TEST_OBJECTS)
	$(CXX) $(PERF_TEST_CXX_FLAGS) $(PERF_TEST_LD_FLAGS) $(INCLUDE_PATH) $(PERF_TEST_INCLUDE_PATH) $(LIB_PATH) -o "$(BUILD_PERF_TEST_DIR)/$(PERF_TEST_TARGET)" $(PERF_TEST_OBJECTS) $(LIBS) $(PERF_TEST_LIBS)
	"$(BUILD_PERF_TEST_DIR)/$(PERF_TEST_TARGET)"

clang_tidy: $(HEADERS) $(SOURCES)
	clang-tidy $? -- -x c++ -std=c++14 -I$(INCLUDE_PATH)

clang_format: $(HEADERS) $(SOURCES) $(TEST_SOURCES) $(PERF_TEST_SOURCES)
	clang-format -i $?
