cmake_minimum_required (VERSION 2.6)
project (animats2)

message("Preparing unit tests")

include_directories(
	${GLFW_INCLUDE}
	${GLM_INCLUDE}
	${INCLUDE_INCLUDE}
	${ARMADILLO_INCLUDE_DIR}
	"${PROJECT_SOURCE_DIR}/include"
)

add_executable(penetration_test penetration_test.cpp ../src/point.cpp ../src/face.cpp ../src/edge.cpp ../src/util.cpp)

add_executable(debug_test debug_test.cpp ../src/util.cpp)

target_link_libraries(penetration_test
	${ALL_LIBS}
)

target_link_libraries(penetration_test openblas)
target_link_libraries(penetration_test lapack)

include(CTest)

enable_testing()
add_test(PenetrationTest penetration_test)
add_test(DebugTest debug_test)