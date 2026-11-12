# Each example takes the path to its softmats.json as its only argument.
foreach(example huddling bags softcollision)
  add_subdirectory(${example})
endforeach()
