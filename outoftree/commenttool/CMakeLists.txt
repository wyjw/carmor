cmake_minimum_required(VERSION 2.8.8)

project(simple_tool)

option(TAINT_TYPE "Taint tool." ON)
mark_as_advanced(TAINT_TYPE)
if (TAINT_TYPE)

    # Find boost
    find_package(Boost COMPONENTS date_time REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})

    # Find LLVM Config binary for LLVM 3.5
    # If you're on debian, go to http://llvm.org/apt/ and install the development
    # release of LLVM (all packages).
    find_program(LLVM_CONFIG_PATH "llvm-config" PATHS "/usr/lib/llvm-9/bin" NO_DEFAULT_PATH)
    if(NOT LLVM_CONFIG_PATH)
        message(FATAL_ERROR "llvm-config not found -- ${LLVM_CONFIG_PATH}")
    endif()
    message(STATUS "Found LLVM_CONFIG_PATH as ${LLVM_CONFIG_PATH}")

    # This function saves the output of the llvm-config command with the given
    # switch to the variable named VARNAME.
    #
    # Example usage: llvm_config(LLVM_CXXFLAGS "--cxxflags")
    function(llvm_config VARNAME switch)
        set(CONFIG_COMMAND "${LLVM_CONFIG_PATH}" "${switch}")

        execute_process(
            COMMAND ${CONFIG_COMMAND}
            RESULT_VARIABLE HAD_ERROR
            OUTPUT_VARIABLE CONFIG_OUTPUT
        )

        if (HAD_ERROR)
            string(REPLACE ";" " " CONFIG_COMMAND_STR "${CONFIG_COMMAND}")
            message(STATUS "${CONFIG_COMMAND_STR}")
            message(FATAL_ERROR "llvm-config failed with status ${HAD_ERROR}")
        endif()

        # replace linebreaks with semicolon
        string(REGEX REPLACE
            "[ \t]*[\r\n]+[ \t]*" ";"
            CONFIG_OUTPUT ${CONFIG_OUTPUT})

        # make result available outside
        set(${VARNAME} ${CONFIG_OUTPUT} PARENT_SCOPE)

        # Optionally output the configured value
        message(STATUS "llvm_config(${VARNAME})=>${CONFIG_OUTPUT}")

        # cleanup
        unset(CONFIG_COMMAND)
    endfunction(llvm_config)

    llvm_config(LLVM_CXXFLAGS "--cxxflags")
    llvm_config(LLVM_LDFLAGS "--ldflags")
    llvm_config(LLVM_LIBS "--libs")
    llvm_config(LLVM_LIBDIR "--libdir")
    llvm_config(LLVM_INCLUDE_DIR "--includedir")
    llvm_config(LLVM_SYSTEM_LIBS "--system-libs")

    add_definitions(${LLVM_CXXFLAGS})
    add_definitions(-fno-rtti)
    include_directories(${LLVM_INCLUDE_DIR})
    link_directories(${LLVM_LIBDIR})

    add_executable( comment-tool
	    CommentTool.cpp
        )

    # Libraries might only be found when building with:
    # LD_LIBRARY_PATH=$(llvm-config-3.5 --libdir):$LD_LIBRARY_PATH make
    target_link_libraries(comment-tool
        ${LLVM_LDFLAGS}
	-lLLVM-9
	-lclangTooling
        -lclangFrontendTool
        -lclangFrontend
        -lclangDriver
        -lclangSerialization
        -lclangParse
        -lclangSema
	-lclangRewrite
	-lclangRewriteFrontend
        -lclangAnalysis
        -lclangEdit
        -lclangAST
        -lclangLex
        -lclangBasic
        -lclangASTMatchers
        ${LLVM_LIBS}
        ${Boost_LIBRARIES}
    )
endif()
