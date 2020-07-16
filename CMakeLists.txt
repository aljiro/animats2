cmake_minimum_required (VERSION 2.6)
project (animats2)

# Required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)
find_package(Armadillo REQUIRED)

include_directories(
	${GLFW_INCLUDE}
	${GLM_INCLUDE}
	${INCLUDE_INCLUDE}
	${ARMADILLO_INCLUDE_DIR}
	
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	${GLM_LIBRARY}
	GLEW
	armadillo
)

add_subdirectory(test)

file(GLOB SOURCES
    src/main.cpp
	src/geometry/*.cpp
	src/geometry/*.h
	src/core/*.cpp
	src/core/*.h
	src/collision/*.cpp
	src/collision/*.h
	src/views/*.cpp
	src/views/*.h
	src/utilities/*.cpp
	src/utilities/*.h
	src/views/*.hpp
)

add_executable(animats ${SOURCES})
target_include_directories(animats PRIVATE
	"${PROJECT_SOURCE_DIR}/src/"
)

target_link_libraries(animats
	${ALL_LIBS}
)

target_link_libraries(animats openblas)
target_link_libraries(animats lapack)

message("Copying the shaders from")
message("${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/")
message("to")
message("${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/")

add_custom_command( TARGET animats POST_BUILD 
					COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders" 
					"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/shaders")





