CC=clang++
LD_FLAG=-lpthread
STD=-std=c++11

all:
	$(CC) $(STD) -o alarm.o -c alarm.cpp -g
	$(CC) $(STD) -o main.o -c main.cpp -g
	$(CC) $(STD) -o alarm alarm.o main.o $(LD_FLAG) -g 

clean:
	rm *.o alarm
