TARGET = game_sock
SOURCE = game.c utils.c master.c slave.c main.c socket_utils.c
HEADERS = game.h utils.h master.h slave.h socket_utils.h

$(TARGET): $(HEADERS) $(SOURCE)
	gcc-6 -std=c99 $(SOURCE) -o $(TARGET) -Wall -D_BSD_SOURCE

clean:
	rm -rf *.o *.out $(TARGET)
