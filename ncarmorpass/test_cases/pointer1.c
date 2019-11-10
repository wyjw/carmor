#include <stdio.h>

void swap(char **p, char **q){
  char* t = *p;
       *p = *q;
       *q = t;
}

void swap2(char **p, char **q){
  char* t = *p;
  	*p = *q;
	*q = t;
}

int main(){
      char a1, b1;
      char *a = &a1;
      char *b = &b1;
      swap(&a,&b);
      printf("%d", a);
      printf("%d", b);
}
