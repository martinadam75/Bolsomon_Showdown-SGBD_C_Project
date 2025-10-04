CC=gcc
CFLAGS=-Wall -g
TARGET=ORI_T01
OBJS=ORI_T01.o ORI_T01_HEADER.o utils.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

ORI_T01.o: ORI_T01.c ORI_T01_HEADER.h utils.h
	$(CC) $(CFLAGS) -c ORI_T01.c

ORI_T01_HEADER.o: ORI_T01_HEADER.c ORI_T01_HEADER.h utils.h
	$(CC) $(CFLAGS) -c ORI_T01_HEADER.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(TARGET) $(OBJS)