// RUN: ast -f test "%s" 2>&1 | FileCheck %s

// CHECK: addFunc struct

struct coordinate_high {
    float x; // @shield:level=high
    float y; // @shield:level=high
    time_t t; // @shield:level=low
}
