add_executable(dynamics dynamics.cpp)
target_link_libraries(dynamics softmats)

# Each scenario runs on the mesh used by the examples and on a finer sphere
foreach(scenario rest soft drop sunk push momentum pile)
  add_test(NAME ${scenario} COMMAND dynamics ${scenario} ${PROJECT_SOURCE_DIR}/res/models/sphere.obj ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

foreach(scenario rest soft drop sunk push momentum pile)
  add_test(NAME ${scenario}_fine COMMAND dynamics ${scenario} icosphere:3 ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# The flattening follows the stiffness
add_test(NAME soft_fine_softer COMMAND dynamics soft icosphere:3 ${CMAKE_CURRENT_BINARY_DIR} "\"stiffness\" : 30000")
add_test(NAME soft_fine_stiffer COMMAND dynamics soft icosphere:3 ${CMAKE_CURRENT_BINARY_DIR} "\"stiffness\" : 400000")
