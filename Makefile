# Usage:
# make		# compile all binaries
# make clean	# remove all binaries

.PHONY: all clean

CC=g++
SRCS:=$(wildcard *.cpp)
HEADERS:=$(wildcard *.h)
EXEC=repl
CFLAGS=-g -Wall

all: ${EXEC}

repl: ${SRCS} ${HEADERS}
	@echo "Compiling..."
	${CC} -g ${SRCS} -o $@
	@echo "Done"

clean:
	rm -rvf *.o ${EXEC}
