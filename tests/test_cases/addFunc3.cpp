// RUN: ast -f test "%s" 2>&1 | FileCheck %s

// CHECK: addFunc test

// @shield:level = high
float addFunc(float x, float y) {
    return x + y;
}
