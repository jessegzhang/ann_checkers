// neural .cpp
// Jesse G. Zhang

//initalization file for Class neuralNet
// Last Updated:
// March 24th 2018

//Implements a checkers AI
//Utilizes a ANN with set weights

#include "neural.h"
#include <iostream> //used for debugging
#include <stack> //used for move generation
#include <set> //used for move generation
#include <algorithm> //used for max
#include <iterator> //used for iteration
#include <chrono> //used for timing analytics
#include <fstream> //used for file reading and writing


//neuralNet
//Default constructor for neuralNet
//Default generation for a neuralNet with no weights built
//Requires a board be initalized and therefore countTurn starts at 0
neuralNet::neuralNet(){
    //generates moveTables
    generateRedMoveTable();
    generateRedJumpTable();
    generateBlackMoveTable();
    generateBlackJumpTable();
    dumbAI=false;
    countTurn =-1;
}

//neuralNet
//Constructor call with board state
//Generates all moves with given board
neuralNet::neuralNet(std::string b, bool ai){
    for(char x : b) {
        boardKey.push_back(x);
    }

    //generates moveTables
    generateRedMoveTable();
    generateRedJumpTable();
    generateBlackMoveTable();
    generateBlackJumpTable();
    makeChecker();
    //std::cout<<totalWeights()<<std::endl;

    countTurn =0;

    //generate the first set of moves
    redMoves = getRedMoves(boardKey);
    blackMoves = getBlackMoves(boardKey);

    //ENABLE FOR ANALYTICS
//    std::vector<int> v = {91, 40, 10, 1};
//    getNeuralNet(v);
//    saveWeights("../analytics/test");
    //double test=neuralEvaluate("rrrrrrrrrrrr________bbbbbbbbbbbb");
    //std::cout<<"myanswer: "<<test<<std::endl;
    //loadWeights("../neuralWeights/GEN0032/17");
    //evalTime();


    //initalization of piece count vs non piece count
    dumbAI=ai;

}

//requestMove
//input whoT is true if it is red moves turn, false if it is black moves turn
//responds with a length 32 string of the board state they want
std::vector<neuralNet::theMove> neuralNet::requestMove(bool whoT) {
    //countTurn++;
    //random AI Moves
    //This is the flag for turning on the dumbAI that does moves at random
    //Training should look to beat this and piece count
    if(true){
        //initalization of the possible moves
        std::vector<std::vector<theMove>> origMoves;
        //initalizes maxPlayer for the minimax tree with alpha beta pruning
        if (whoT) {
             origMoves = getRedMoves(boardKey);
            maxPlayer = 'r';
        } else {
            origMoves = getBlackMoves(boardKey);
            maxPlayer = 'b';
        }
        int it = 0;
        double val = -1000;
        //calls the minimax with the first depth
        for (int i = 0; i < origMoves.size(); i++) {
            //does a miniMax tree with a depth of 8
            double value;
            if (whoT) {
                //red turn max
                value = miniMaxPC(translateMove(origMoves[i], boardKey), 'b', 9, -100000, 100000);
            } else {
                value = miniMaxPC(translateMove(origMoves[i], boardKey), 'r', 9, -100000, 100000);
            }

            if (value > val) {
                it = i;
                val = value;
            }

        }
        //translates the best move obtained from minimax
        boardKey=translateMove(origMoves[it], boardKey);
        //sends the move
        return origMoves[it];
    }

        //smartAIMoves
    } else {
        //initalization of the possible moves
        std::vector<std::vector<theMove>> origMoves;
        //initalizes maxPlayer for the minimax tree with alpha beta pruning
        if (whoT) {
             origMoves = getRedMoves(boardKey);
            maxPlayer = 'r';
        } else {
            origMoves = getBlackMoves(boardKey);
            maxPlayer = 'b';
        }
        int it = 0;
        double val = -1000;
        //calls the minimax with the first depth
        for (int i = 0; i < origMoves.size(); i++) {
            //does a miniMax tree with a depth of 8
            double value;
            if (whoT) {
                //red turn max
                value = miniMax(translateMove(origMoves[i], boardKey), 'b', 7, -100000, 100000);
            } else {
                value = miniMax(translateMove(origMoves[i], boardKey), 'r', 7, -100000, 100000);
            }

            if (value > val) {
                it = i;
                val = value;
            }

        }
        //translates the best move obtained from minimax
        boardKey=translateMove(origMoves[it], boardKey);
        //sends the move
        return origMoves[it];
    }

}

//getBlackMoves
//generates all possible black moves
//returns all possible moves in a vector of vector of theMove struct type
std::vector<std::vector<neuralNet::theMove>> neuralNet::getBlackMoves(const std::string &board) {
    std::vector<std::vector<theMove>> listofMoves;
    std::string b = board;
    for (int i = 0; i < b.size(); i++) {
        //normal black checker piece
        if (b[i] == 'b') {
            for (theMove x : blackJumpTable[i + 1]) {
                std::stack<theMove> jumpTraverse;
                std::set<theMove> visited;
                std::vector<theMove> moveList;
                if (b[x.to - 1] == '_') {
                    if (b[x.over - 1] == 'r' || b[x.over - 1] == 'R') {
                        jumpTraverse.push(x);
                    }
                }
                while(!jumpTraverse.empty()){
                    theMove current = jumpTraverse.top();
                    jumpTraverse.pop();


                    //if node has not been visited already
                    if(visited.count(current)==0){

                        moveList.push_back(current);
                        visited.insert(current);
                    }
                    int it=0;
                    for(theMove adj: blackJumpTable[current.to]){
                        if (b[adj.to - 1] == '_'&& visited.count(adj) == 0) {
                            if (b[adj.over - 1] == 'r' || b[adj.over - 1] == 'R') {
                                it++;
                                jumpTraverse.push(adj);
                            }
                        }
                    }

                    if( it==0) {
                        listofMoves.push_back(moveList);
                        moveList.pop_back();
                    }
                }
            }
        } else if (b[i] == 'B') {
            std::vector<theMove> checkMoves;
            checkMoves.reserve(blackJumpTable[i + 1].size() + redJumpTable[i + 1].size());
            checkMoves.insert(checkMoves.end(), blackJumpTable[i + 1].begin(), blackJumpTable[i + 1].end());
            checkMoves.insert(checkMoves.end(), redJumpTable[i + 1].begin(), redJumpTable[i + 1].end());
            for (theMove x : checkMoves) {
                std::stack<theMove> jumpTraverse;
                std::set<theMove> visited;
                std::set<int> jumpedFrom;
                std::vector<theMove> moveList;

                if (b[x.to - 1] == '_') {
                    if (b[x.over - 1] == 'r' || b[x.over - 1] == 'R') {
                        jumpTraverse.push(x);
                    }
                }
                while (!jumpTraverse.empty()) {
                    theMove current = jumpTraverse.top();
                    jumpTraverse.pop();


                    //if node has not been visited already

                    if (visited.count(current) == 0) {
                        jumpedFrom.insert(current.from-1);
                        moveList.push_back(current);
                        visited.insert(current);
                    }
                    int it = 0;
                    std::vector<theMove> checkinMoves;
                    checkinMoves.reserve(blackJumpTable[current.to].size() + redJumpTable[current.to].size());
                    checkinMoves.insert(checkinMoves.end(), blackJumpTable[current.to].begin(),
                                        blackJumpTable[current.to].end());
                    checkinMoves.insert(checkinMoves.end(), redJumpTable[current.to].begin(),
                                        redJumpTable[current.to].end());
                    for (theMove adj: checkinMoves) {
                        if ((b[adj.to - 1] == '_' || jumpedFrom.count(adj.to-1) ==1) && visited.count(adj) == 0 && adj.to!=current.from) {
                            if (b[adj.over - 1] == 'r' || b[adj.over - 1] == 'R') {
                                it++;
                                jumpTraverse.push(adj);
                            }
                        }
                    }

                    if (it == 0) {
                        listofMoves.push_back(moveList);
                        moveList.pop_back();
                    }
                }
            }
        }

    }

    if (listofMoves.empty()) {
        for (int i = 0; i < b.size(); i++) {

            if (b[i] == 'b') {
                for (theMove x : blackMoveTable[i + 1]) {
                    //if the move is valid make a vector and push it
                    if (b[x.to - 1] == '_') {
                        std::vector<theMove> singleMove;
                        singleMove.push_back(x);
                        listofMoves.push_back(singleMove);
                    }
                }
            } else if(b[i]=='B') {
                for (theMove x : blackMoveTable[i + 1]) {
                    //if the move is valid make a vector and push it
                    if (b[x.to - 1] == '_') {
                        std::vector<theMove> singleMove;
                        singleMove.push_back(x);
                        listofMoves.push_back(singleMove);
                    }
                }
                for (theMove x : redMoveTable[i + 1]) {
                    //if the move is valid make a vector and push it
                    if (b[x.to - 1] == '_') {
                        std::vector<theMove> singleMove;
                        singleMove.push_back(x);
                        listofMoves.push_back(singleMove);
                    }
                }
            }
        }
    }
    return listofMoves;
}

