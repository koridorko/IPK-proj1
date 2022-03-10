# Makefile for project
CC=g++
FLAGS=-Wall -Wextra

all: proj

proj:
	$(CC) server.cpp $(FLAGS) -o hinfosvc

clean:
	rm hinfosvc
