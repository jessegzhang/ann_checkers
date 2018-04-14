//
// Created by jesse on 3/16/2018.
//

#include "tournament.h"
#include <sys/stat.h>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <omp.h>


Tournament::Tournament(int g, int p) {
    gen = g;
    popSize = p;
    //given neural Net toplogy
    std::vector<int> layNum = {91, 40, 10, 1};
    //the correct weight number that should be in each file
    int corWeightNum = 5047;
    for(int i=0; i<p;i++) {
        //push back p number neuralNets;
        nnVec.push_back(neuralNet());
    }
    std::string fileBase = "../neuralWeights/GEN" + convertToGen(g) + "/";
    mkdir(fileBase.c_str(),  0777);

    //generate gen0
    if(gen == 0) {
        for(int j=0; j<p; j++) {
            //generate initial weights
            nnVec[j].getNeuralNet(layNum);
            //save weights to a file
            nnVec[j].saveWeights(fileBase+convertToPop(j));
        }
    } else {
        //not gen0 so load in weights
        for(int k=0; k<p; k++) {
            nnVec[k].loadWeights(fileBase+convertToPop(k));
        }
    }
}

void Tournament::playTournament() {
    std::string fileBase = "../neuralWeights/TOUR" + convertToGen(gen) + "/";
    mkdir(fileBase.c_str(), 0777);

    std::vector<int> scores(popSize,0);
    tournScore=scores;
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(0,popSize-1);
    int gameCounter=0;
	omp_set_num_threads(4);
	
	#pragma omp parallel for
    for(int i=0; i<popSize; i++){
        int j=0;
        while(j<5) {
            int num = distribution(generator);
            //prevent neuralNet from playing against itself
            if(num != i) {
                tournScore[i] += playGame(gameCounter,i,num);
                j++;
                gameCounter++;
            }
        }
    }
    std::ofstream myFile(fileBase+"FINALSCORES");
    for(auto i: tournScore) {
        myFile<< std::to_string(i) + "\n";
    }
    myFile.close();
    chooseWinners();

}

int Tournament::playGame(int gameCounter, int bNum, int rNum) {
    //initalize fitness to zero
    int fitness=0;
    //set up default board
    std::string board="rrrrrrrrrrrr________bbbbbbbbbbbb";
    //initalize move tracker
    std::vector<std::string> moveTracker;
    std::vector<int> fitnessTracker;
    //push the first board state into the move tracker
    moveTracker.push_back(board);

	neuralNet redP =  neuralNet(nnVec[rNum]);
	neuralNet blackP =  neuralNet(nnVec[bNum]);
	
    //tell AI the boards they are playing on
    redP.updateBoard(board);
    blackP.updateBoard(board);
    //set upo turn counter
    int turns=0;
    //set up who's turn is first
    bool redTurn = true;
    //declare winner
    bool winner = false;
    bool bWinner = false;
    while(turns < 100) {
        if(redTurn) {
            //if red player has not lost
            if(!redP.getRedMoves().empty() && !winner) {
                board = redP.translateMove(redP.requestMove(true), board);
                moveTracker.push_back(board);
                blackP.updateBoard(board);
            } else if(redP.getRedMoves().empty() && !winner)  {
                winner = true;
                bWinner= true;
            }

            redTurn = false;
            //black moves
        } else {
            //calculate fitness function
            int ms=0;
            int mo=0;
            int ks=0;
            int ko=0;
            for(char x: board) {
                if(x=='b'){
                    ms++;
                } else if(x=='r') {
                    mo++;
                } else if(x=='B') {
                    ks++;
                } else if(x=='R') {
                    ko++;
                }
            }
            //fitness function iterates every black turn over 50 times
            fitness+= 100 + 2*ms+3*ks+2*(12-mo)+3*(12-ko);
            fitnessTracker.push_back(100 + 2*ms+3*ks+2*(12-mo)+3*(12-ko));
            //if a winner is not decided and the black player has not lost
            if(!blackP.getBlackMoves().empty() && !winner) {
                board = blackP.translateMove(blackP.requestMove(false), board);
                moveTracker.push_back(board);
                redP.updateBoard(board);
            } else if(blackP.getBlackMoves().empty() && !winner) {
                winner = true;
            }
            redTurn = true;

        }
        turns++;
    }
    if(bWinner) {
        fitness+=30000;
    }
    //write to file the game recorded
    std::string fileBase = "../neuralWeights/TOUR" + convertToGen(gen) + "/";
//    blackPlayer->saveWeights(fileBase+"GAME"+convertToGen(gameCounter));
    std::ofstream myFile(fileBase+"GAME"+convertToGen(gameCounter));
    myFile <<  "Red Player: " + convertToPop(rNum)+ " versus black Player: "+ convertToPop(bNum)+"\n";
    myFile<< "Moves:"<<+"\n";
    for(std::string x: moveTracker) {
        myFile<<x + "\n";
    }
    myFile << "Fitness Tracker:" <<"\n";
    for(int j : fitnessTracker) {
        myFile<<std::to_string(j)<<"\n";
    }
    myFile <<"Final Fitness:"<<"\n"<<std::to_string(fitness)<<"\n";
    myFile.close();
    return fitness;
}

// sort indexes via a lambda function see
//https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
template <typename T>
std::vector<size_t> sort_indexes(const std::vector<T> &v) {

    // initialize original index locations
    std::vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);

    // sort indexes based on comparing values in v
    std::sort(idx.begin(), idx.end(),
              [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

    return idx;
}

void Tournament::chooseWinners() {
    int i=0;
    gen++;
    std::vector<int> winners;
    for (auto x: sort_indexes(tournScore)) {
        if(i>=popSize/2) {
            winners.push_back(x);
        }
        i++;
    }
    std::string fileBase = "../neuralWeights/GEN" + convertToGen(gen) + "/";
    mkdir(fileBase.c_str(), 0777);
    int j=0;
    for(auto i : winners) {
        nnVec[i].saveWeights(fileBase+convertToPop(j));
        nnVec[i].evolveWeights(fileBase+convertToPop(j+1));
        j+=2;
    }

}

std::string Tournament::convertToGen(int g) {
    std::string numStr = std::to_string(g);
    int misLength = 4-numStr.size();
    std::string begStr = "";
    for(int i=0; i<misLength; i++) {
        begStr = begStr + "0";
    }
    return begStr + numStr;

}

std::string Tournament::convertToPop(int p) {
    std::string numStr = std::to_string(p);
    int misLength = 2-numStr.size();
    std::string begStr = "";
    for(int i=0; i<misLength; i++) {
        begStr = begStr + "0";
    }
    return begStr + numStr;

}

