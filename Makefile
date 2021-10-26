CC=gcc
CFLAGS = #-Wall
LIBS =

READ = noncanonical.c
WRITE = writenoncanonical.c

READ_OBJ = $(READ:.c=.o)
WRITE_OBJ = $(WRITE:.c=.o)

READ_EXEC = read
WRITE_EXEC = write

all: $(READ_EXEC) $(WRITE_EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(READ_EXEC): $(READ_OBJ)
	$(CC) $(CFLAGS) -DDELAY=100 -o $@ $(READ_OBJ) $(LIBS)

$(WRITE_EXEC): $(WRITE_OBJ)
	$(CC) $(CFLAGS) -o $@ $(WRITE_OBJ) $(LIBS)

clean:
	rm -rf $(READ_EXEC) $(WRITE_EXEC) *.o *.d
