# simple-shell
Implement a simple shell program with a few features.

Sample shell program display a prompt, accept user input in the form of a command line and execute the program requested by the user. The first token in the command line is the name of the program to be executed: if it is found in the PATH environment variable then it will be executed, otherwise a warning will be printed. 

When a program finished, it will print out why it stopped running
- "exited with code 42" when the process completed its execution and terminated normally(non-zero exit status).
- "Process terminated by signal [signal number]"
- "Process did not exit normally"

## Start and Exit:

To enable the program run the program within current folder:

`export PATH="$PATH:$(pwd)"`

example to run pip:

`hello | reader`

The shell can only take 10 arguement maximum by defult.

To exit the shell, enter `exit`.





## Features

Shell including folloding features:
### Cancellation
Handle Ctrl+C input, using it to terminate a currently-executing child process rather than the shell program itself.

### History
Implement a history built-in command that will display all of the commands that the user has typed in this session.

### Pipes
Allow the output of one program to be attached to the input of another program using the pipe (|) character.

### Suspension

Handle Ctrl+Z input, using it to suspend a currently-executing child process rather than the shell program itself.