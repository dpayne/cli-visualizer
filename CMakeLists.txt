cmake_minimum_required(VERSION 3.5.0)

project(vis)
set(vis_version 1.0)
set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)
set(PROJECT_PERF_TEST_NAME ${PROJECT_NAME}_perf_test)
option(VIS_WITH_TESTS "Compile tests executables" OFF)
option(VIS_WITH_PERF_TESTS "Compile tests executables" OFF)
option(VIS_RUN_CLANG_FORMAT "Run clang formatter" OFF)
option(VIS_RUN_CLANG_TIDY "Run clang tidy" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif()

set(PROJECT_VERSION ${vis_version})
project(${PROJECT_NAME} VERSION ${vis_version} LANGUAGES CXX C)

if(NOT DEFINED VIS_COMPILER_ARCH)
    set(VIS_COMPILER_ARCH "native")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -D__extern_always_inline=inline -D_XOPEN_SOURCE_EXTENDED")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -g3 -ffast-math -march=x86-64 -mtune=generic -DVIS_LOG_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -g1 -O3 -march=${VIS_COMPILER_ARCH} -fno-omit-frame-pointer -DNDEBUG")

set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} ${VIS_DEFAULT_FLAGS} -fno-omit-frame-pointer -D__extern_always_inline=inline -D_XOPEN_SOURCE_EXTENDED")
set(CMAKE_LD_FLAGS_DEBUG "${CMAKE_LD_FLAGS_DEBUG} -ggdb -g3 -ffast-math -march=x86-64 -mtune=generic -DVIS_LOG_DEBUG")
set(CMAKE_LD_FLAGS_RELEASE "${CMAKE_LD_FLAGS_RELEASE} -ffast-math -g1 -O3 -march=${VIS_COMPILER_ARCH} -fno-omit-frame-pointer -DNDEBUG")

if(FORCE_NCURSESW AND APPLE)
    set(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES} /usr/local/opt/ncurses/include")
endif()

include(CheckIncludeFile)
check_include_file("ncursesw/ncurses.h" HAVE_NCURSESW_H)
if(HAVE_NCURSESW_H)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNCURSESW")
endif()

if(DEFINED VIS_SANITIZER)
    # compile with O1 to cause a little more havoc
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -fsanitize=${VIS_SANITIZER}")
    set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -O1 -fsanitize=${VIS_SANITIZER}")
endif()

# OS specific flags
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L/usr/local/lib/ -I/usr/local/include/ -D _OS_OSX")
    set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -L/usr/local/lib/ -D _OS_OSX")

    # Enable runtime search path support for dynamic libraries on OSX
    set(CMAKE_MACOSX_RPATH 1)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _LINUX")
    set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -ldl -D _LINUX")
endif()

