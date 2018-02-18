# a simple Makefile for compiling this program

# optimization is on
CC=g++
STD=-std=c++11
# for debugging use this
#CC = c++ -g -o0


all: schedule

schedule: schedule.o RoundRobin.o SRTF.o FCFS.o Scheduler.o Process.o MLFQ.o
	$(CC) $(STD) -o schedule schedule.o RoundRobin.o SRTF.o FCFS.o Scheduler.o Process.o MLFQ.o

RoundRobin.o: Process.h Scheduler.h RoundRobin.cpp
	$(CC) $(STD) -c RoundRobin.cpp

SRTF.o: Process.h Scheduler.h SRTF.cpp
	$(CC) $(STD) -c SRTF.cpp

FCFS.o: Process.h Scheduler.h FCFS.cpp
	$(CC) $(STD) -c FCFS.cpp

Scheduler.o: Scheduler.h Scheduler.cpp
	$(CC) $(STD) -c Scheduler.cpp

Process.o: Process.h Process.cpp
	$(CC) $(STD) -c Process.cpp

schedule.o: FCFS.h RoundRobin.h SRTF.h Scheduler.h schedule.cpp
	$(CC) $(STD) -c schedule.cpp

MLFQ.o:	Scheduler.h MLFQ.h MLFQ.cpp
	$(CC) $(STD) -c MLFQ.cpp

clean:
	/bin/rm -f *.o schedule 
