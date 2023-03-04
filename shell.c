#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_ARGS 10

char** get_args(char* cl) {
    char* arg;
    char** args = (char**)malloc(MAX_ARGS * sizeof(char*));
    // cl = strsep(&cl, '\n');

    if (cl == NULL)
    {
      printf("invalid command or something");
      args[0] = NULL;
      return args;
    }
    
    int i;
    for (i = 0; i < MAX_ARGS; i++){
      arg = strsep(&cl, " ");
      if(arg != NULL){
        args[i] = arg;
      }else{
        break;
      }
    }    

    // Terminate the args array with a null pointer
    args[i] = NULL; 
    // Remove the newline character 
    args[i-1] = strsep(&args[i-1], "\n");; 
    return args;
}


int main(int argc, char const *argv[])
{

  char command_line[100];
  char error[100];
  char** args;
  printf ("Entered to finley's shell!\n> ");
  
  while(1){

    

    fgets(command_line, 100, stdin);
    // printf("You entered: %s", command_line);
    // command_line = strsep(command_line, '\n');
    // command_line[strcspn(command_line, "\n")] = 0; 
    args = get_args(command_line);
    int i = 0;
    while (args[i] != NULL) {
        printf("Arg %d: %s\n", i, args[i]);
        i++;
    }

    free(args);
  }
  return 0;
}
