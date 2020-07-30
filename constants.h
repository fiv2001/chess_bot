#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <bits/stdc++.h>

using namespace std;

const int inf = 1e9; /// maximum position evaluation
const int SZ = 8; // size of board
const double MOVE_COST = 0.05; // used in evaluation
const double STRATEGIC_MULTIPLIER = 0.2;

const int EMPTY = 0;
const int PAWN = 1;
const int KNIGHT = 2;
const int BISHOP = 3;
const int ROOK = 4;
const int QUEEN = 5;
const int KING = 6;

const int WHITE = 0;
const int BLACK = 1;

const int MIN_DEPTH = 2;

const int MAX_DEPTH = 4;

const int MAX_MOVES = 200; // maximum amount of moves if bot is playing against itself

const std::string ENGINE_NAME = "some_weird_bot";

#endif