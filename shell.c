#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>

#define MAX_ARGS 10
//export PATH="$PATH:$(pwd)"

// pid_t pid;
pid_t current_pid = 0;

char** get_args(char* cl) {
    char* arg;
    char** args = (char**)malloc(MAX_ARGS * sizeof(char*));

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


char* get_file(const char* filename) {
    char* path = strdup(getenv("PATH"));
    char* path_entry;
    char* full_path;

    // printf("path: %s \n", path);
    // Iterate over each directory in the PATH variable
    path_entry = strtok(path, ":");
    while (path_entry != NULL) {
      // printf("path_entry: %s \n", path_entry);
        // Create a full path to the file in this directory
        full_path = malloc(strlen(path_entry) + strlen(filename) + 2);
        sprintf(full_path, "%s/%s", path_entry, filename);

        // Check if the file exists in this directory
        if (access(full_path, X_OK) == 0) {
          free(path);
          return full_path;
        }

        free(full_path);

        // Move to the next directory in the PATH variable
        path_entry = strtok(NULL, ":");
    }
    // If we get here, the file was not found in any directory in the PATH variable
    printf("File '%s' was not found in the PATH variable directories.\n", filename);
    return NULL;
}

// Handle Ctrl+C
void cancellation_handler(int dummy){
  if (current_pid != 0) {
    kill(current_pid, SIGTERM); // Send SIGTSTP signal to child process
    printf("\nProcess %d canceled.\n", current_pid);
    current_pid = 0;
  }
}


// Handle Ctrl+Z
void suspension_handler(int dummy) {
  if (current_pid != 0) {
    kill(current_pid, SIGTSTP); // Send SIGTSTP signal to child process
    printf("\nProcess %d suspended.\n", current_pid);
    current_pid = 0;
  }
}

  
pid_t execute_file(const char *file_path) {
  pid_t pid = fork(); // Create a child process

  if (pid == -1) {
      perror("fork"); // Error occurred
      exit(EXIT_FAILURE);
  } else if (pid == 0) {
      // Child process
      if (execl(file_path, file_path, (char *) NULL) == -1) {
          perror("execl"); // Error occurred
          exit(EXIT_FAILURE);
      }
  } else {
    // Parent process
    int status;
    current_pid = pid;
    if (waitpid(pid, &status, WUNTRACED) == -1) {
        perror("waitpid"); // Error occurred
        exit(EXIT_FAILURE);
    }

    return pid;
  }
  return pid;
}


int main(int argc, char const *argv[])
{
  char command_line[100];
  char error[100];
  char** args;
  char* file_path;
  bool isExist = false;

  signal(SIGINT, cancellation_handler); // Handle Ctrl+Z
  signal(SIGTSTP, suspension_handler); // Handle Ctrl+C

  printf ("Entered to finley's shell!\n> ");

  while(!isExist){
    fgets(command_line, 100, stdin);
    args = get_args(command_line);
    // print the args 
    // int i = 0;
    // while (args[i] != NULL) {
    //     printf("Arg %d: %s\n", i, args[i]);
    //     i++;
    // }

    if(strcasecmp(args[0],"exist") == 0){
      isExist = true;
    }
    else if(args[0] != NULL ){
      file_path = get_file(args[0]);
      if (file_path != NULL)
      {
        execute_file(file_path);
      }
      
      // printf("%s\n",file_path);
    }

    free(args);
    printf ("\n> ");
  }
  return 0;
}
