CC= g++
CFLAGS= -lcurl

banno:
	$(CC) bannoMain.cpp -o test $(CFLAGS)