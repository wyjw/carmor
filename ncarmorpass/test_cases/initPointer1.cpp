// RUN: ast -f test "%s" 2>&1 | FileCheck %s

// CHECK: initPointer main

int main() {
    (int *)x = 5; // @shield:level=high
    (int *)y; // @shield:level=low
}
