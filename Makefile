CC      = g++ -std=c++11
CFLAGS  = 
LDFLAGS = 

all: main clean

main: main.o
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: main.cpp commun.h
	$(CC) -c $(CFLAGS) $<

.PHONY: clean cleanest

clean:
	rm *.o

cleanest: clean
	rm main