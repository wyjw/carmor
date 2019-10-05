int if_zero (int x) {
  if (x == 0) {
    x -= 7;
  }
  else {
    x += 4;
  }
  return x;
}

int main () {
  int y = 4;
  int z = if_zero(y);
}
