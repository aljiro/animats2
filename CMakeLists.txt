cmake_minimum_required (VERSION 2.6)
project (animats2)

# Required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(glew REQUIRED)
find_package(glew REQUIRED)
find_package(Armadillo REQUIRED)

include_directories(
	${GLFW_INCLUDE}
	${GLM_INCLUDE}
	${INCLUDE_INCLUDE}
	${ARMADILLO_INCLUDE_DIR}
	"${PROJECT_SOURCE_DIR}/include"
)

add_subdirectory (include)


set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	${GLM_LIBRARY}
	GLEW
	Armadillo
)

file(GLOB SOURCES
    src/*.cpp
)

add_executable(animats ${SOURCES})

target_link_libraries(animats
	${ALL_LIBS}
)

add_custom_command( TARGET animats POST_BUILD 
					COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/source/shaders" 
					"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/particles${CMAKE_EXECUTABLE_SUFFIX}")

add_custom_command( TARGET triangles POST_BUILD 
					COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/triangles${CMAKE_EXECUTABLE_SUFFIX}"
					"${CMAKE_CURRENT_SOURCE_DIR}/triangles")






