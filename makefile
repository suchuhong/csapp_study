CC = /usr/bin/gcc-11
CFLAGS = -Wall -g -O2 -Werror -std=gnu99 -Wno-unused-function

EXECUTABLE = csapp

SRC_DIR = ./src

CODE = ./src/memory/instruction.c ./src/memory/dram.c ./src/cpu/mmu.c ./src/cpu/register.c ./src/disk/code.c ./src/main.c

.PHONY: csapp
csapp:
	${CC} ${CFLAGS} -I${SRC_DIR} ${CODE} -o ${EXECUTABLE}
	./${EXECUTABLE}

clean:
	rm -f *.o *~ ${EXECUTABLE}