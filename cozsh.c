#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
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
    tokens[position] = token;
    position++;

    // Reallocate
    if (position > bufsize) {
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

void shell_loop() {
  char *lines;
  char **args;
  int status;

  do {
    printf("> ");
    lines = read_line();
    args = parse_line(lines);
    printf("%s\n", *args);

    free(lines);
    free(args);
  } while (1);
}

int main() {
  shell_loop();

  return EXIT_SUCCESS;
}
