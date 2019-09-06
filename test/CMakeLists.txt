cmake_minimum_required (VERSION 2.6)
project (animats2)

include_directories(
	${GLFW_INCLUDE}
	${GLM_INCLUDE}
	${INCLUDE_INCLUDE}
	${ARMADILLO_INCLUDE_DIR}
	"${PROJECT_SOURCE_DIR}/include"
)

add_executable(animats_test penetration.cpp ../src/point.cpp ../src/face.cpp ../src/edge.cpp ../src/util.cpp)

target_link_libraries(animats_test
	${ALL_LIBS}
)
target_link_libraries(animats_test openblas)
target_link_libraries(animats_test lapack)

include(CTest)

add_test(PenetrationTest animats_test)