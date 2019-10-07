///@2
int sub_one (int x) {
  return x-1;
}

///@1
int add_one (int x) {
  return x+1;
}

int main () {
  int a = 4; ///@1
  int b = 5; ///@2
  int x = add_one(a); ///@1
  int y = sub_one(b); ///@2
}
