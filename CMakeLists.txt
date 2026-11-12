cmake_minimum_required(VERSION 3.10)
project(softmats CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Following `cmake --help-policy CMP0072`
set(OpenGL_GL_PREFERENCE "GLVND")

option(USE_GLEW "Link GLEW (try if the linker can't find glCreateVertexArrays)" OFF)
option(BUILD_EXAMPLES "Build the example programs" ON)
option(BUILD_TESTS "Build the tests of the dynamics" ON)

# Required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(Armadillo REQUIRED)
find_package(OpenCV REQUIRED) # Only used by the video recorder
find_package(OpenMP)          # Armadillo uses OpenMP when available

file(GLOB SOURCES
  src/*.cpp
  src/visual/*.cpp
  src/core/*.cpp
  src/util/*.cpp
  src/collisions/*.cpp
)

add_library(softmats SHARED ${SOURCES})

# Examples include the headers relative to src, e.g. <softmatsim.h>, <core/animat.h>
target_include_directories(softmats PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${ARMADILLO_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

# glm is header only; newer versions export a target, older ones only a variable
if(TARGET glm::glm)
  target_link_libraries(softmats PUBLIC glm::glm)
else()
  target_include_directories(softmats PUBLIC ${GLM_INCLUDE_DIRS})
endif()

target_compile_definitions(softmats PUBLIC GL3_PROTOTYPES GL_GLEXT_PROTOTYPES)

if(APPLE)
  # The sources select the system OpenGL headers with __OSX__
  target_compile_definitions(softmats PUBLIC __OSX__ GL_SILENCE_DEPRECATION)
else()
  # Khronos <GL3/gl3.h>, which is not shipped by Linux distributions
  target_include_directories(softmats PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
endif()

target_link_libraries(softmats PUBLIC
  OpenGL::GL
  glfw
  ${ARMADILLO_LIBRARIES}
  ${OpenCV_LIBS}
)

if(OpenMP_CXX_FOUND)
  target_link_libraries(softmats PUBLIC OpenMP::OpenMP_CXX)
endif()

if(USE_GLEW)
  find_package(GLEW REQUIRED)
  target_compile_definitions(softmats PUBLIC USE_GLEW)
  target_link_libraries(softmats PUBLIC GLEW::GLEW)
endif()

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
