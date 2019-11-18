
CC = gcc
CFLAGS = -O3 -g -Werror -Wall -std=c11 #-DDEBUG
CCCOM = $(CC) $(CFLAGS)
SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)
LDLIBS = -lmingw32
LDFLAGS = -L/mingw/lib 
EXE = huffman
RM = del -f

all: $(EXE)

$(EXE): $(OBJS)
	$(CCCOM) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean 

clean: 
	$(RM) *.o *.exe *~ *.stckdump *.bak