# Enable color diagnostics, if the compiler supports it
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-fcolor-diagnostics CXX_COLOR_SUPPORTED)
if(CXX_COLOR_SUPPORTED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
endif()

# Compiler specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-ignored-qualifiers -Wno-unused-command-line-argument")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wshadow -Wextra -Wno-sign-conversion")
endif()

# Use ccache when available
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

########################################################################
# Setup libraries
########################################################################
# Find correct ncurses lib
if (FORCE_NCURSESW AND APPLE)
    set(LIB_NCURSES "/usr/local/opt/ncurses/lib/libncursesw.dylib")
else()
    find_library(LIB_NCURSES NAMES ncursesw ncurses)
endif()

set(DYNAMIC_LIBRARIES -lfftw3 -lm -lstdc++ -lrt ${LIB_NCURSES})

# Add jemalloc if avaiable
find_library(JEMALLOC_FOUND NAMES jemalloc)
if(JEMALLOC_FOUND AND NOT APPLE)
  set(DYNAMIC_LIBRARIES ${DYNAMIC_LIBRARIES} jemalloc)
endif()

find_library(LIB_TINFO NAMES tinfow tinfo)
if(LIB_TINFO)
    set(DYNAMIC_LIBRARIES ${DYNAMIC_LIBRARIES} ${LIB_TINFO})
endif()

find_library(PORT_FOUND NAMES portaudio)
if(PORT_FOUND)
  set(DYNAMIC_LIBRARIES ${DYNAMIC_LIBRARIES} portaudio)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_ENABLE_PORT")
  set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -D_ENABLE_PORT")
endif()

find_library(PULSE_FOUND NAMES pulse)
if(PULSE_FOUND)
  set(DYNAMIC_LIBRARIES ${DYNAMIC_LIBRARIES} pulse pulse-simple)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_ENABLE_PULSE")
  set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -D_ENABLE_PULSE")
endif()

########################################################################
# Setup target
########################################################################

include(project_files.cmake)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

link_directories(${PROJECT_SOURCE_DIR}/src/)
add_executable(${PROJECT_NAME} "")

set( HEADER_FILES
    ${VIS_PROJECT_HEADER_FILES}
)

target_sources(${PROJECT_NAME} PRIVATE
    ${VIS_PROJECT_HEADER_FILES}
    ${VIS_PROJECT_SOURCE_FILES}
)


target_link_libraries(${PROJECT_NAME} ${DYNAMIC_LIBRARIES})

install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

########################################################################
# Add Test libraries
########################################################################
include(ExternalProject)
set(EXTERNAL_PROJECTS_DIR ${CMAKE_CURRENT_BINARY_DIR}/external_libs)
set(EXTERNAL_PROJECTS_INCLUDE_DIR ${EXTERNAL_PROJECTS_DIR}/include)
set(EXTERNAL_PROJECTS_LIB_DIR ${EXTERNAL_PROJECTS_DIR}/lib)
if(VIS_WITH_TESTS)
    ExternalProject_Add(googletest
      PREFIX ${EXTERNAL_PROJECTS_DIR}
      GIT_REPOSITORY "https://github.com/google/googletest.git"
      GIT_TAG ${gtest_version}
      CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_PREFIX=${EXTERNAL_PROJECTS_DIR}
    )

    set(GTEST_INCLUDE_DIRS ${EXTERNAL_PROJECTS_INCLUDE_DIR})
    set(GMOCK_INCLUDE_DIRS ${EXTERNAL_PROJECTS_INCLUDE_DIR})

    set(GTEST_LIBRARY_PATH ${EXTERNAL_PROJECTS_LIB_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a)
    set(GTEST_LIBRARY gtest)
    add_library(${GTEST_LIBRARY} UNKNOWN IMPORTED)
    set_target_properties(${GTEST_LIBRARY} PROPERTIES
      IMPORTED_LOCATION ${GTEST_LIBRARY_PATH} )
    add_dependencies(${GTEST_LIBRARY} googletest)

    set(GTEST_MAIN_LIBRARY_PATH ${EXTERNAL_PROJECTS_LIB_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a)
    set(GTEST_MAIN_LIBRARY gtest_main)
    add_library(${GTEST_MAIN_LIBRARY} UNKNOWN IMPORTED)
    set_target_properties(${GTEST_MAIN_LIBRARY} PROPERTIES
      IMPORTED_LOCATION ${GTEST_MAIN_LIBRARY_PATH} )
    add_dependencies(${GTEST_MAIN_LIBRARY} googletest)

    set(GMOCK_LIBRARY_PATH ${EXTERNAL_PROJECTS_LIB_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}gmock.a)
    set(GMOCK_LIBRARY gmock)
    add_library(${GMOCK_LIBRARY} UNKNOWN IMPORTED)
    set_target_properties(${GMOCK_LIBRARY} PROPERTIES
      IMPORTED_LOCATION ${GMOCK_LIBRARY_PATH} )
    add_dependencies(${GMOCK_LIBRARY} googletest)

    set(GMOCK_MAIN_LIBRARY_PATH ${EXTERNAL_PROJECTS_LIB_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main.a)
    set(GMOCK_MAIN_LIBRARY gmock_main)
    add_library(${GMOCK_MAIN_LIBRARY} UNKNOWN IMPORTED)
    set_target_properties(${GMOCK_MAIN_LIBRARY} PROPERTIES
      IMPORTED_LOCATION ${GMOCK_MAIN_LIBRARY_PATH} )
    add_dependencies(${GMOCK_MAIN_LIBRARY} ${GTEST_LIBRARY})

    add_executable(${PROJECT_TEST_NAME} "")

    set(VIS_PROJECT_TESTING_SOURCES ${VIS_PROJECT_SOURCE_FILES})
    list(REMOVE_ITEM VIS_PROJECT_TESTING_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vis.cpp)

    # Add all source files
    target_sources(${PROJECT_TEST_NAME} PRIVATE
        ${VIS_PROJECT_HEADER_FILES}
        ${VIS_PROJECT_TESTING_SOURCES}
        ${VIS_PROJECT_TEST_FILES}
    )

    target_include_directories(${PROJECT_TEST_NAME} INTERFACE
        ${PROJECT_SOURCE_DIR}/tests
    )

    target_link_libraries(${PROJECT_TEST_NAME} gtest gmock pthread)
    target_link_libraries(${PROJECT_TEST_NAME} ${DYNAMIC_LIBRARIES})

    add_custom_target(vis_test_run ALL
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_TEST_NAME} --gtest_output=xml:${PROJECT_SOURCE_DIR}/gtestresults.xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Running tests in ${CMAKE_CURRENT_BINARY_DIR}"
        SOURCES ${vis_PROJECT_TEST_FILES}
    )

    add_dependencies(vis_test_run ${PROJECT_TEST_NAME})
endif()

########################################################################
# Add Perf Test libraries
########################################################################
if(VIS_WITH_PERF_TESTS)
    ExternalProject_Add(googlebenchmark
      PREFIX ${EXTERNAL_PROJECTS_DIR}
      GIT_REPOSITORY "git@github.com:google/benchmark.git"
      GIT_TAG ${googlebenchmark_version}
      CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${EXTERNAL_PROJECTS_DIR} -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
    )

    set(BENCHMARK_LIBRARY_PATH ${EXTERNAL_PROJECTS_LIB_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark.a)
    set(BENCHMARK_LIBRARY benchmark)
    add_library(${BENCHMARK_LIBRARY} UNKNOWN IMPORTED)
    set_target_properties(${BENCHMARK_LIBRARY} PROPERTIES
      IMPORTED_LOCATION ${BENCHMARK_LIBRARY_PATH} )
    add_dependencies(${BENCHMARK_LIBRARY} googlebenchmark)

    add_executable(${PROJECT_PERF_TEST_NAME} "")

    set(VIS_PROJECT_PERF_TESTING_SOURCES ${VIS_PROJECT_SOURCE_FILES})
    list(REMOVE_ITEM VIS_PROJECT_PERF_TESTING_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vis.cpp)

    # Add all source files
    target_sources(${PROJECT_PERF_TEST_NAME} PRIVATE
        ${VIS_PROJECT_HEADER_FILES}
        ${VIS_PROJECT_PERF_TESTING_SOURCES}
        ${VIS_PROJECT_PERF_TEST_FILES}
    )

    target_include_directories(${PROJECT_PERF_TEST_NAME} INTERFACE
        ${PROJECT_SOURCE_DIR}/perf_tests
    )

    target_link_libraries(${PROJECT_PERF_TEST_NAME} benchmark pthread)
    target_link_libraries(${PROJECT_PERF_TEST_NAME} ${DYNAMIC_LIBRARIES})

    add_custom_target(vis_perf_test_run ALL
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_PERF_TEST_NAME}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Running tests in ${CMAKE_CURRENT_BINARY_DIR}"
        SOURCES ${VIS_PROJECT_PERF_TEST_FILES}
    )

    add_dependencies(vis_perf_test_run ${PROJECT_PERF_TEST_NAME})
endif()

########################################################################
# Add clang format
########################################################################
find_program(CLANGFORMAT_CMD clang-format)
function(ADD_CLANGFORMAT _TARGETNAME)
    if (CLANGFORMAT_CMD)
        if (NOT TARGET ${_TARGETNAME})
            message(FATAL_ERROR "add_clangformat should only be called on targets (got " ${_TARGETNAME} ")")
        endif()

        # figure out which sources this should be applied to
        get_target_property(_CLANG_SOURCES ${_TARGETNAME} SOURCES)
        get_target_property(_BUILDDIR ${_TARGETNAME} BINARY_DIR)


        set(_SOURCES "")
        foreach (_SOURCE ${_CLANG_SOURCES})
            if (NOT TARGET ${_SOURCE})
                get_filename_component(_SOURCE_FILE ${_SOURCE} NAME)
                get_source_file_property(_CLANG_LOC "${_SOURCE}" LOCATION)

                set(_FORMAT_FILE ${_TARGETNAME}_${_SOURCE_FILE}.FORMAT)

                add_custom_command(OUTPUT ${_FORMAT_FILE}
                        DEPENDS ${_SOURCE}
                        COMMENT "Clang-Format ${_SOURCE}"
                        COMMAND ${CLANGFORMAT_CMD} -style=file -fallback-style=WebKit -sort-includes -i ${_CLANG_LOC}
                        COMMAND ${CMAKE_COMMAND} -E touch ${_FORMAT_FILE})

                list(APPEND _SOURCES ${_FORMAT_FILE})
            endif ()
        endforeach ()

        if (_SOURCES)
            add_custom_target(RUN_CLANGFORMAT
                    SOURCES ${_SOURCES}
                    COMMENT "Clang-Format for target ${_TARGET}")

                add_dependencies(${_TARGETNAME} RUN_CLANGFORMAT)
        endif ()
    endif ()
endfunction()

if(VIS_RUN_CLANG_FORMAT)
    add_clangformat(${PROJECT_NAME})
endif()

if(${VIS_RUN_CLANG_TIDY})
    find_program(
      CLANG_TIDY
      NAMES "clang-tidy"
      DOC "Path to clang-tidy executable"
      )
    message(STATUS "clang-tidy found: ${CLANG_TIDY}")
    set(DO_CLANG_TIDY "${CLANG_TIDY}" "-p=${CMAKE_CURRENT_SOURCE_DIR}")

    set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        COMPILE_FLAGS ""
    )

    if(CLANG_TIDY)
      set_target_properties(
          ${PROJECT_NAME} PROPERTIES
        CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
      )
    endif()
endif()


