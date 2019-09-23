// RUN: ast -f test "%s" 2>&1 | FileCheck %s

// CHECK: init main

int main() {
    int x = 4; // @shield:level = high |        checkRange; 3,4 |
}
