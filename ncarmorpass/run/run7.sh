/usr/lib/llvm-9/bin/clang -emit-llvm ../../outoftree/commenttool/split2.c -S -o out.ll
/usr/lib/llvm-9/bin/opt --load ../build/lib/libCarmorPass.so --legacy-mba-add out.ll -S -o out_.ll
/usr/lib/llvm-9/bin/clang -emit-llvm ../lib/mem.c -S -o out2.ll
/usr/lib/llvm-9/bin/clang -emit-llvm ../lib/shadow_metadata_table.c -S -o out4.ll
/usr/lib/llvm-9/bin/llvm-link -S out_.ll out2.ll out4.ll> out3.ll
/usr/lib/llvm-9/bin/llc -filetype=obj out3.ll
clang++ -no-pie out3.o -lpthread
