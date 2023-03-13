#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_ARGS 10

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
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid"); // Error occurred
        exit(EXIT_FAILURE);
    }

    return pid;
  }
  return pid;
}

pid_t pipe(const char *file_path1, const char *file_path2) {
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

    pid_t pid1 = fork(); // Create first child process
  if (pid1 == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid1 == 0) {
    // Child process 1
    close(pipefd[0]); // Close unused read end of pipe
    if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    if (execl(file_path1, file_path1, (char *) NULL) == -1) {
      perror("execl");
      exit(EXIT_FAILURE);
    }
  }

  pid_t pid2 = fork(); // Create second child process
  if (pid2 == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid2 == 0) {
    // Child process 2
    close(pipefd[1]); // Close unused write end of pipe
    if (dup2(pipefd[0], STDIN_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    if (execl(file_path2, file_path2, (char *) NULL) == -1) {
      perror("execl");
      exit(EXIT_FAILURE);
    }
  }

  // Parent process
  close(pipefd[0]); // Close unused read end of pipe
  close(pipefd[1]); // Close unused write end of pipe

  int status1, status2;
  if (waitpid(pid1, &status1, 0) == -1) {
    perror("waitpid");
    exit(EXIT_FAILURE);
  }
  if (waitpid(pid2, &status2, 0) == -1) {
    perror("waitpid");
    exit(EXIT_FAILURE);
  }

  return pid2;
}


int main(int argc, char const *argv[])
{
  char command_line[100];
  char error[100];
  char** args;
  char* file_path;
  printf ("Entered to finley's shell!\n> ");

  while(1){
    fgets(command_line, 100, stdin);
    args = get_args(command_line);
    
    // print the args 
    // int i = 0;
    // while (args[i] != NULL) {
    //     printf("Arg %d: %s\n", i, args[i]);
    //     i++;
    // }

    if(args[0] != NULL){
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
