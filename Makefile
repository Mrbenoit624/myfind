CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Wfloat-equal -Wundef -Wshadow
CFLAGS+=-Wpointer-arith -Wbad-function-cast -Wcast-qual -Wcast-align
CFLAGS+=-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=-Wmissing-declarations -Wnested-externs -Wunreachable-code
CFLAGS+=-Wwrite-strings -g
VPATH=src
INCLUDE=include

FIND=myfind


FIND_SRC=$(wildcard src/*.c)
SRC=$(filter-out $(FIND).c, $(FIND_SRC))
OBJS=$(FIND_SRC:.c=.o)

SRC_STRING=$(wildcard src/libstring/*.c)
OBJS_STRING=$(SRC_STRING:.c=.o)

SRC_TREE=$(wildcard src/tree/*.c)
OBJS_TREE=$(SRC_TREE:.c=.o)


.PHONY:all clean check
all: $(FIND)

check:$(FIND)
	./test.sh

$(FIND):CFLAGS+=-iquote $(INCLUDE)
$(FIND):$(OBJS) $(OBJS_STRING) $(OBJS_TREE)


clean:
	$(RM) src/*.o $(FIND)
	$(RM) src/libstring/*.o
	$(RM) src/tree/*.o

