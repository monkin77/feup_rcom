CC=gcc
CFLAGS = -Wall
LIBS =

APP_PROTOCOL = appProtocol.c emissor.c receptor.c common.c

APP_PROTOCOL_OBJ = $(APP_PROTOCOL:.c=.o)

APP_PROTOCOL_EXEC = appProtocol

all: $(EMISSOR_EXEC) $(RECEPTOR_EXEC) $(APP_PROTOCOL_EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(APP_PROTOCOL_EXEC): $(APP_PROTOCOL_OBJ)
	$(CC) $(CFLAGS) -o $@ $(APP_PROTOCOL_OBJ) $(LIBS)

clean:
	rm -rf $(APP_PROTOCOL_EXEC) *.o *.d
