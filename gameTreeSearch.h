#include <vector>
#include <string>
#include <math.h>
#include <stdexcept>
#ifndef GAME_TREE_SEARCH_H
#define GAME_TREE_SEARCH_H

bool isValid2DField(int x, int y, int size);
std::vector<int> findNeighbours(int field, int size);
bool isStartingField(int field, int size, bool isMaxPlayer);
bool isEndField(int field, int size, bool isMaxPlayer);
bool winnerPathBfs(int size, bool isMaxPlayer,std::string board);

// GameState class
class GameState {
public:
    std::string board;
    bool isMaxPlayer;
    int size;
        
    GameState() : board(""), isMaxPlayer(false), size(0) {}

    GameState(std::string board, bool isMaxPlayer) {
        this->board = board;
        this->isMaxPlayer = isMaxPlayer;
        this->size = sqrt(board.size());
    }
    
    int getLowestField(int column) {
        column -= size;
        int x = size-1, y = size-1;
        if(column <= 0) y += column;
        else x -= column;

        int lowest = x*size + y;

        while(board[lowest] != '0') {
            lowest -= (size+1);
            if(lowest < 0) throw std::out_of_range("No more space in column");
        }
        return lowest;
    }

    std::string getNextMove(int column) {
        if(column < 1 || column > size*2-1) throw std::out_of_range("Invalid column index");

        int lowest = getLowestField(column);
        std::string new_board = this->board;
        new_board[lowest] = isMaxPlayer ? '1' : '2';
        return new_board;
    }
};


#endif