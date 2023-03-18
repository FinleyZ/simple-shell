#include<stdio.h>

int main() {
  char buffer[100];
  printf("Enter a string: \n");
  fgets(buffer, 100, stdin);
  printf("You entered: %s\n", buffer);
  return 0;
}
