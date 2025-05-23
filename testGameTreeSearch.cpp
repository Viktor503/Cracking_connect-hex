#include <assert.h>
#include "gameTreeSearch.h"
#include <iostream>

// Test cases for the functions in whatever.cpp
void test_isValid2DField() {
    assert(isValid2DField(0, 0, 5) == true);
    assert(isValid2DField(4, 4, 3) == false);
    assert(isValid2DField(-1, 0, 5) == false);
    assert(isValid2DField(0, -1, 2) == false);
    assert(isValid2DField(5, 5, 5) == false);
}


void test_findNeighbours() {
    std::vector<int> expectedNeighbours = {1, 5, 6};
    assert(findNeighbours(0, 5) == expectedNeighbours);

    expectedNeighbours = {0, 2, 6, 7};
    assert(findNeighbours(1, 5) == expectedNeighbours);

    expectedNeighbours = {0, 1, 4, 6, 9, 10};
    assert(findNeighbours(5, 4) == expectedNeighbours);

    expectedNeighbours = {4,11};
    assert(findNeighbours(5, 6) == expectedNeighbours);

    expectedNeighbours = {0, 1, 6, 8, 13, 14};
    assert(findNeighbours(7, 6) == expectedNeighbours);

    expectedNeighbours = {10, 11, 16, 23};
    assert(findNeighbours(17, 6) == expectedNeighbours);
 
    expectedNeighbours = {};
    assert(findNeighbours(-1, 6) == expectedNeighbours);
}

void test_isStartingField() {
    assert(isStartingField(0, 5, true) == true);
    assert(isStartingField(1, 5, true) == true);
    assert(isStartingField(4, 5, false) == false);
    assert(isStartingField(7, 5, false) == false);

    assert(isStartingField(0, 1, true) == true);
    assert(isStartingField(0, 1, false) == true);
    assert(isStartingField(1, 1, true) == false);

    assert(isStartingField(0, 10, true) == true);
    assert(isStartingField(9, 10, false) == false);
    assert(isStartingField(30, 10, true) == false);

    assert(isStartingField(-1, 5, true) == false);
    assert(isStartingField(0, 0, true) == false);
    assert(isStartingField(0, -5, true) == false);
}

void test_isEndField() {
    assert(isEndField(24, 5, true) == true);
    assert(isEndField(20, 5, true) == true);
    assert(isEndField(19, 5, true) == false);
    assert(isEndField(0, 5, true) == false);
    assert(isEndField(8, 3, true) == true);
    assert(isEndField(6, 3, true) == true);

    assert(isEndField(4, 5, false) == true);
    assert(isEndField(9, 5, false) == true);
    assert(isEndField(3, 5, false) == false);
    assert(isEndField(0, 5, false) == false);
    assert(isEndField(2, 3, false) == true);
    assert(isEndField(0, 3, false) == false);

    assert(isEndField(-1, 5, true) == false);
    assert(isEndField(25, 5, true) == false);
    assert(isEndField(24, 0, true) == false);
    assert(isEndField(24, -5, true) == false);
}

void test_winnerPathBfs() {
    assert(winnerPathBfs(5, true, "111112222200000000000000") == false);
    assert(winnerPathBfs(5, true, "1222212000100001000010000") == true);

    assert(winnerPathBfs(5, false, "2222211111000000000000000") == true);
    assert(winnerPathBfs(5, false, "1111122220000000200000000") == false);

    assert(winnerPathBfs(4, true, "0012021100120021") == true);
    assert(winnerPathBfs(4, true, "1000210012121212") == true);

    assert(winnerPathBfs(4, false, "0012021100120021") == false);
    assert(winnerPathBfs(4, false, "1000210012121212") == false);

    assert(winnerPathBfs(0, true, "") == false);
    assert(winnerPathBfs(0, false, "") == false);

    assert(winnerPathBfs(1, true, "1") == true);
    assert(winnerPathBfs(1, false, "2") == true);

}


int main() {
    test_isValid2DField();
    test_findNeighbours();
    test_isStartingField();
    test_isEndField();
    test_winnerPathBfs();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}

