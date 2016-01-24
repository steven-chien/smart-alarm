CC=clang++
LD_FLAG=-lpthread -lsfml-audio
STD=-std=c++11

all:
	$(CC) $(STD) -o Alarm.o -c Alarm.cpp -g
	$(CC) $(STD) -o main.o -c main.cpp -g
	$(CC) $(STD) -o Alarm Alarm.o main.o $(LD_FLAG) -g 

clean:
	rm *.o alarm
