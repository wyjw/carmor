; RUN: %clang -S -O1 -emit-llvm %S/../inputs/input_for_mba.c -o - \
; RUN:  | opt -load ../lib/libMBAAdd%shlibext -legacy-mba-add -S \
; RUN:  | FileCheck %s
; RUN: %clang -S -O1 -emit-llvm %S/../inputs/input_for_mba.c -o - \
; RUN:  | opt -load-pass-plugin=../lib/libMBAAdd%shlibext -passes="mba-add" -S \
; RUN:  | FileCheck %s

; The input file contains 3 additions. Verify that these are correctly
; replaced as follows:
;    a + b == (((a ^ b) + 2 * (a & b)) * 39 + 23) * 151 + 111

; CHECK-LABEL: @foo
; 1st substitution
; CHECK-DAG:   {{%[0-9]+}} = xor i8 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK-DAG:   {{%[0-9]+}} = and i8 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK-DAG:   {{%[0-9]+}} = mul i8 2, {{%[0-9]+}}
; CHECK-NOT:   [[REG_2:%[0-9]+]] = mul i8 39, [[REG_1]]
; CHECK-DAG:   [[REG_1:%[0-9]+]] = add i8 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK-NEXT:  [[REG_2:%[0-9]+]] = mul i8 39, [[REG_1]]
; CHECK-NEXT:  [[REG_3:%[0-9]+]] = add i8 23, [[REG_2]]
; CHECK-NEXT:  [[REG_4:%[0-9]+]] = mul i8 -105, [[REG_3]]
; CHECK-NEXT:  [[REG_5:%[0-9]+]] = add i8 111, [[REG_4]]
;
; 2nd addition
; CHECK-DAG:   {{%[0-9]+}} = xor i8 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK-DAG:   {{%[0-9]+}} = and i8 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK-DAG:   {{%[0-9]+}} = mul i8 2, {{%[0-9]+}}
; CHECK-NOT:   [[REG_2:%[0-9]+]] = mul i8 39, [[REG_1]]
; CHECK-DAG:   [[REG_1:%[0-9]+]] = add i8 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK-NEXT:  [[REG_2:%[0-9]+]] = mul i8 39, [[REG_1]]
; CHECK-NEXT:  [[REG_3:%[0-9]+]] = add i8 23, [[REG_2]]
; CHECK-NEXT:  [[REG_4:%[0-9]+]] = mul i8 -105, [[REG_3]]
; CHECK-NEXT:  [[REG_5:%[0-9]+]] = add i8 111, [[REG_4]]
;
; 3rd addition
; CHECK-DAG:   {{%[0-9]+}} = xor i8 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK-DAG:   {{%[0-9]+}} = and i8 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK-DAG:   {{%[0-9]+}} = mul i8 2, {{%[0-9]+}}
; CHECK-NOT:   [[REG_2:%[0-9]+]] = mul i8 29, [[REG_1]]
; CHECK-DAG:   [[REG_1:%[0-9]+]] = add i8 {{%[0-9]+}}, {{%[0-9]+}}
; CHECK-NEXT:  [[REG_2:%[0-9]+]] = mul i8 39, [[REG_1]]
; CHECK-NEXT:  [[REG_3:%[0-9]+]] = add i8 23, [[REG_2]]
; CHECK-NEXT:  [[REG_4:%[0-9]+]] = mul i8 -105, [[REG_3]]
; CHECK-NEXT:  [[REG_5:%[0-9]+]] = add i8 111, [[REG_4]]
;
; Verify that there are no more additions (obfuscated or non-obfuscated)
; CHECK-NOT:   xor
; CHECK-NOT:   and
; CHECK-NOT:   mul
; CHECK-NOT:   add

; CHECK-LABEL: @main
