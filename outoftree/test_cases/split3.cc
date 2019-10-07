///@2
int sub_one (int x) {
  return x-1;
}

///@1
int add_one (int x) {
  return x+1;
}

int main () {
  int a = 4; ///@2
  a = 6; ///@2
  a = 45; ///@2
  int b = 5; ///@1
  int x = add_one(a); ///@1
  int y = sub_one(b); ///@2
}
