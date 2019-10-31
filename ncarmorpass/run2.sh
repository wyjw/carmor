/usr/lib/llvm-9/bin/clang -emit-llvm test_cases/pointer1.c -S -o out.ll
/usr/lib/llvm-9/bin/opt --load build/lib/libCarmorPass.so --legacy-mba-add -disable-output out.ll