int neuralNet::totalWeights(){
    int weightNum=0;
    int total=0;
    //all 3x3 subsquares
    for(int i=0; i<6; i++) {
        for(int j=0; j<6; j++) {
            total++;
            for(int k=j; k<j+3; k++){
                for(int l=i; l<i+3; l++) {
                    if(checkerBoard[l][k]){
                        weightNum++;
                    }
                }
            }
        }
    }

    //all 4x4 subsquares
    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            total++;
            for(int k=j; k<j+4; k++){
                for(int l=i; l<i+4; l++) {
                    if(checkerBoard[l][k]){
                        weightNum++;
                    }
                }
            }
        }
    }

    //all 5x5 subsquares
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            total++;
            for(int k=j; k<j+5; k++){
                for(int l=i; l<i+5; l++) {
                    if(checkerBoard[l][k]){
                        weightNum++;
                    }
                }
            }
        }
    }

    //all 6x6 subsquares
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            total++;
            for(int k=j; k<j+6; k++){
                for(int l=i; l<i+6; l++) {
                    if(checkerBoard[l][k]){
                        weightNum++;
                    }
                }
            }
        }
    }

    //all 7x7 subsquares
    for(int i=0; i<2; i++) {
        for(int j=0; j<2; j++) {
            total++;
            for(int k=j; k<j+7; k++){
                for(int l=i; l<i+7; l++) {
                    if(checkerBoard[l][k]){
                        weightNum++;
                    }
                }
            }
        }
    }

    total++;
    weightNum+=32;


    std::cout<<"total: "<<total<<std::endl;
    return weightNum;
}
//getRedMoves
//generates all possible black moves
//returns all possible moves in a vector of vector of theMove struct type
std::vector<std::vector<neuralNet::theMove>> neuralNet::getRedMoves(const std::string &board) {
    std::vector<std::vector<theMove>> listofMoves;
    std::string b = board;
    for (int i = 0; i < b.size(); i++) {
        //normal red checker piece
        if (b[i] == 'r') {
            for (theMove x : redJumpTable[i + 1]) {
                std::stack<theMove> jumpTraverse;
                std::set<theMove> visited;
                std::vector<theMove> moveList;
                if (b[x.to - 1] == '_') {
                    if (b[x.over - 1] == 'b' || b[x.over - 1] == 'B') {
                        jumpTraverse.push(x);
                    }
                }
                while (!jumpTraverse.empty()) {
                    theMove current = jumpTraverse.top();
                    jumpTraverse.pop();


                    //if node has not been visited already
                    if (visited.count(current) == 0) {
                        moveList.push_back(current);
                        visited.insert(current);
                    }
                    int it = 0;
                    for (theMove adj: redJumpTable[current.to]) {
                        if (b[adj.to - 1] == '_' && visited.count(adj) == 0) {
                            if (b[adj.over - 1] == 'b' || b[adj.over - 1] == 'B') {
                                it++;
                                jumpTraverse.push(adj);
                            }
                        }
                    }

                    if (it == 0) {
                        listofMoves.push_back(moveList);
                        moveList.pop_back();
                    }
                }
            }
            //black checker logic
        } else if (b[i] == 'R') {
            std::vector<theMove> checkMoves;
            checkMoves.reserve(blackJumpTable[i + 1].size() + redJumpTable[i + 1].size());
            checkMoves.insert(checkMoves.end(), blackJumpTable[i + 1].begin(), blackJumpTable[i + 1].end());
            checkMoves.insert(checkMoves.end(), redJumpTable[i + 1].begin(), redJumpTable[i + 1].end());
            for (theMove x : checkMoves) {
                std::stack<theMove> jumpTraverse;
                std::set<theMove> visited;
                std::vector<theMove> moveList;


                if (b[x.to - 1] == '_') {
                    if (b[x.over - 1] == 'b' || b[x.over - 1] == 'B') {
                        jumpTraverse.push(x);
                    }
                }
                while (!jumpTraverse.empty()) {
                    theMove current = jumpTraverse.top();
                    jumpTraverse.pop();


                    //if node has not been visited already
                    if (visited.count(current) == 0) {
                        moveList.push_back(current);
                        visited.insert(current);
                    }
                    int it = 0;
                    std::vector<theMove> checkinMoves;
                    checkinMoves.reserve(blackJumpTable[current.to].size() + redJumpTable[current.to].size());
                    checkinMoves.insert(checkinMoves.end(), blackJumpTable[current.to].begin(),
                                        blackJumpTable[current.to].end());
                    checkinMoves.insert(checkinMoves.end(), redJumpTable[current.to].begin(),
                                        redJumpTable[current.to].end());
                    for (theMove adj: checkinMoves) {
                        if (b[adj.to - 1] == '_' && visited.count(adj) == 0 && adj.to!=current.from) {
                            if (b[adj.over - 1] == 'b' || b[adj.over - 1] == 'B') {
                                it++;
                                jumpTraverse.push(adj);
                            }
                        }
                    }

                    if (it == 0) {
                        listofMoves.push_back(moveList);
                        moveList.pop_back();
                    }
                }
            }
        }
    }

    if (listofMoves.empty()) {
        for (int i = 0; i < b.size(); i++) {
            if (b[i] == 'r') {
                for (theMove x : redMoveTable[i + 1]) {
                    if (b[x.to - 1] == '_') {
                        std::vector<theMove> singleRedMove;
                        singleRedMove.push_back(x);
                        listofMoves.push_back(singleRedMove);
                    }
                }
            } else if(b[i]=='R') {
                for (theMove x : blackMoveTable[i + 1]) {
                    //if the move is valid make a vector and push it
                    if (b[x.to - 1] == '_') {
                        std::vector<theMove> singleMove;
                        singleMove.push_back(x);
                        listofMoves.push_back(singleMove);
                    }
                }
                for (theMove x : redMoveTable[i + 1]) {
                    //if the move is valid make a vector and push it
                    if (b[x.to - 1] == '_') {
                        std::vector<theMove> singleMove;
                        singleMove.push_back(x);
                        listofMoves.push_back(singleMove);
                    }
                }
            }
        }
    }

    return listofMoves;
}

//translateMove
//takes given vector of theMove, and takes a string which represents board
//returns string of theMovet translated onto the board, also ensures Kings are done properly
std::string neuralNet::translateMove(std::vector<theMove> move, std::string board) {
    std::string boardCopy = board;
    //if a simple move

    if(move[0].over==-1) {
        boardCopy[move[0].to-1]=boardCopy[move[0].from-1];
        boardCopy[move[0].from-1]='_';
        //if a jump we need to calculate the estimated jump state
    } else {
        char orig=boardCopy[move[0].from-1];
        for(int i=0; i<move.size(); i++) {
            boardCopy[move[i].over-1]='_';
        }
        boardCopy[move[move.size()-1].to-1]=orig;
        boardCopy[move[0].from-1]='_';
    }
    int j=31;
    for(int i=0; i<4; i++) {
        if(boardCopy[i] == 'b') {
            boardCopy[i] = 'B';
        }
        if(boardCopy[j] == 'r') {
            boardCopy[j] = 'R';
        }
        j--;
    }

    return boardCopy;

}

//boardEval
//given a board state and the char for whos turn
//spits out piececount of red-black or black-red based on char t
//Kings are default valued at 1.5 as this is used solely by piececount and
//not an ANN
double neuralNet::boardEval(std::string b, char t) {
    double reds=0;
    double blacks=0;
    for (char x : b) {
        if(x=='r'){
            reds++;
        } else if(x=='R') {
            reds=reds+kingWeights;
        } else if(x=='b') {
            blacks++;
        } else if(x=='B') {
            blacks = blacks+kingWeights;
        }
    }
    if(t=='r') {
        return reds-blacks;
    } else if(t=='b') {
        return blacks-reds;
    }
}

