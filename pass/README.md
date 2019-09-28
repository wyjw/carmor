# Pass module

## Requires:
1) LLVM, built from source.
2) `$LLVM_HOME` set to the LLVM directory.

## How to build:
```
mkdir build
cd build
cmake ..
make
```

## How to run the pass:
```
clang -Xclang -load -Xclang build/pass/libSkeletonPass.* ExamplePass.c
```
