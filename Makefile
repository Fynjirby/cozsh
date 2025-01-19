CC = gcc
CFLAGS = -Wall -Wextra -pedantic

TARGET = cozsh
SRC = cozsh.c

.PHONY: clean install

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/$(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
