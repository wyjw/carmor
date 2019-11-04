/usr/lib/llvm-9/bin/clang -emit-llvm test_cases/pointer2.c -S -o out.ll
/usr/lib/llvm-9/bin/opt --load build/lib/libCarmorPass.so --legacy-mba-add out.ll -S -o out_.ll
/usr/lib/llvm-9/bin/opt --load build/lib/libCarmorPass.so --legacy-mba-add out.ll -o out.bc

