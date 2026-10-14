CC      = gcc
CFLAGS  = -Wall -Wextra -g
TARGET  = keyword_index
SRC     = $(wildcard src/*.c)
HDR     = $(wildcard src/*.h)

.PHONY: build run test asan clean

build: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: build
	./$(TARGET) examples/sample.in

test: build
	./tests/run_tests.sh ./$(TARGET)


asan: CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
asan: clean build
	./tests/run_tests.sh ./$(TARGET)

clean:
	rm -f $(TARGET)