double neuralNet::miniMaxPC(std::string b, char turn, int depth, double alpha, double beta) {
    if(depth==0 && turn == 'r') {
        return boardEval(b, 'r');
    } else if(depth==0 && turn == 'b') {
        return boardEval(b, 'b');
    }
    std::vector<std::vector<neuralNet::theMove>> moveList;

    if(turn == 'r') {
        moveList = getRedMoves(b);
    } else if(turn == 'b') {
        moveList = getBlackMoves(b);
    }

    //maxPlayer is what denotes maxNode vs minNode
    if(turn == maxPlayer) {
        double bestVal = -100000;
        //maxNode for red
        if(turn == 'r') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMax(translateMove(x , b), 'b', depth-1, alpha, beta);
                bestVal = std::max(bestVal, value);
                alpha = std::max(alpha, bestVal);
                if(beta <= alpha) {
                    break;
                }
            }
            return bestVal;
            // maxNode for black
        } else if(turn == 'b') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMax(translateMove(x , b), 'r', depth-1, alpha, beta);
                bestVal = std::max(bestVal, value);
                alpha = std::max(alpha, bestVal);
                if(beta <= alpha) {
                    break;
                }
            }
            return bestVal;
        }

    } else  {
        double bestVal=100000;
        if(turn == 'r') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMax(translateMove(x , b), 'b', depth-1, alpha, beta);
                bestVal = std::min(bestVal, value);
                beta = std::min(bestVal, beta);
                if (beta <= alpha) {
                    break;
                }
            }
            return bestVal;
        } else if(turn == 'b') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMax(translateMove(x , b), 'r', depth-1, alpha, beta);
                bestVal = std::min(bestVal, value);
                beta = std::min(bestVal, beta);
                if(beta <= alpha) {
                    break;
                }
            }
            return bestVal;
        }

    }
}

//miniMax
//miniMax called with string representing board, who's turn
//current depth, alpha, and beta
//tree is called recursively with alpha beta pruning
double neuralNet::miniMax(std::string b, char turn, int depth, double alpha, double beta) {
    if(depth==0 && turn == 'r') {
        return neuralEvaluate(b);
    } else if(depth==0 && turn == 'b') {
        return -1*neuralEvaluate(b);
    }
    std::vector<std::vector<neuralNet::theMove>> moveList;

    if(turn == 'r') {
        moveList = getRedMoves(b);
    } else if(turn == 'b') {
        moveList = getBlackMoves(b);
    }

    //maxPlayer is what denotes maxNode vs minNode
    if(turn == maxPlayer) {
        double bestVal = -100000;
        //maxNode for red
        if(turn == 'r') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMax(translateMove(x , b), 'b', depth-1, alpha, beta);
                bestVal = std::max(bestVal, value);
                alpha = std::max(alpha, bestVal);
                if(beta <= alpha) {
                    break;
                }
            }
            return bestVal;
            // maxNode for black
        } else if(turn == 'b') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMax(translateMove(x , b), 'r', depth-1, alpha, beta);
                bestVal = std::max(bestVal, value);
                alpha = std::max(alpha, bestVal);
                if(beta <= alpha) {
                    break;
                }
            }
            return bestVal;
        }

    } else  {
        double bestVal=100000;
        if(turn == 'r') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMax(translateMove(x , b), 'b', depth-1, alpha, beta);
                bestVal = std::min(bestVal, value);
                beta = std::min(bestVal, beta);
                if (beta <= alpha) {
                    break;
                }
            }
            return bestVal;
        } else if(turn == 'b') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMax(translateMove(x , b), 'r', depth-1, alpha, beta);
                bestVal = std::min(bestVal, value);
                beta = std::min(bestVal, beta);
                if(beta <= alpha) {
                    break;
                }
            }
            return bestVal;
        }

    }
}

//updateBoard
//takes a move and updates the internal board of the AI
//is called when the oponent makes the move
//internal checks for cheating should be given here
void neuralNet::updateBoard(std::string b) {
    //countTurn++;
    boardKey=b;
    redMoves = getRedMoves(boardKey);
    blackMoves = getBlackMoves(boardKey);
//    std::vector<std::vector<theMove>> origMoves;
//    std::cout<<"Red Possible Moves"<<std::endl;
//    origMoves = getRedMoves(boardKey);
//    for(int i=0; i<origMoves.size();i++) {
//        std::cout<<translateMove(origMoves[i],boardKey)<<std::endl;
//    }
//    std::cout<<"Black Possible Moves"<<std::endl;
//    origMoves = getBlackMoves(boardKey);
//    for(int i=0; i<origMoves.size();i++) {
//        std::cout<<translateMove(origMoves[i],boardKey)<<std::endl;
//    }
}

//saveWeights
//given string file saves all the weights into given file format
//also saves sigma values, king weights, and layers
void neuralNet::saveWeights(std::string file) {
    std::ofstream myFile(file);
    myFile << std::to_string(generationID) +"\n";
    myFile << std::to_string(kingWeights) + "\n";
    myFile <<std::to_string(layers.size())+"\n";

    //get the layers file
    for(int i=0; i<layers.size(); i++) {
    myFile<<std::to_string(layers[i]) + "\n";
    }

    //write total weights
    //get the weights and write to a file
    myFile << std::to_string(weights.size())+"\n";

    //write weights
    for(int j=0; j<weights.size(); j++) {
        myFile<<std::to_string(weights[j])+"\n";
    }

    //write sigmaWeights
    for(int k=0; k<sigmaWeights.size(); k++) {
        myFile<<std::to_string(sigmaWeights[k])+"\n";
    }

    myFile.close();



}

//loadWeights
//generates all possible black moves
//returns all possible moves in a vector of vector of theMove struct type
void neuralNet::loadWeights(std::string file) {
    weights.clear();
    sigmaWeights.clear();
    layers.clear();

    std::string inputString = "";
    std::ifstream myFile(file);

    //get generationID
    getline (myFile,inputString);
    generationID = std::stoi(inputString);


    //get kingWeight
    getline (myFile,inputString);
    kingWeights = std::stod(inputString);

    //get layers
    getline (myFile,inputString);
    int layNum = std::stoi(inputString);

    //generate layer vector
    for(int i=0; i<layNum;i++) {
        getline (myFile,inputString);
        layers.push_back(std::stoi(inputString));
    }

    //get weights
    getline (myFile,inputString);
    int weightNum = std::stoi(inputString);


    for(int j=0; j<weightNum; j++) {
        getline (myFile,inputString);
        weights.push_back(std::stod(inputString));
    }

    //get sigmaWeights
    for(int k=0; k<weightNum; k++) {
        getline (myFile,inputString);
        sigmaWeights.push_back(std::stod(inputString));
    }

}

//evolveWeights
//takes given sigmaWeight values
//and tunes the weights then saves it into given file string
void neuralNet::evolveWeights(std::string file){
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    generationID++;
    std::uniform_real_distribution<double> kingShift (-0.1, 0.1);
    //shift king weight on a uniform distribution
    kingWeights = kingWeights + kingShift(generator);

    double tau = 1/sqrt(2*sqrt(weights.size()));
    std::normal_distribution<double> weightDistribution (0, 1);
    //evolve sigma and weights based on set requirements defined in homework
    for(int j=0; j<weights.size(); j++) {
        sigmaWeights[j]= sigmaWeights[j]*exp(tau*weightDistribution(generator));
	if(sigmaWeights[j]>1 || sigmaWeights[j]<0) {
	    sigmaWeights[j]=0.05;
	}
        weights[j]= weights[j] + sigmaWeights[j]*weightDistribution(generator);
        if(weights[j]>1) {
            weights[j]=1;
        }
        if(weights[j]<-1) {
            weights[j]=-1;
        }
    }

    saveWeights(file);

}

//getNeuralNet
//constructor for a neuralNet weights given a vector
//describing each weights in each layer
void neuralNet::getNeuralNet(std::vector<int> lay) {
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    kingWeights=0;
    weights.clear();
    sigmaWeights.clear();
    //generate kingWeights
    generationID=0;
    std::uniform_real_distribution<double> kingDistribution (0.0, 3.0);
    kingWeights = kingDistribution(generator);
    //preprocessing layer of 854 weights
    int totalWeights=945;
    for (int i=0; i < lay.size()-1;i++) {
        //+1 for each bias added on
        int weightCount = (lay[i]+1)*(lay[i+1]);
        totalWeights+=weightCount;
    }
    totalWeights++;

    layers=lay;

    std::uniform_real_distribution<double> weightDistribution (-0.2, 0.2);
    for(int k = 0; k<totalWeights; k++){
        double num = weightDistribution(generator);
        //push back weight
        weights.push_back(num);
        //push default sigma weight
        sigmaWeights.push_back(0.05);
    }



}

