
SET( VIS_PROJECT_HEADER_FILES
    ${CMAKE_SOURCE_DIR}/src/Domain/ColorDefinition.h
    ${CMAKE_SOURCE_DIR}/src/Domain/Settings.h
    ${CMAKE_SOURCE_DIR}/src/Domain/VisConstants.h
    ${CMAKE_SOURCE_DIR}/src/Domain/VisException.h
    ${CMAKE_SOURCE_DIR}/src/Domain/VisTypes.h
    ${CMAKE_SOURCE_DIR}/src/Source/AudioSource.h
    ${CMAKE_SOURCE_DIR}/src/Source/MpdAudioSource.h
    ${CMAKE_SOURCE_DIR}/src/Source/PortAudioSource.h
    ${CMAKE_SOURCE_DIR}/src/Source/PulseAudioSource.h
    ${CMAKE_SOURCE_DIR}/src/Source/ShmemAudioSource.h
    ${CMAKE_SOURCE_DIR}/src/Transformer/EllipseTransformer.h
    ${CMAKE_SOURCE_DIR}/src/Transformer/GenericTransformer.h
    ${CMAKE_SOURCE_DIR}/src/Transformer/LorenzTransformer.h
    ${CMAKE_SOURCE_DIR}/src/Transformer/SpectrumCircleTransformer.h
    ${CMAKE_SOURCE_DIR}/src/Transformer/SpectrumTransformer.h
    ${CMAKE_SOURCE_DIR}/src/Utils/ConfigurationUtils.h
    ${CMAKE_SOURCE_DIR}/src/Utils/Logger.h
    ${CMAKE_SOURCE_DIR}/src/Utils/NcursesUtils.h
    ${CMAKE_SOURCE_DIR}/src/Utils/Utils.h
    ${CMAKE_SOURCE_DIR}/src/Visualizer.h
    ${CMAKE_SOURCE_DIR}/src/Writer/NcursesWriter.h
)

SET( VIS_PROJECT_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/Domain/ColorDefinition.cpp
    ${CMAKE_SOURCE_DIR}/src/Domain/Settings.cpp
    ${CMAKE_SOURCE_DIR}/src/Domain/VisException.cpp
    ${CMAKE_SOURCE_DIR}/src/Source/AudioSource.cpp
    ${CMAKE_SOURCE_DIR}/src/Source/MpdAudioSource.cpp
    ${CMAKE_SOURCE_DIR}/src/Source/PortAudioSource.cpp
    ${CMAKE_SOURCE_DIR}/src/Source/PulseAudioSource.cpp
    ${CMAKE_SOURCE_DIR}/src/Source/ShmemAudioSource.cpp
    ${CMAKE_SOURCE_DIR}/src/Transformer/EllipseTransformer.cpp
    ${CMAKE_SOURCE_DIR}/src/Transformer/GenericTransformer.cpp
    ${CMAKE_SOURCE_DIR}/src/Transformer/LorenzTransformer.cpp
    ${CMAKE_SOURCE_DIR}/src/Transformer/SpectrumCircleTransformer.cpp
    ${CMAKE_SOURCE_DIR}/src/Transformer/SpectrumTransformer.cpp
    ${CMAKE_SOURCE_DIR}/src/Utils/ConfigurationUtils.cpp
    ${CMAKE_SOURCE_DIR}/src/Utils/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/Utils/NcursesUtils.cpp
    ${CMAKE_SOURCE_DIR}/src/Utils/Utils.cpp
    ${CMAKE_SOURCE_DIR}/src/vis.cpp
    ${CMAKE_SOURCE_DIR}/src/Visualizer.cpp
    ${CMAKE_SOURCE_DIR}/src/Writer/NcursesWriter.cpp
)

SET( VIS_PROJECT_TEST_FILES
    ${CMAKE_SOURCE_DIR}/tests/ConfigurationUtilsTest.cpp
    ${CMAKE_SOURCE_DIR}/tests/run_tests.cpp
    ${CMAKE_SOURCE_DIR}/tests/UtilsTest.cpp
)

SET( VIS_PROJECT_PERF_TEST_FILES
    ${CMAKE_SOURCE_DIR}/perf_tests/run_perf_tests.cpp
)
