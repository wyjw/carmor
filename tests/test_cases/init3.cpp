// RUN: ast -f test "%s" 2>&1 | FileCheck %s

// CHECK: init main

int main() {
    int x = 5; // @shield:process = childLevel; 3
}