//neuralEvaluate
//takes given string represnting board
//and pushes it through the ANN
double neuralNet::neuralEvaluate(std::string b) {
    previousLayer.clear();
    nextLayer.clear();
    int layerNum = 1;
    //Load up the input vector
    int ind=0;
    for(int i=0; i < 8; i++) {
        for(int j=0; j < 8; j++) {
            if(checkerBoard[i][j]) {
                if(b[ind] == 'r') {
                    checkKey[i][j]=1;
                } else if(b[ind] == 'b') {
                    checkKey[i][j]=-1;
                } else if(b[ind] == '_') {
                    checkKey[i][j]=0;
                } else if(b[ind] == 'R') {
                    checkKey[i][j] = kingWeights;
                } else if(b[ind] == 'B') {
                    checkKey[i][j] = -1*kingWeights;
                }
                ind++;
            }
        }
    }


    //start working through all possible weights
    int wT=0;
    double wTotal=0;
    //all 3x3 subsquares
    for(int i=0; i<6; i++) {
        for(int j=0; j<6; j++) {
            for(int k=j; k<j+3; k++){
                for(int l=i; l<i+3; l++) {
                    if(checkerBoard[l][k]){
                        wTotal+=weights[wT]*checkKey[l][k];
                        wT++;
                    }
                }
            }
            wTotal+=weights[wT];
            wT++;
            previousLayer.push_back(tanh(wTotal));
            wTotal=0;
        }
    }
    //all 4x4 subsquares
    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            for(int k=j; k<j+4; k++){
                for(int l=i; l<i+4; l++) {
                    if(checkerBoard[l][k]){
                        wTotal+=weights[wT]*checkKey[l][k];
                        wT++;
                    }
                }
            }
            wTotal+=weights[wT];
            wT++;
            previousLayer.push_back(tanh(wTotal));
            wTotal=0;
        }
    }

    //all 5x5 subsquares
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            for(int k=j; k<j+5; k++){
                for(int l=i; l<i+5; l++) {
                    if(checkerBoard[l][k]){
                        wTotal+=weights[wT]*checkKey[l][k];
                        wT++;
                    }
                }
            }
            wTotal+=weights[wT];
            wT++;
            previousLayer.push_back(tanh(wTotal));
            wTotal=0;
        }
    }

    //all 6x6 subsquares
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            for(int k=j; k<j+6; k++){
                for(int l=i; l<i+6; l++) {
                    if(checkerBoard[l][k]){
                        wTotal+=weights[wT]*checkKey[l][k];
                        wT++;
                    }
                }
            }
            wTotal+=weights[wT];
            wT++;
            previousLayer.push_back(tanh(wTotal));
            wTotal=0;
        }
    }

    //all 7x7 subsquares
    for(int i=0; i<2; i++) {
        for(int j=0; j<2; j++) {
            for(int k=j; k<j+7; k++){
                for(int l=i; l<i+7; l++) {
                    if(checkerBoard[l][k]){
                        wTotal+=weights[wT]*checkKey[l][k];
                        wT++;
                    }
                }
            }
            wTotal+=weights[wT];
            wT++;
            previousLayer.push_back(tanh(wTotal));
            wTotal=0;
        }
    }

    //all 8x8 subsquares
    for(int i=0; i < 8; i++){
        for(int j=0; j < 8; j++) {
            if(checkerBoard[i][j]){
                wTotal+=weights[wT]*checkKey[i][j];
                wT++;
            }
        }
    }
    wTotal+=weights[wT];
    wT++;
    previousLayer.push_back(tanh(wTotal));
    wTotal=0;




    //grab value of the last weights for the final computation
    //piece differential
    double toFinal = weights[wT]*boardEval(b, 'r');
    wT++;


    //tracker to see about reaching the end of a layer
    //while loop calculates till last layer
    while(layerNum<layers.size()-1) {

        for(int i=0; i<layers[layerNum]; i++) {
            double sum=0;
            for(int j=0; j<layers[(layerNum-1)]; j++) {
                sum+=previousLayer[j]*weights[wT];
                wT++;
            }
            //bias weight
            sum+=weights[wT];
            wT++;
            nextLayer.push_back(tanh(sum));
        }
        previousLayer=nextLayer;
        nextLayer.clear();
        layerNum++;
    }
    double retSum=0;
    for(int k=0; k<layers[layerNum-1]; k++) {
        retSum+=previousLayer[k]*weights[wT];
        wT++;
    }
    retSum+=weights[wT];
    retSum+=toFinal;
    return tanh(retSum);
}

