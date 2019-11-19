__attribute__((annotate("carmorsplit_1"))) int sub_one (int x) {
  return x-1;
}

__attribute__((annotate("carmorsplit_0"))) int add_one (int x) {
  return x+1;
}

int main () {
  int a = 4; 
  int b = 5; 
  int x = add_one(a); 
  int y = sub_one(b); 
}
