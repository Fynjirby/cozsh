#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char *read_line() {
  char *line;
  size_t bufsize = 0;

  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_FAILURE); // EOF
    } else {
      printf("Error reading line\n");
      exit(EXIT_FAILURE);
    }
  }

  return line;
}

void shell_loop() {
  char *lines;
  char **args;
  int status;

  do {
    printf("> ");
    lines = read_line();
    args = parse_line(lines);
    status = exec_line(args);

    free(lines);
    free(args);
  } while (status);
}

int main() {
  shell_loop();

  return EXIT_SUCCESS;
}
