# derived from https://github.com/abenkhadra/llvm-pass-tutorial

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET (CMAKE_C_COMPILER "/usr/local/bin/clang")
SET (CMAKE_C_FLAGS "-Wall")
SET (CMAKE_CXX_COMPILER "/usr/local/bin/clang++")
SET (CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libstdc++")


add_library(SkeletonPass MODULE
    # List your source files here.
    Ncarmorpass.cpp
    Ncarmorpass.h
)

# Use C++11 to compile your pass (i.e., supply -std=c++11).
target_compile_features(SkeletonPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(SkeletonPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(SkeletonPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)
