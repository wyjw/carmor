// RUN: ast -i -f test "%s" 2>&1 | FileCheck %s

// CHECK: CXXRecordDecl class
// CHECK-NEXT: DeclarationName test1
int main() {
    float x; // @shield:level=high
    float y; // @shield:level=high
    printf(“%f”, x+y);
    return 0;
}
