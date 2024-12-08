CC = /usr/bin/gcc-11
CFLAGS = -Wall -g -O2 -Werror -std=gnu99

EXE = program

SRC = ./src

CODE = ./src/memory/instruction.c ./src/disk/code.c ./src/main.c

.PHONY: program
main:
	${CC} ${CFLAGS} -I${SRC} ${CODE} -o ${EXE}

run:
	./${EXE}