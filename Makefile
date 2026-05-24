# The compiler to be used
CC      := gcc

# The options to use with the compiler
CFLAGS  := -Wall -Wextra -g -Iinclude -std=c11 -D_GNU_SOURCE

# this syntax makes the variable src hold all the files inside src folder ending with .c
SRC     = $(wildcard src/*.c)

# this two subsitution syntax makes the obj variable hold the eqivalent object file path for each src file
OBJ1    = $(SRC:.c=.o)
OBJ     = $(patsubst src/%.o, build/%.o, $(OBJ1))

#this s tjhe name of the executable
TARGET  = build/bin/myShell.out

#index target, for executable, we depend on all the object files that are made by doing -o obj.o
$(TARGET): $(OBJ)
	@mkdir -p build/bin
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) 

#target for all the build/.o files, they depend on the respective src/.c files, and for each file $< resolves to that specific named flag
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

#make function to clear out the build directory
clean:
	rm -rf build

#make function to run the compiled binary
run:
	./${TARGET}