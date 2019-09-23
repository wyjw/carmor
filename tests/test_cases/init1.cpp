// RUN: ast -f test "%s" 2>&1 | FileCheck %s

// CHECK: init main

int main() {
    float x = 4.1; // @shield:level=low
    float y; // @shield:level=high
    y = x;
}
