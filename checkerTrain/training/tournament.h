//
// Created by jesse on 3/16/2018.
//
#include <vector>
#include <fstream>
#include "neural.h"
#include <string>

#ifndef CHECKERS_TOURNAMENT_H
#define CHECKERS_TOURNAMENT_H

class Tournament {
public:
    Tournament();
    Tournament(int g, int p);
    void playTournament();
private:

    int playGame(int gameCounter, int bNum, int rNum);
    void chooseWinners();
    std::string convertToGen(int g);
    std::string convertToPop(int p);
    int gen;
    int popSize;
    std::vector<int> tournScore;
    std::vector<neuralNet> nnVec;

};
#endif //CHECKERS_TOURNAMENT_H