//evalTime
//used for evaluating neural network with weights
//used for analytics
void neuralNet::evalTime() {
    std::vector<int> v = {91, 40, 10, 1};
    getNeuralNet(v);
    //evaluate 10000 boards
    std::cout<<"Starting up evaluation"<<std::endl;
    auto start = std::chrono::system_clock::now();
    for(int i = 0; i<10000;i++) {
        double val_1  = neuralEvaluate("rrrrrr_r__rr_rr____br_b__bbbb_bb");
    }
    auto end = std::chrono::system_clock::now();
    std::cout<<"Average neuralNet evaluation time: "<< std::chrono::duration_cast
            <std::chrono::milliseconds>(end-start).count()
             << " milliseconds\n";
    optimalBoardEval();
    //Testing done here
    //    generate neuralNet
        std::vector<int> layNum = {91, 40, 10, 1};

    //getNeuralNet
    getNeuralNet(layNum);
    std::cout<<std::endl<<"Starting check on writes/reads/and evolves"<<std::endl;

    //Test to show it saves and loads properly
    saveWeights("../analytics/test");
    std::vector<double> oldWeights = weights;
    std::vector<double> oldSWeights = sigmaWeights;
    std::vector<int> oldLayer = layers;
    double oldKingW = kingWeights;
    loadWeights("../analytics/test");
    std::cout<<"Weights match from loading: ";
    if(oldWeights==weights){
        std::cout<<"True"<<std::endl;
    } else {
        std::cout<<"False"<<std::endl;
        double weightDiff=0;
        for(int i=0; i<oldWeights.size(); i++) {
            weightDiff+=oldWeights[i]-weights[i];
        }
        std::cout<<"Difference in weights: "<<weightDiff<<std::endl;
    }
    std::cout<<"Sigma Weights match from loading: ";
    if(oldSWeights==sigmaWeights){
        std::cout<<"True"<<std::endl;
    } else {
        std::cout<<"False"<<std::endl;
    }
    std::cout<<"Layers match from loading: ";
    if(oldLayer==layers){
        std::cout<<"True"<<std::endl;
    } else {
        std::cout<<"False"<<std::endl;
    }
    std::cout<<"king weights match from loading: ";
    if(oldKingW==kingWeights) {
        std::cout<<"True"<<std::endl;
    } else {
        std::cout<<"False"<<std::endl;
        std::cout<<oldKingW<< " vs "<<kingWeights<<std::endl;
    }
    saveWeights("../analytics/test2");

    //evolve for testing
    evolveWeights("../analytics/testE");
    loadWeights("../analytics/testE");
    double sum=0;
    for(int i=0; i<oldWeights.size(); i++) {
        sum+=weights[i]-oldWeights[i];
    }
    std::cout<<std::endl<<"Starting check on evolution function: "<<std::endl;
    //according to evolution this should be a modified sigma value approximately around 0.5 multiplied
    //by a normal distribution with mean 0 and standard deviation 1
    //expected value of sigma should be approximately 0.5
    //and expected value of normal should be 0 therefore this sum should be approximately near 0
    std::cout<<"sum of weights vs old weights average value "<<sum/oldWeights.size()<<std::endl;
    std::cout<<"Expected value: "<<0<<std::endl;

    //according to evolution sigma'(j) = sigma(j) exp(tau *(0,1))   j=1,...,n
    //Note we assume that sigma(j) and exp(tau*N(0,1)) are independent therefore
    //E(sigma'(j))=E(sigma(j))*E(exp(tau*N(0,1))
    //The expected value of exp(tau Y) where Y~N(0,1) is e^(tau*(0)+1/2) or e^(tau*0.5)
    //therefore our expected value is 0.5*e^(tau*0.5)
    double tau = 1/sqrt(2*sqrt(weights.size()));
    double sigmaSum=0;
    for(int i=0; i<sigmaWeights.size(); i++) {
        sigmaSum+=sigmaWeights[i];
    }
    std::cout<<"Sigma average value: "<<sigmaSum/sigmaWeights.size()<<std::endl;
    std::cout<<"Expected value: "<<"0.0050421714";

    std::cout<<std::endl<<"Starting check on Alpha Beta Savings: "<<std::endl;


    std::vector<std::vector<theMove>> origMoves;

    std::vector<std::string> boards;
    boards.push_back("rrrrrrrrrrrr________bbbbbbbbbbbb");
    boards.push_back("r__rrrrr____r_br_b___b__b__bbbbb");
    boards.push_back("r__r___rr______r____bb_____bb__b");
    maxPlayer = 'r';
    double savSum=0;
    for(int i=0; i<boards.size();i++) {
        AB=0;
        miniM=0;
        boardCount=0;
        evalCount=0;
        origMoves = getRedMoves(boards[i]);
        int it = 0;
        double val = -1000;
        //calls the minimax with the first depth
        boardCount++;
        evalCount+=origMoves.size();
        for (int i = 0; i < origMoves.size(); i++) {
            //does a miniMax tree with a depth of 8

            double value_1;
            double value_2;
            value_1 = miniMaxEval(translateMove(origMoves[i], boardKey), 'b', 5, -100000, 100000);
            value_2 = miniMaxEvalAB(translateMove(origMoves[i], boardKey), 'b', 5, -100000, 100000);
            if(value_1!=value_2) {
                std::cout << "alpha beta with min max and min max results do not match!"<<std::endl;
                std::cout << value_1<< " vs "<<value_2<<std::endl;
            }
        }
        std::cout<<AB<<" vs "<<miniM<<std::endl;
        savSum+=AB/miniM;
        std::cout<<"Ply depth 8"<<std::endl
                 <<"Boards evaluated per move: "<<(double)evalCount/boardCount
                 <<std::endl<<"Board Evaluation Functions called: "<<miniM<<std::endl
                 <<"Inner vs Leaf Nodes: "<<evalCount-miniM <<" vs "<< evalCount<<std::endl;
    }

    for(int i=0; i<boards.size();i++) {
        AB=0;
        miniM=0;
        boardCount=0;
        evalCount=0;
        origMoves = getRedMoves(boards[i]);
        int it = 0;
        double val = -1000;
        //calls the minimax with the first depth
        boardCount++;
        evalCount+=origMoves.size();
        for (int i = 0; i < origMoves.size(); i++) {
            //does a miniMax tree with a depth of 8

            double value_1;
            double value_2;
            value_1 = miniMaxEval(translateMove(origMoves[i], boardKey), 'b', 5, -100000, 100000);
            value_2 = miniMaxEvalAB(translateMove(origMoves[i], boardKey), 'b', 5, -100000, 100000);
            if(value_1!=value_2) {
                std::cout << "alpha beta with min max and min max results do not match!"<<std::endl;
                std::cout << value_1<< " vs "<<value_2<<std::endl;
            }
        }
        std::cout<<AB<<" vs "<<miniM<<std::endl;
        savSum+=AB/miniM;
        std::cout<<"Ply depth 6"<<std::endl
                 <<"Boards evaluated per move: "<<(double)evalCount/boardCount
                 <<std::endl<<"Board Evaluation Functions called: "<<miniM<<std::endl
                 <<"Inner vs Leaf Nodes: "<<evalCount-miniM <<" vs "<< evalCount<<std::endl;
    }

    for(int i=0; i<boards.size();i++) {
        AB=0;
        miniM=0;
        boardCount=0;
        evalCount=0;
        origMoves = getRedMoves(boards[i]);
        int it = 0;
        double val = -1000;
        //calls the minimax with the first depth
        boardCount++;
        evalCount+=origMoves.size();
        for (int i = 0; i < origMoves.size(); i++) {
            //does a miniMax tree with a depth of 8

            double value_1;
            double value_2;
            value_1 = miniMaxEval(translateMove(origMoves[i], boardKey), 'b', 3, -100000, 100000);
            value_2 = miniMaxEvalAB(translateMove(origMoves[i], boardKey), 'b', 3, -100000, 100000);
            if(value_1!=value_2) {
                std::cout << "alpha beta with min max and min max results do not match!"<<std::endl;
                std::cout << value_1<< " vs "<<value_2<<std::endl;
            }
        }
        std::cout<<AB<<" vs "<<miniM<<std::endl;
        savSum+=AB/miniM;
        std::cout<<"Ply depth 4"<<std::endl
                 <<"Boards evaluated per move: "<<(double)evalCount/boardCount
                 <<std::endl<<"Board Evaluation Functions called: "<<miniM<<std::endl
                 <<"Inner vs Leaf Nodes: "<<evalCount-miniM <<" vs "<< evalCount<<std::endl;
    }


    maxPlayer = 'b';
    for(int i=0; i<boards.size();i++) {
        AB=0;
        miniM=0;
        boardCount=0;
        evalCount=0;
        origMoves = getBlackMoves(boards[i]);
        int it = 0;
        double val = -1000;
        //calls the minimax with the first depth
        for (int i = 0; i < origMoves.size(); i++) {
            //does a miniMax tree with a depth of 8
            double value_1;
            double value_2;
            value_1 = miniMaxEval(translateMove(origMoves[i], boardKey), 'r', 7, -100000, 100000);
            value_2 = miniMaxEvalAB(translateMove(origMoves[i], boardKey), 'r', 7, -100000, 100000);
            if(value_1!=value_2) {
                std::cout << "alpha beta with min max and min max results do not match!"<<std::endl;
                std::cout << value_1<< " vs "<<value_2<<std::endl;
            }
        }
        std::cout<<AB<<" vs "<<miniM<<std::endl;
        savSum+=AB/miniM;

    }

    std::cout<<"Average savings from alpha beta pruning: "<<1-(savSum/20)<<std::endl;

}

//optimalBoardEval
//spits out optimal board evaluation
void neuralNet::optimalBoardEval() {
    std::vector<int> vectorone;
    std::vector<int> vectortwo;
    for(int i=0; i<6000; i++){
        vectorone.push_back(rand());
        vectortwo.push_back(rand());
    }
    int sum=0;
    auto start = std::chrono::system_clock::now();
    for(int i = 0; i<100000;i++) {
        for(int i=0; i<vectorone.size(); i++){
            sum+=vectorone[i]*vectortwo[i];
        }
    }
    auto end = std::chrono::system_clock::now();
    std::cout<<sum<<std::endl;
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout<<"Average neuralNet evaluation time: "<< std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()
             << " milliseconds\n";
}

double neuralNet::miniMaxEval(std::string b, char turn, int depth, double alpha, double beta) {
    if(depth==0 && turn == 'r') {
        miniM++;
        return neuralEvaluate(b);
    } else if(depth==0 && turn == 'b') {
        miniM++;
        return -1*neuralEvaluate(b);
    }
    std::vector<std::vector<neuralNet::theMove>> moveList;

    if(turn == 'r') {
        moveList = getRedMoves(b);
    } else if(turn == 'b') {
        moveList = getBlackMoves(b);
    }

    //maxPlayer is what denotes maxNode vs minNode
    if(turn == maxPlayer) {
        double bestVal = -100000;
        //maxNode for red
        if(turn == 'r') {
            boardCount++;
            evalCount+=moveList.size();
            for(std::vector<theMove> x : moveList) {
                double value = miniMaxEval(translateMove(x , b), 'b', depth-1, alpha, beta);
                bestVal = std::max(bestVal, value);
            }
            return bestVal;
            // maxNode for black
        } else if(turn == 'b') {
            boardCount++;
            evalCount+=moveList.size();
            for(std::vector<theMove> x : moveList) {
                double value = miniMaxEval(translateMove(x , b), 'r', depth-1, alpha, beta);
                bestVal = std::max(bestVal, value);
            }
            return bestVal;
        }

    } else  {
        double bestVal=100000;
        if(turn == 'r') {
            boardCount++;
            evalCount+=moveList.size();
            for(std::vector<theMove> x : moveList) {
                double value = miniMaxEval(translateMove(x , b), 'b', depth-1, alpha, beta);
                bestVal = std::min(bestVal, value);
            }
            return bestVal;
        } else if(turn == 'b') {
            boardCount++;
            evalCount+=moveList.size();
            for(std::vector<theMove> x : moveList) {
                double value = miniMaxEval(translateMove(x , b), 'r', depth-1, alpha, beta);
                bestVal = std::min(bestVal, value);
            }
            return bestVal;
        }

    }
}

