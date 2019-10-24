// RUN: ast -f test "%s" 2>&1 | FileCheck %s

// CHECK: init main

int main() {
    int y = 87; // @shield:remote=restrict;low
}
