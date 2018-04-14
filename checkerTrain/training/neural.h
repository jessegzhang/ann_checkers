// neural .h
// Jesse G. Zhang

//Header file for Class neuralNet
// Last Updated:
// March 24th 2018

//Implements a checkers AI
//Utilizes a ANN with set weights

#include <vector>
#include <string>
#include <unordered_map>
#include <random>

#ifndef CHECKERS_NEURAL_H
#define CHECKERS_NEURAL_H

class string;

class neuralNet {
public:
    //definition of one move or jump
    //ints represent boardState
    struct theMove{
        int from;
        int to;
        int over;
    };

    neuralNet();
    neuralNet(std::string b);
    std::vector<theMove> requestMove(bool whoT);
    void updateBoard(std::string b);
    std::vector<std::vector<theMove>> getRedMoves(){ return redMoves; };
    std::vector<std::vector<theMove>> getBlackMoves(){ return blackMoves; };
    std::string getBoard() {return boardKey;};
    int getCountTurn(){return countTurn;};
    void evolveWeights(std::string file);
    void loadWeights(std::string file);
    void getNeuralNet(std::vector<int> lay);
    void saveWeights(std::string file);
    std::string translateMove(std::vector<theMove> move, std::string board);

private:
    //minimax maxPlayer
    char maxPlayer;
    //activate for randomAI
    bool dumbAI;
    int countTurn;
    int generationID;
    double kingWeights;
    bool checkerBoard[8][8];
    double checkKey[8][8];

    //board state in the string
    std::string boardKey;

    //vectors for ANN
    std::vector<double> weights;
    std::vector<double> sigmaWeights;
    std::vector<int> layers;
    std::vector<double> previousLayer;
    std::vector<double> nextLayer;

    //move tables to reference for all possible moves and jumps
    std::unordered_map<int,std::vector<theMove>> redMoveTable;
    std::unordered_map<int,std::vector<theMove>> blackMoveTable;
    std::unordered_map<int,std::vector<theMove>> redJumpTable;
    std::unordered_map<int,std::vector<theMove>> blackJumpTable;

    //testing for timing
    void optimalBoardEval();
    void evalTime();
    double miniMaxEval(std::string b, char turn, int depth, double alpha, double beta);
    double miniMaxEvalAB(std::string b, char turn, int depth, double alpha, double beta);
    int miniM;
    int AB;
    int boardCount;
    int evalCount;

    //sigmoid function takes the sum of the output*weights and give the output of the sigmoid function
    double neuralEvaluate(std::string b);

    //generate moves for red and black
    std::vector<std::vector<theMove>> getRedMoves(const std::string &board);
    std::vector<std::vector<theMove>> getBlackMoves(const std::string &board);

    //Build table functions seperated for readability
    void generateRedMoveTable();
    void generateRedJumpTable();
    void generateBlackMoveTable();
    void generateBlackJumpTable();
    void makeChecker();
    int totalWeights();

    //storing redmoves and blackmoves from getRedMoves and getBlackMoves
    std::vector<std::vector<theMove>> redMoves;
    std::vector<std::vector<theMove>> blackMoves;
    double miniMax(std::string b, char turn, int depth, double alpha, double beta);
    double boardEval(std::string b, char t);

};

//defined for theMove for being stored in a set during move generation
inline bool operator<(const neuralNet::theMove& lhs, const neuralNet::theMove& rhs)
{
    return((lhs.from!=rhs.from)&&(lhs.to!=rhs.to)&&(lhs.over!=rhs.over));
}
#endif //CHECKERS_NEURAL_H
