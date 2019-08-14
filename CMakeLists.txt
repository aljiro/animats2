cmake_minimum_required (VERSION 2.6)
project (ogladventures)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(glew REQUIRED)

include_directories(
	${GLFW_INCLUDE}
	${GLM_INCLUDE}
	${INCLUDE_INCLUDE}
	"${PROJECT_SOURCE_DIR}/common"
)

add_subdirectory (common)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	${GLM_LIBRARY}
	GLEW
)

add_executable(particles particles/particles.cpp)

target_link_libraries(particles
	${ALL_LIBS}
	Shaders
)

add_executable(triangles triangles/triangles.cpp triangles/SimpleVertexShader.vsh triangles/SimpleFragmentShader.fsh)

target_link_libraries(triangles
	${ALL_LIBS}
	Shaders
)

add_custom_command( TARGET particles POST_BUILD 
					COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/particles${CMAKE_EXECUTABLE_SUFFIX}"
					"${CMAKE_CURRENT_SOURCE_DIR}/particles")

add_custom_command( TARGET triangles POST_BUILD 
					COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/triangles${CMAKE_EXECUTABLE_SUFFIX}"
					"${CMAKE_CURRENT_SOURCE_DIR}/triangles")






