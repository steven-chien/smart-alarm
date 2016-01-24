CC=clang++
LD_PATH=-I/usr/local/bin/
LD_FLAG=-lpthread -lsfml-audio -lonion
STD=-std=c++11

all:
	$(CC) $(STD) -o Alarm.o -c Alarm.cpp -g
	$(CC) $(STD) -o main.o -c main.cpp -g
	$(CC) $(STD) -o alarm Alarm.o main.o $(LD_PATH) $(LD_FLAG) -g 
clean:
	rm *.o alarm
