CC=gcc
CFLAGS = -Wall
LIBS =

APP = download.c connection.c parse.c

APP_OBJ = $(APP:.c=.o)

APP_EXEC = download

all: $(APP_EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(APP_EXEC): $(APP_OBJ)
	$(CC) $(CFLAGS) -o $@ $(APP_OBJ) $(LIBS)

clean:
	rm -rf $(APP_EXEC) *.o *.d
