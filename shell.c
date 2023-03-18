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

void add_history(char* user_command){
    FILE * fPtr; /* File pointer to hold reference to our file */
    fPtr = fopen("./history.txt", "a");
    if(fPtr == NULL)
    {
      printf("Unable to history.txt.\n");/* File not created hence exit */
    }
    fprintf (fPtr, "%s", user_command);
    fprintf(fPtr, "\n");
    fclose(fPtr);
}

void history( ){
      FILE *fp = fopen("history.txt", "r");
      if(fp == NULL) {
        perror("Unable to open file!");
        exit(1);
      }
      char chunk[128];
      while(fgets(chunk, sizeof(chunk), fp) != NULL) {
        fputs(chunk, stdout); // marker string used to show where the content of the chunk array has ended
      }
      fclose(fp);
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
      perror("fork"); 
      exit(EXIT_FAILURE);
  } else if (pid == 0) {
      // Child process
      if (execl(file_path, file_path, (char *) NULL) == -1) {
          perror("execl"); 
          exit(EXIT_FAILURE);
      }
  } else {
    // Parent process
    int status;
    current_pid = pid;
    if (waitpid(pid, &status, WUNTRACED) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status)) {
      printf("\nexited with status 42\n");
    } else if (WIFSIGNALED(status)) {
      printf("\nProcess terminated by signal %d\n", WTERMSIG(status));
    } else {
      printf("\nProcess did not exit normally\n");
    }

    return pid;
  }
  return pid;
}


pid_t my_pipe(const char *file_path1, const char *file_path2) {
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

  if (WIFEXITED(status2)) {
    printf("\nexited with status 42\n");
  } else if (WIFSIGNALED(status2)) {
    printf("\nProcess terminated by signal %d\n", WTERMSIG(status2));
  } else { 
    printf("\nProcess did not exit normally\n");
  }

  return pid2;
}


int main(int argc, char const *argv[])
{
  char command_line[100];
  char error[100];
  char** args;
  char* file_path;
  bool isExit = false;
  remove("./history.txt");

  signal(SIGINT, cancellation_handler); // Handle Ctrl+Z
  signal(SIGTSTP, suspension_handler); // Handle Ctrl+C

  printf ("Entered to finley's shell!\n> ");

  while(!isExit){
    fgets(command_line, 100, stdin);
    args = get_args(strdup(command_line));
    // print the args 
    // int i = 0;
    // while (args[i] != NULL) {
    //     printf("Arg %d: %s\n", i, args[i]);
    //     i++;
    // }

    // printf("command: %s\n", command_line);
    if(strcasecmp(args[0],"exit") == 0){
      isExit = true;
    }
    
    else if(strcmp(args[0], "history")==0){
      add_history(args[0]);
      history();
    }


    else if(args[0] != NULL && args[2] != NULL && strcmp(args[1],"|") == 0){
      int i = 0;
      
      while(i <8){

        int input_index = i;
        int pip_index = i + 1;
        int output_index = i+2;

        if(args[input_index] != NULL && args[output_index] != NULL && strcmp(args[pip_index],"|") == 0){

          char* input_file_path = get_file(args[input_index]);
          char* output_file_path = get_file(args[output_index]);

          my_pipe(input_file_path, output_file_path);
        }else{
          printf("invalid command\n");
        }
        
        i = i+2;
        
        if(args[i+1] == NULL){
          break;
        }
      }

      add_history(command_line);
    }


    else if(args[0] != NULL){
      file_path = get_file(args[0]);
      if (file_path != NULL)
      {
        execute_file(file_path);
      }
      add_history(command_line);
      
      // printf("%s\n",file_path);
    }else{
      printf("invalid command");
    }

    free(args);
    printf ("\n> ");
  }
  return 0;
}
