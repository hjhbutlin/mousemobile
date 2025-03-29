CC = gcc


IFLAGS = -I/usr/local/include -I/usr/include
CFLAGS = -Wall -Wextra -pedantic -std=c11
LDFLAGS = -L/usr/local/lib -lm -framework OpenGL -framework GLUT


PATH = .
SRC = main.c
OBJ = $(SRC:.c=.o)
TARGET = run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(PATH)/%.o: $(PATH)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