double neuralNet::miniMaxEvalAB(std::string b, char turn, int depth, double alpha, double beta) {
    if(depth==0 && turn == 'r') {
        AB++;
        return neuralEvaluate(b);
    } else if(depth==0 && turn == 'b') {
        AB++;
        return -1*neuralEvaluate(b);
    }
    std::vector<std::vector<neuralNet::theMove>> moveList;

    if(turn == 'r') {
        moveList = getRedMoves(b);
    } else if(turn == 'b') {
        moveList = getBlackMoves(b);
    }

    //maxPlayer is what denotes maxNode vs minNode
    if(turn == maxPlayer) {
        double bestVal = -100000;
        //maxNode for red
        if(turn == 'r') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMaxEvalAB(translateMove(x , b), 'b', depth-1, alpha, beta);
                bestVal = std::max(bestVal, value);
                alpha = std::max(alpha, bestVal);
                if(beta <= alpha) {
                    break;
                }
            }
            return bestVal;
            // maxNode for black
        } else if(turn == 'b') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMaxEvalAB(translateMove(x , b), 'r', depth-1, alpha, beta);
                bestVal = std::max(bestVal, value);
                alpha = std::max(alpha, bestVal);
                if(beta <= alpha) {
                    break;
                }
            }
            return bestVal;
        }

    } else  {
        double bestVal=100000;
        if(turn == 'r') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMaxEvalAB(translateMove(x , b), 'b', depth-1, alpha, beta);
                bestVal = std::min(bestVal, value);
                beta = std::min(bestVal, beta);
                if (beta <= alpha) {
                    break;
                }
            }
            return bestVal;
        } else if(turn == 'b') {
            for(std::vector<theMove> x : moveList) {
                double value = miniMaxEvalAB(translateMove(x , b), 'r', depth-1, alpha, beta);
                bestVal = std::min(bestVal, value);
                beta = std::min(bestVal, beta);
                if(beta <= alpha) {
                    break;
                }
            }
            return bestVal;
        }

    }
}
//move table generation
//code below does move generation and stores it unordered_map
void neuralNet::generateRedMoveTable() {
    //GENERATOR_FOR_RED_MOVES
    theMove move;
    move.over=-1;
    std::vector<theMove> tile;

    //generate moves for RED_TILE_1
    tile.clear();
    move.from=1;
    move.to=5;
    tile.push_back(move);
    move.to=6;
    tile.push_back(move);
    redMoveTable.insert({1,tile});

    //generate moves for RED_TILE_2
    tile.clear();
    move.from=2;
    move.to=6;
    tile.push_back(move);
    move.to=7;
    tile.push_back(move);
    redMoveTable.insert({2,tile});

    //generate moves for RED_TILE_3
    tile.clear();
    move.from=3;
    move.to=7;
    tile.push_back(move);
    move.to=8;
    tile.push_back(move);
    redMoveTable.insert({3,tile});

    //generate moves for RED_TILE_4
    tile.clear();
    move.from=4;
    move.to=8;
    tile.push_back(move);
    redMoveTable.insert({4,tile});

    //generate moves for RED_TILE_5
    tile.clear();
    move.from=5;
    move.to=9;
    tile.push_back(move);
    redMoveTable.insert({5,tile});

    //generate moves for RED_TILE_6
    tile.clear();
    move.from=6;
    move.to=9;
    tile.push_back(move);
    move.to=10;
    tile.push_back(move);
    redMoveTable.insert({6,tile});

    //generate moves for RED_TILE_7
    tile.clear();
    move.from=7;
    move.to=10;
    tile.push_back(move);
    move.to=11;
    tile.push_back(move);
    redMoveTable.insert({7,tile});

    //generate moves for RED_TILE_8
    tile.clear();
    move.from=8;
    move.to=11;
    tile.push_back(move);
    move.to=12;
    tile.push_back(move);
    redMoveTable.insert({8,tile});

    //generate moves for RED_TILE_9
    tile.clear();
    move.from=9;
    move.to=13;
    tile.push_back(move);
    move.to=14;
    tile.push_back(move);
    redMoveTable.insert({9,tile});

    //generate moves for RED_TILE_10
    tile.clear();
    move.from=10;
    move.to=14;
    tile.push_back(move);
    move.to=15;
    tile.push_back(move);
    redMoveTable.insert({10,tile});

    //generate moves for RED_TILE_11
    tile.clear();
    move.from=11;
    move.to=15;
    tile.push_back(move);
    move.to=16;
    tile.push_back(move);
    redMoveTable.insert({11,tile});

    //generate moves for RED_TILE_12
    tile.clear();
    move.from=12;
    move.to=16;
    tile.push_back(move);
    redMoveTable.insert({12,tile});

    //generate moves for RED_TILE_13
    tile.clear();
    move.from=13;
    move.to=17;
    tile.push_back(move);
    redMoveTable.insert({13,tile});

    //generate moves for RED_TILE_14
    tile.clear();
    move.from=14;
    move.to=17;
    tile.push_back(move);
    move.to=18;
    tile.push_back(move);
    redMoveTable.insert({14,tile});

    //generate moves for RED_TILE_15
    tile.clear();
    move.from=15;
    move.to=18;
    tile.push_back(move);
    move.to=19;
    tile.push_back(move);
    redMoveTable.insert({15,tile});

    //generate moves for RED_TILE_16
    tile.clear();
    move.from=16;
    move.to=19;
    tile.push_back(move);
    move.to=20;
    tile.push_back(move);
    redMoveTable.insert({16,tile});

    //generate moves for RED_TILE_17
    tile.clear();
    move.from=17;
    move.to=21;
    tile.push_back(move);
    move.to=22;
    tile.push_back(move);
    redMoveTable.insert({17,tile});

    //generate moves for RED_TILE_18
    tile.clear();
    move.from=18;
    move.to=22;
    tile.push_back(move);
    move.to=23;
    tile.push_back(move);
    redMoveTable.insert({18,tile});

    //generate moves for RED_TILE_19
    tile.clear();
    move.from=19;
    move.to=23;
    tile.push_back(move);
    move.to=24;
    tile.push_back(move);
    redMoveTable.insert({19,tile});

    //generate moves for RED_TILE_20
    tile.clear();
    move.from=20;
    move.to=24;
    tile.push_back(move);
    redMoveTable.insert({20,tile});

    //generate moves for RED_TILE_21
    tile.clear();
    move.from=21;
    move.to=25;
    tile.push_back(move);
    redMoveTable.insert({21,tile});

    //generate moves for RED_TILE_22
    tile.clear();
    move.from=22;
    move.to=25;
    tile.push_back(move);
    move.to=26;
    tile.push_back(move);
    redMoveTable.insert({22,tile});

    //generate moves for RED_TILE_23
    tile.clear();
    move.from=23;
    move.to=26;
    tile.push_back(move);
    move.to=27;
    tile.push_back(move);
    redMoveTable.insert({23,tile});

    //generate moves for RED_TILE_24
    tile.clear();
    move.from=24;
    move.to=27;
    tile.push_back(move);
    move.to=28;
    tile.push_back(move);
    redMoveTable.insert({24,tile});

    //generate moves for RED_TILE_25
    tile.clear();
    move.from=25;
    move.to=29;
    tile.push_back(move);
    move.to=30;
    tile.push_back(move);
    redMoveTable.insert({25,tile});

    //generate moves for RED_TILE_26
    tile.clear();
    move.from=26;
    move.to=30;
    tile.push_back(move);
    move.to=31;
    tile.push_back(move);
    redMoveTable.insert({26,tile});

    //generate moves for RED_TILE_27
    tile.clear();
    move.from=27;
    move.to=31;
    tile.push_back(move);
    move.to=32;
    tile.push_back(move);
    redMoveTable.insert({27,tile});

    //generate moves for RED_TILE_28
    tile.clear();
    move.from=28;
    move.to=32;
    tile.push_back(move);
    redMoveTable.insert({28,tile});

}

