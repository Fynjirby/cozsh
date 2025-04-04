/* cozsh - A minimal shell
 * Written by Leanghok Oeng
 *
 * This is a simple shell that has a couple of built-in commands (cd, exit,
 * help) and executes external commands via execvp.
 *
 * This shell was built for the purpose of learning about C and shell stuff
 */
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *read_line(void) {
  char *line = NULL;
  size_t bufsize = 0;

  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_FAILURE); // EOF
    } else {
      printf("cozsh: failied to read line\n");
      exit(EXIT_FAILURE);
    }
  }

  return line;
}

char **parse_line(char *line) {
  size_t bufsize = 64;
  size_t position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;

  if (!tokens) {
    printf("cozsh: allocation failed\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, " \t\r\n\a");
  while (token != NULL) {
    char *home_dir = getenv("HOME");

    if (token[0] == '~') {
      char *new_token = malloc(strlen(home_dir) + strlen(token) * sizeof(char));
      if (new_token == NULL) {
        printf("cozsh: allocation failed\n");
        exit(EXIT_FAILURE);
      }
      sprintf(new_token, "%s%s", home_dir, token + 1);
      tokens[position] = new_token;
    } else {
      tokens[position] = token;
    }

    position++;

    if (position >= bufsize) {
      bufsize *= 2;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens) {
        printf("cozsh: allocation failed\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, " \t\r\n\a");
  }

  tokens[position] = NULL;
  return tokens;
}

int handle_builtin(char **args) {
  if (strcmp(args[0], "cd") == 0) {
    // cd command
    if (args[1] == NULL) {
      printf("cozsh: please specify a directory\n");
    } else if (chdir(args[1]) != 0) {
      printf("cozsh: cd failed\n");
    }
    return 1;
  }
  if (strcmp(args[0], "exit") == 0 || strcmp(args[0], ":q") == 0) {
    // exit command
    printf("cozsh: exiting...\n");
    exit(EXIT_SUCCESS);
  }
  if (strcmp(args[0], "help") == 0) {
    // help command
    printf("cozsh - a minimal shell :3\n");
    printf("Builtin commands:\n");
    printf("  cd <dir>: Change directory\n");
    printf("  exit: exit cozsh\n");
    printf("  help: display this help message\n");
    printf("Note: You can also use other commands like ls, vim...\n");

    return 1;
  }

  return 0;
}

int execute(char **args) {
  // Empty command
  if (args[0] == NULL) {
    return 1; // do nothing
  }

  // Handle builtin command
  if (handle_builtin(args) == 1) {
    return 1;
  }

  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      printf("cozsh: execvp failed\n");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    printf("cozsh: fork failed\n");
    return 1;
  } else {
    // Parent process
    int status;
    waitpid(pid, &status, 0);
  }

  return 1;
}

// Main shell loop
void shell_loop(void) {
  char *lines;
  char **args;
  int status;
  char cwd[1024];
  char *home_dir = getenv("HOME");

  do {
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      if (strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
        printf("~%s > ", cwd + strlen(home_dir));
      } else {
        printf("%s > ", cwd);
      }
    } else {
      printf("> ");
    }

    lines = read_line();
    args = parse_line(lines);
    status = execute(args);

    free(lines);
    free(args);
  } while (status);
}

int main(void) {
  signal(SIGINT, SIG_IGN);

  shell_loop();

  return EXIT_SUCCESS;
}
