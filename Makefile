BIN := main
SOURCES := main.c 
HEADERS := main.h 
CFLAGS := -g
LDFLAGS :=

CC := gcc

.PHONY: clean

all: clean $(BIN) 

$LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH

$(BIN): $(SOURCES)
		$(CC) $(SOURCES) -o $(BIN) $(CFLAGS) $(LDFLAGS)
$(BIN): $(HEADERS)

clean:
		rm -rf $(BIN)