void neuralNet::generateBlackMoveTable() {
    //GENERATOR_FOR_BLACK_MOVES
    theMove move;
    move.over = -1;
    std::vector<theMove> tile;

    //generate moves for BLACK_TILE_32
    tile.clear();
    move.from = 32;
    move.to = 27;
    tile.push_back(move);
    move.to = 28;
    tile.push_back(move);
    blackMoveTable.insert({32, tile});

    //generate moves for BLACK_TILE_31
    tile.clear();
    move.from = 31;
    move.to = 26;
    tile.push_back(move);
    move.to = 27;
    tile.push_back(move);
    blackMoveTable.insert({31, tile});

    //generate moves for BLACK_TILE_30
    tile.clear();
    move.from = 30;
    move.to = 25;
    tile.push_back(move);
    move.to = 26;
    tile.push_back(move);
    blackMoveTable.insert({30, tile});

    //generate moves for BLACK_TILE_29
    tile.clear();
    move.from = 29;
    move.to = 25;
    tile.push_back(move);
    blackMoveTable.insert({29, tile});

    //generate moves for BLACK_TILE_28
    tile.clear();
    move.from = 28;
    move.to = 24;
    tile.push_back(move);
    blackMoveTable.insert({28, tile});

    //generate moves for BLACK_TILE_27
    tile.clear();
    move.from = 27;
    move.to = 23;
    tile.push_back(move);
    move.to = 24;
    tile.push_back(move);
    blackMoveTable.insert({27, tile});

    //generate moves for BLACK_TILE_26
    tile.clear();
    move.from = 26;
    move.to = 22;
    tile.push_back(move);
    move.to = 23;
    tile.push_back(move);
    blackMoveTable.insert({26, tile});

    //generate moves for BLACK_TILE_25
    tile.clear();
    move.from = 25;
    move.to = 21;
    tile.push_back(move);
    move.to = 22;
    tile.push_back(move);
    blackMoveTable.insert({25, tile});

    //generate moves for BLACK_TILE_24
    tile.clear();
    move.from = 24;
    move.to = 19;
    tile.push_back(move);
    move.to = 20;
    tile.push_back(move);
    blackMoveTable.insert({24, tile});

    //generate moves for BLACK_TILE_23
    tile.clear();
    move.from = 23;
    move.to = 18;
    tile.push_back(move);
    move.to = 19;
    tile.push_back(move);
    blackMoveTable.insert({23, tile});

    //generate moves for BLACK_TILE_22
    tile.clear();
    move.from = 22;
    move.to = 17;
    tile.push_back(move);
    move.to = 18;
    tile.push_back(move);
    blackMoveTable.insert({22, tile});

    //generate moves for BLACK_TILE_21
    tile.clear();
    move.from = 21;
    move.to = 17;
    tile.push_back(move);
    blackMoveTable.insert({21, tile});

    //generate moves for BLACK_TILE_20
    tile.clear();
    move.from = 20;
    move.to = 16;
    tile.push_back(move);
    blackMoveTable.insert({20, tile});

    //generate moves for BLACK_TILE_19
    tile.clear();
    move.from = 19;
    move.to = 15;
    tile.push_back(move);
    move.to = 16;
    tile.push_back(move);
    blackMoveTable.insert({19, tile});

    //generate moves for BLACK_TILE_18
    tile.clear();
    move.from = 18;
    move.to = 14;
    tile.push_back(move);
    move.to = 15;
    tile.push_back(move);
    blackMoveTable.insert({18, tile});

    //generate moves for BLACK_TILE_17
    tile.clear();
    move.from = 17;
    move.to = 13;
    tile.push_back(move);
    move.to = 14;
    tile.push_back(move);
    blackMoveTable.insert({17, tile});

    //generate moves for BLACK_TILE_16
    tile.clear();
    move.from = 16;
    move.to = 11;
    tile.push_back(move);
    move.to = 12;
    tile.push_back(move);
    blackMoveTable.insert({16, tile});

    //generate moves for BLACK_TILE_15
    tile.clear();
    move.from = 15;
    move.to = 10;
    tile.push_back(move);
    move.to = 11;
    tile.push_back(move);
    blackMoveTable.insert({15, tile});

    //generate moves for BLACK_TILE_14
    tile.clear();
    move.from = 14;
    move.to = 9;
    tile.push_back(move);
    move.to = 10;
    tile.push_back(move);
    blackMoveTable.insert({14, tile});

    //generate moves for BLACK_TILE_13
    tile.clear();
    move.from = 13;
    move.to = 9;
    tile.push_back(move);
    blackMoveTable.insert({13, tile});

    //generate moves for BLACK_TILE_12
    tile.clear();
    move.from = 12;
    move.to = 8;
    tile.push_back(move);
    blackMoveTable.insert({12, tile});

    //generate moves for BLACK_TILE_11
    tile.clear();
    move.from = 11;
    move.to = 7;
    tile.push_back(move);
    move.to = 8;
    tile.push_back(move);
    blackMoveTable.insert({11, tile});

    //generate moves for BLACK_TILE_10
    tile.clear();
    move.from = 10;
    move.to = 6;
    tile.push_back(move);
    move.to = 7;
    tile.push_back(move);
    blackMoveTable.insert({10, tile});

    //generate moves for BLACK_TILE_9
    tile.clear();
    move.from = 9;
    move.to = 5;
    tile.push_back(move);
    move.to = 6;
    tile.push_back(move);
    blackMoveTable.insert({9, tile});

    //generate moves for BLACK_TILE_8
    tile.clear();
    move.from = 8;
    move.to = 3;
    tile.push_back(move);
    move.to = 4;
    tile.push_back(move);
    blackMoveTable.insert({8, tile});

    //generate moves for BLACK_TILE_7
    tile.clear();
    move.from = 7;
    move.to = 2;
    tile.push_back(move);
    move.to = 3;
    tile.push_back(move);
    blackMoveTable.insert({7, tile});

    //generate moves for BLACK_TILE_6
    tile.clear();
    move.from = 6;
    move.to = 1;
    tile.push_back(move);
    move.to = 2;
    tile.push_back(move);
    blackMoveTable.insert({6, tile});

    //generate moves for BLACK_TILE_5
    tile.clear();
    move.from = 5;
    move.to = 1;
    tile.push_back(move);
    blackMoveTable.insert({5, tile});
}

void neuralNet::generateRedJumpTable() {
    //GENERATOR_FOR_RED_JUMPS
    theMove jump;
    std::vector<theMove> tile;

    //red jump from TILE_1
    tile.clear();
    jump.from=1;
    jump.to=10;
    jump.over=6;
    tile.push_back(jump);
    redJumpTable.insert({1,tile});

    //red jump from TILE_2
    tile.clear();
    jump.from=2;
    jump.to=9;
    jump.over=6;
    tile.push_back(jump);
    jump.to=11;
    jump.over=7;
    tile.push_back(jump);
    redJumpTable.insert({2,tile});

    //red jump from TILE_3
    tile.clear();
    jump.from=3;
    jump.to=10;
    jump.over=7;
    tile.push_back(jump);
    jump.to=12;
    jump.over=8;
    tile.push_back(jump);
    redJumpTable.insert({3,tile});

    //red jump from TILE_4
    tile.clear();
    jump.from=4;
    jump.to=11;
    jump.over=8;
    tile.push_back(jump);
    redJumpTable.insert({4,tile});

    //red jump from TILE_5
    tile.clear();
    jump.from=5;
    jump.to=14;
    jump.over=9;
    tile.push_back(jump);
    redJumpTable.insert({5,tile});

    //red jump from TILE_6
    tile.clear();
    jump.from=6;
    jump.to=13;
    jump.over=9;
    tile.push_back(jump);
    jump.to=15;
    jump.over=10;
    tile.push_back(jump);
    redJumpTable.insert({6,tile});

    //red jump from TILE_7
    tile.clear();
    jump.from=7;
    jump.to=14;
    jump.over=10;
    tile.push_back(jump);
    jump.to=16;
    jump.over=11;
    tile.push_back(jump);
    redJumpTable.insert({7,tile});

    //red jump from TILE_8
    tile.clear();
    jump.from=8;
    jump.to=15;
    jump.over=11;
    tile.push_back(jump);
    redJumpTable.insert({8,tile});

    //red jump from TILE_9
    tile.clear();
    jump.from=9;
    jump.to=18;
    jump.over=14;
    tile.push_back(jump);
    redJumpTable.insert({9,tile});

    //red jump from TILE_10
    tile.clear();
    jump.from=10;
    jump.to=17;
    jump.over=14;
    tile.push_back(jump);
    jump.to=19;
    jump.over=15;
    tile.push_back(jump);
    redJumpTable.insert({10,tile});

    //red jump from TILE_11
    tile.clear();
    jump.from=11;
    jump.to=18;
    jump.over=15;
    tile.push_back(jump);
    jump.to=20;
    jump.over=16;
    tile.push_back(jump);
    redJumpTable.insert({11,tile});

    //red jump from TILE_12
    tile.clear();
    jump.from=12;
    jump.to=19;
    jump.over=16;
    tile.push_back(jump);
    redJumpTable.insert({12,tile});

    //red jump from TILE_13
    tile.clear();
    jump.from=13;
    jump.to=22;
    jump.over=17;
    tile.push_back(jump);
    redJumpTable.insert({13,tile});

    //red jump from TILE_14
    tile.clear();
    jump.from=14;
    jump.to=21;
    jump.over=17;
    tile.push_back(jump);
    jump.to=23;
    jump.over=18;
    tile.push_back(jump);
    redJumpTable.insert({14,tile});

    //red jump from TILE_15
    tile.clear();
    jump.from=15;
    jump.to=22;
    jump.over=18;
    tile.push_back(jump);
    jump.to=24;
    jump.over=19;
    tile.push_back(jump);
    redJumpTable.insert({15,tile});

    //red jump from TILE_16
    tile.clear();
    jump.from=16;
    jump.to=23;
    jump.over=19;
    tile.push_back(jump);
    redJumpTable.insert({16,tile});

    //red jump from TILE_17
    tile.clear();
    jump.from=17;
    jump.to=26;
    jump.over=22;
    tile.push_back(jump);
    redJumpTable.insert({17,tile});

    //red jump from TILE_18
    tile.clear();
    jump.from=18;
    jump.to=25;
    jump.over=22;
    tile.push_back(jump);
    jump.to=27;
    jump.over=23;
    tile.push_back(jump);
    redJumpTable.insert({18,tile});

    //red jump from TILE_19
    tile.clear();
    jump.from=19;
    jump.to=26;
    jump.over=23;
    tile.push_back(jump);
    jump.to=28;
    jump.over=24;
    tile.push_back(jump);
    redJumpTable.insert({19,tile});

    //red jump from TILE_20
    tile.clear();
    jump.from=20;
    jump.to=27;
    jump.over=24;
    tile.push_back(jump);
    redJumpTable.insert({20,tile});

    //red jump from TILE_21
    tile.clear();
    jump.from=21;
    jump.to=30;
    jump.over=25;
    tile.push_back(jump);
    redJumpTable.insert({21,tile});

    //red jump from TILE_22
    tile.clear();
    jump.from=22;
    jump.to=29;
    jump.over=25;
    tile.push_back(jump);
    jump.to=31;
    jump.over=26;
    tile.push_back(jump);
    redJumpTable.insert({22,tile});

    //red jump from TILE_23
    tile.clear();
    jump.from=23;
    jump.to=30;
    jump.over=26;
    tile.push_back(jump);
    jump.to=32;
    jump.over=27;
    tile.push_back(jump);
    redJumpTable.insert({23,tile});

    //red jump from TILE_24
    tile.clear();
    jump.from=24;
    jump.to=31;
    jump.over=27;
    tile.push_back(jump);
    redJumpTable.insert({24,tile});


}

