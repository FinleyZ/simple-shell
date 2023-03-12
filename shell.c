#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
    // Remove the newline character for last arg 
    args[i-1] = strsep(&args[i-1], "\n");; 
    return args;
}



// unfinished and incorrect
void get_file(){
  char* filename = "finley79212.txt";
  char* path = getenv("PATH");
  char* dir = strtok(path, ":");
  printf ("dir: %s\n", dir);

    while (dir != NULL) {
      char filepath[1024];
      snprintf(path, sizeof(path), "%s/%s", dir, filename);
      printf ("PATH: %s\n", path);
      if (access(path, F_OK) == 0) {
          printf("Found %s at %s\n", filename, dir);
          break;
      }
      dir = strtok(NULL, ":");
    }
}

void executeFile(const char* filename) {
    char command[100];
    sprintf(command, "./%s", filename);
    system(command);
}

int main(int argc, char const *argv[])
{

  char command_line[100];
  char error[100];
  char** args;
  printf ("Entered to finley's shell!\n> ");

  while(1){
    fgets(command_line, 100, stdin);
    args = get_args(command_line);
    
    // print the args 
    int i = 0;
    while (args[i] != NULL) {
        printf("Arg %d: %s\n", i, args[i]);
        i++;
    }

    if(args[0] != NULL){
      executeFile(args[0]);
    }

    free(args);
    printf ("\n> ");
  }
  return 0;
}
