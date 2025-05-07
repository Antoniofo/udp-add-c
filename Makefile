CC = gcc
CROSS_CC = x86_64-w64-mingw32-gcc
SRC = src
OUT = out

all: client server client.exe server.exe

client: $(SRC)/client.c
	$(CC) $(SRC)/client.c -o $(OUT)/client

server: $(SRC)/server.c
	$(CC) $(SRC)/server.c -o $(OUT)/server

client.exe: $(SRC)/client_win.c
	$(CROSS_CC) $(SRC)/client_win.c -o $(OUT)/client.exe -lws2_32

server.exe: $(SRC)/server_win.c
	$(CROSS_CC) $(SRC)/server_win.c -o $(OUT)/server.exe -lws2_32
clean:
	rm -f out/*
