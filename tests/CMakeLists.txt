cmake_minimum_required(VERSION 2.3)
set(LIT_ARGS "--path=${CMAKE_CURRENT_BINARY_DIR}/..")
set(LIT "llvm-lit")
add_custom_target(test
  COMMAND ${LIT}
          ${LIT_ARGS}
	  ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Running regression tests"
  DEPENDS ast
  )
