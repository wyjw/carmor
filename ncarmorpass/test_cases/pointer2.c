#include <stdio.h>
#include <stdlib.h>

int main () {
   char *str;

   /* Initial memory allocation */
   str = (char *) malloc(11);
   strcpy(str, "hello world");
   printf("String = %s,  Address = %u\n", str, str);

   /* Reallocating memory */
   str = (char *) realloc(str, 11);
   strcat(str, "hello world");
   printf("String = %s,  Address = %u\n", str, str);

   free(str);
   
   return(0);
}
