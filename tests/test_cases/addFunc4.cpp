// RUN: ast -f test "%s" 2>&1 | FileCheck %s

// CHECK: addFunc test

// @shield:level = high | downgrade = precision;3
int addFunc(int x, int y) {
    return x + y;
}
