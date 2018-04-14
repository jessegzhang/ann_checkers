CXX=g++
CFLAGS=-O2 -std=c++11 -fopenmp -Wall -Wno-unused-result -Wno-deprecated-register
LIB=

CHECKER_SRC=tournament.cpp neural.cpp

ifeq ($(OS),Windows_NT)
	LIB+=-lWs2_32
endif

all: train_game

train_game: main.cpp $(CHECKER_SRC)
	$(CXX) $(CFLAGS) $(LIB) $^ -o $@

clean:
	-rm -rf train_game train_game.exe
