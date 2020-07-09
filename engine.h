#ifndef ENGINE_H
#define ENGINE_H
#include "constants.h"

using namespace std;

#define square pair <int, int>
#define x first
#define y second

struct position;

struct step {
    square from;
    square to;
    int promote = 1;
    step ();
    step (pair <int, int> from_, pair <int, int> to_);
    step (pair <int, int> from_, pair <int, int> to_, int promote_);

    bool operator ==(step other);

    void output();

    bool check(position cur);
};

struct piece {
    bool side = 0; // 0 - white, 1 - black;
    int type = 0; // 0 - empty, 1 - pawn, 2 - knight, 3 - bishop, 4 - rook,
    // 5 - queen, 6 - king
    bool moved = 0;

    bool operator< (const piece other) const;

    bool operator==(const piece other) const;

    bool operator!=(const piece other) const;

    piece();

    piece(bool side_, int type_);

    void add_sweep_moves(vector <step> &ans, position &cur, int dx, int dy, int x, int y);

    vector <step> get_all_steps(position &cur, int x, int y);
};

struct position {
    step last_step;
    piece data[SZ][SZ];
    bool move;

    position();

    bool operator<(const position other) const;

    bool in_check(bool side);

    vector <step> find_moves();
    vector <step> find_all_moves();

    bool win();

    bool can_win(bool side);

    bool draw(map<position, int> &all_positions);

    bool end_check(map<position, int> &all_positions);

    void output();

    double eval(map<position, int> &all_positions);

    pair <step, double> search(int depth, double alpha, double beta, bool color, map<position, int> &all_positions);

    step choose_move(map<position, int> &all_positions);
};

position make_move(position cur, step step);

vector <step> remove_bad_moves(vector <step> all, position &cur);

#endif