void neuralNet::generateBlackJumpTable() {
    //GENERATOR_FOR_BLACK_JUMPS
    theMove jump;
    std::vector<theMove> tile;

    //black jump from TILE_32
    tile.clear();
    jump.from=32;
    jump.to=23;
    jump.over=27;
    tile.push_back(jump);
    blackJumpTable.insert({32,tile});

    //black jump from TILE_31
    tile.clear();
    jump.from=31;
    jump.to=22;
    jump.over=26;
    tile.push_back(jump);
    jump.to=24;
    jump.over=27;
    tile.push_back(jump);
    blackJumpTable.insert({31,tile});

    //black jump from TILE_30
    tile.clear();
    jump.from=30;
    jump.to=21;
    jump.over=25;
    tile.push_back(jump);
    jump.to=23;
    jump.over=26;
    tile.push_back(jump);
    blackJumpTable.insert({30,tile});

    //black jump from TILE_29
    tile.clear();
    jump.from=29;
    jump.to=22;
    jump.over=25;
    tile.push_back(jump);
    blackJumpTable.insert({29,tile});

    //black jump from TILE_28
    tile.clear();
    jump.from=28;
    jump.to=19;
    jump.over=24;
    tile.push_back(jump);
    blackJumpTable.insert({28,tile});

    //black jump from TILE_27
    tile.clear();
    jump.from=27;
    jump.to=18;
    jump.over=23;
    tile.push_back(jump);
    jump.to=20;
    jump.over=24;
    tile.push_back(jump);
    blackJumpTable.insert({27,tile});

    //black jump from TILE_26
    tile.clear();
    jump.from=26;
    jump.to=17;
    jump.over=22;
    tile.push_back(jump);
    jump.to=19;
    jump.over=23;
    tile.push_back(jump);
    blackJumpTable.insert({26,tile});

    //black jump from TILE_25
    tile.clear();
    jump.from=25;
    jump.to=18;
    jump.over=22;
    tile.push_back(jump);
    blackJumpTable.insert({25,tile});

    //red jump from TILE_24
    tile.clear();
    jump.from=24;
    jump.to=15;
    jump.over=19;
    tile.push_back(jump);
    blackJumpTable.insert({24,tile});

    //black jump from TILE_23
    tile.clear();
    jump.from=23;
    jump.to=14;
    jump.over=18;
    tile.push_back(jump);
    jump.to=16;
    jump.over=19;
    tile.push_back(jump);
    blackJumpTable.insert({23,tile});

    //black jump from TILE_22
    tile.clear();
    jump.from=22;
    jump.to=13;
    jump.over=17;
    tile.push_back(jump);
    jump.to=15;
    jump.over=18;
    tile.push_back(jump);
    blackJumpTable.insert({22,tile});

    //black jump from TILE_21
    tile.clear();
    jump.from=21;
    jump.to=14;
    jump.over=17;
    tile.push_back(jump);
    blackJumpTable.insert({21,tile});

    //black jump from TILE_20
    tile.clear();
    jump.from=20;
    jump.to=11;
    jump.over=16;
    tile.push_back(jump);
    blackJumpTable.insert({20,tile});

    //black jump from TILE_19
    tile.clear();
    jump.from=19;
    jump.to=10;
    jump.over=15;
    tile.push_back(jump);
    jump.to=12;
    jump.over=16;
    tile.push_back(jump);
    blackJumpTable.insert({19,tile});

    //black jump from TILE_18
    tile.clear();
    jump.from=18;
    jump.to=9;
    jump.over=14;
    tile.push_back(jump);
    jump.to=11;
    jump.over=15;
    tile.push_back(jump);
    blackJumpTable.insert({18,tile});

    //black jump from TILE_17
    tile.clear();
    jump.from=17;
    jump.to=10;
    jump.over=14;
    tile.push_back(jump);
    blackJumpTable.insert({17,tile});

    //black jump from TILE_16
    tile.clear();
    jump.from=16;
    jump.to=7;
    jump.over=11;
    tile.push_back(jump);
    blackJumpTable.insert({16,tile});

    //black jump from TILE_15
    tile.clear();
    jump.from=15;
    jump.to=6;
    jump.over=10;
    tile.push_back(jump);
    jump.to=8;
    jump.over=11;
    tile.push_back(jump);
    blackJumpTable.insert({15,tile});

    //black jump from TILE_14
    tile.clear();
    jump.from=14;
    jump.to=5;
    jump.over=9;
    tile.push_back(jump);
    jump.to=7;
    jump.over=10;
    tile.push_back(jump);
    blackJumpTable.insert({14,tile});

    //black jump from TILE_13
    tile.clear();
    jump.from=13;
    jump.to=6;
    jump.over=9;
    tile.push_back(jump);
    blackJumpTable.insert({13,tile});

    //black jump from TILE_12
    tile.clear();
    jump.from=12;
    jump.to=3;
    jump.over=8;
    tile.push_back(jump);
    blackJumpTable.insert({12,tile});

    //black jump from TILE_11
    tile.clear();
    jump.from=11;
    jump.to=2;
    jump.over=7;
    tile.push_back(jump);
    jump.to=4;
    jump.over=8;
    tile.push_back(jump);
    blackJumpTable.insert({11,tile});

    //black jump from TILE_10
    tile.clear();
    jump.from=10;
    jump.to=1;
    jump.over=6;
    tile.push_back(jump);
    jump.to=3;
    jump.over=7;
    tile.push_back(jump);
    blackJumpTable.insert({10,tile});

    //black jump from TILE_9
    tile.clear();
    jump.from=9;
    jump.to=2;
    jump.over=6;
    tile.push_back(jump);
    blackJumpTable.insert({9,tile});
}

void neuralNet::makeChecker(){
    for(int i=0; i < 8; i++) {
        for(int j=0; j < 8; j++) {
            checkerBoard[i][j]=false;
            checkKey[i][j]=0;
        }
    }
    for(int k=0; k<8; k=k+2) {
        for(int l=1; l < 8; l=l+2) {
            checkerBoard[k][l]=true;
            checkerBoard[l][k]=true;
        }
    }
//    for(int i=0; i < 8; i++) {
//        for(int j=0; j < 8; j++) {
//            std::cout<<checkerBoard[i][j]<< " ";
//        }
//        std::cout<<std::endl;
//    }
}
