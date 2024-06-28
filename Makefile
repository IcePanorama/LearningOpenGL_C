SRC_DIR=src
SRC_FILES=$(SRC_DIR)/*.c
#INCL_FILES=$(SRC_DIR)/*.h
LIBS=-lSDL2 -lGL -lGLEW
TARGET=main
CFLAGS=-std=c99 -Wpedantic -Wextra -Werror -Wall -Wstrict-aliasing=3 -Wwrite-strings -Wvla -Wcast-align=strict -Wstrict-prototypes -Wstringop-overflow=4 -Wshadow -fanalyzer
STYLE=GNU

all: clean format $(TARGET)

full: all test

$(TARGET): 
	gcc $(SRC_FILES) -o $(TARGET) $(LIBS) $(CFLAGS)

test: $(TARGET)
	valgrind --leak-check=yes ./$(TARGET)

clean:
	-@rm $(TARGET) 2>/dev/null || true

format: 
	clang-format -style=$(STYLE) -i $(SRC_FILES) $(INCL_FILES)
