#include "engine.h"
#include "tables.h"

bool in(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

step::step () {
    from.x = from.y = to.x = to.y = 0;
}
step::step (pair <int, int> from_, pair <int, int> to_) {
    from.x = from_.first;
    from.y = from_.second;
    to.x = to_.first;
    to.y = to_.second;
}
step::step (pair <int, int> from_, pair <int, int> to_, int promote_) {
    from.x = from_.first;
    from.y = from_.second;
    to.x = to_.first;
    to.y = to_.second;
    promote = promote_;
}

bool step::operator ==(step other) {
    return from == other.from && to == other.to && promote == other.promote;
}

void step::output() {
    cout << static_cast<char>(from.x + 'a');
    cout << from.y + 1;
    cout << static_cast<char>(to.x + 'a');
    cout << to.y + 1;
    if (promote != 1) {
        string promotes = "0pnbrqk";
        cout << promotes[promote];
    }
    cout << endl;
}

bool piece::operator< (const piece other) const {
        if (side == other.side) {
            return type < other.type;
        }
        return side < other.side;
    }

bool piece::operator==(const piece other) const {
    return !((*this) < other || other < (*this));
}

bool piece::operator!=(const piece other) const {
    return !((*this) == other);
}

piece::piece() {
    type = 0;
    side = 0;
}

piece::piece(bool side_, int type_) {
    type = type_;
    side = side_;
}

void piece::add_sweep_moves(vector <step> &ans, position &cur, int dx, int dy, int x, int y) {
    for (int x1 = x + dx, y1 = y + dy; in(x1, y1); x1 += dx, y1 += dy) {
        if (cur.data[x1][y1].type != EMPTY) {
            if (cur.data[x1][y1].side != side) {
                ans.push_back({{x, y}, {x1, y1}});
            }
            break;
        }
        ans.push_back({{x, y}, {x1, y1}});
    }
}

vector <step> piece::get_all_steps(position &cur, int x, int y) {
    if (type == EMPTY) {
        return {};
    }
    vector <step> ans;
    if (type == PAWN) {
        if (side == WHITE) {
            // one forward
            if (in(x, y + 1) && cur.data[x][y + 1].type == EMPTY) {
                // promotion
                if (y == SZ - 2) {
                    for (int i = KNIGHT; i <= QUEEN; i++) {
                        ans.push_back({{x, y}, {x, y + 1}, i});
                    }
                }
                else {
                    ans.push_back({{x, y}, {x, y + 1}});
                }
            }
            // takes right
            if (in(x + 1, y + 1) && cur.data[x + 1][y + 1].type != EMPTY &&
                cur.data[x + 1][y + 1].side != side) {
                    // promotion
                    if (y == SZ - 2) {
                        for (int i = KNIGHT; i <= QUEEN; i++) {
                            ans.push_back({{x, y}, {x + 1, y + 1}, i});
                        }
                    }
                    else {
                        ans.push_back({{x, y}, {x + 1, y + 1}});
                    }
            }
            // takes left
            if (in(x - 1, y + 1) && cur.data[x - 1][y + 1].type != EMPTY &&
                cur.data[x - 1][y + 1].side != side) {
                    //promotion
                    if (y == SZ - 2) {
                        for (int i = KNIGHT; i <= QUEEN; i++) {
                            ans.push_back({{x, y}, {x - 1, y + 1}, i});
                        }
                    }
                    else {
                        ans.push_back({{x, y}, {x - 1, y + 1}});
                    }
            }
            // two forward
            if (y == 1 && cur.data[x][y + 1].type == EMPTY &&
             cur.data[x][y + 2].type == EMPTY) {
                ans.push_back({{x, y}, {x, y + 2}});
            }
            // en pasant
            step right_en_pasant = {{x + 1, y + 2}, {x + 1, y}};
            step left_en_pasant = {{x - 1, y + 2}, {x - 1, y}};
            if (cur.last_step == right_en_pasant &&
                    cur.data[cur.last_step.to.x][cur.last_step.to.y].type == PAWN) {
                ans.push_back({{x, y}, {x + 1, y + 1}});
            }
            if (cur.last_step == left_en_pasant &&
                    cur.data[cur.last_step.to.x][cur.last_step.to.y].type == PAWN) {
                ans.push_back({{x, y}, {x - 1, y + 1}});
            }
        }
        else {
            // one forward
            if (in(x, y - 1) && cur.data[x][y - 1].type == EMPTY) {
                // promotion
                if (y == 1) {
                    for (int i = 2; i <= 5; i++) {
                        ans.push_back({{x, y}, {x, y - 1}, i});
                    }
                }
                else {
                    ans.push_back({{x, y}, {x, y - 1}});
                }
            }
            // takes right
            if (in(x + 1, y - 1) && cur.data[x + 1][y - 1].type != EMPTY &&
                cur.data[x + 1][y - 1].side != side) {
                // promotion
                if (y == 1) {
                    for (int i = KNIGHT; i <= QUEEN; i++) {
                        ans.push_back({{x, y}, {x + 1, y - 1}, i});
                    }
                }
                else {
                    ans.push_back({{x, y}, {x + 1, y - 1}});
                }
            }
            // takes left
            if (in(x - 1, y - 1) && cur.data[x - 1][y - 1].type != EMPTY &&
                cur.data[x - 1][y - 1].side != side) {
                // promotion
                if (y == 1) {
                    for (int i = KNIGHT; i <= QUEEN; i++) {
                        ans.push_back({{x, y}, {x - 1, y - 1}, i});
                    }
                }
                else {
                    ans.push_back({{x, y}, {x - 1, y - 1}});
                }
            }
            // two forward
            if (y == 6 && cur.data[x][y - 1].type == EMPTY &&
             cur.data[x][y - 2].type == EMPTY) {
                ans.push_back({{x, y}, {x, y - 2}});
            }
            // en pasant
            step right_en_pasant = {{x + 1, y - 2}, {x + 1, y}};
            step left_en_pasant = {{x - 1, y - 2}, {x - 1, y}};
            if (cur.last_step == right_en_pasant) {
                ans.push_back({{x, y}, {x + 1, y - 1}});
            }
            if (cur.last_step == left_en_pasant) {
                ans.push_back({{x, y}, {x - 1, y - 1}});
            }
        }
    }

    if (type == KNIGHT) {
        // cout << "KNIGHT" << endl;
        vector <pair <int, int>> moves = {{1, 2}, {-1, 2}, {1, -2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
        for (auto p : moves) {
            int dx = p.first, dy = p.second;
            int nx = dx + x, ny = dy + y;
            if (in(nx, ny) && !(cur.data[nx][ny].type != EMPTY && cur.data[nx][ny].side == side) ) {
                ans.push_back({{x, y}, {nx, ny}});
            }
        }
    }

    if (type == KING) {
        vector <pair <int, int>> moves = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {0, 1},
            {-1, -1}, {-1, 0}, {-1, 1}};
        for (auto p : moves) {
            int dx = p.first, dy = p.second;
            int nx = dx + x, ny = dy + y;
            if (in(nx, ny) && !(cur.data[nx][ny].type != EMPTY && cur.data[nx][ny].side == side) ) {
                ans.push_back({{x, y}, {nx, ny}});
            }
        }
        // short castling
        if (cur.data[x + 1][y].type == EMPTY && cur.data[x + 2][y].type == EMPTY
            && cur.data[x][y].moved == false && cur.data[x + 3][y].type == ROOK &&
                cur.data[x + 3][y].moved == false) {
            // cout << "TRYING TO CASTLE" << endl;
            position cur1 = cur;
            cur1.data[x + 1][y].type = KING;
            cur1.data[x + 1][y].side = side;
            cur1.data[x + 2][y].type = KING;
            cur1.data[x + 2][y].side = side;
            if (!cur1.in_check(side)) {
                ans.push_back({{x, y}, {x + 2, y}});
            }
        }
        // long castling
        if (cur.data[x - 1][y].type == EMPTY && cur.data[x - 2][y].type == EMPTY
            && cur.data[x - 3][y].type == EMPTY && cur.data[x][y].moved == false && cur.data[x - 4][y].type == ROOK &&
                cur.data[x - 4][y].moved == false) {
            position cur1 = cur;
            cur1.data[x - 1][y].type = KING;
            cur1.data[x - 1][y].side = side;
            cur1.data[x - 2][y].type = KING;
            cur1.data[x - 2][y].side = side;
            if (!cur1.in_check(side)) {
                ans.push_back({{x, y}, {x - 2, y}});
            }
        }
    }

    if (type == BISHOP) {
        add_sweep_moves(ans, cur, 1, 1, x, y);
        add_sweep_moves(ans, cur, 1, -1, x, y);
        add_sweep_moves(ans, cur, -1, 1, x, y);
        add_sweep_moves(ans, cur, -1, -1, x, y);
    }

    if (type == ROOK) {
        add_sweep_moves(ans, cur, 1, 0, x, y);
        add_sweep_moves(ans, cur, -1, 0, x, y);
        add_sweep_moves(ans, cur, 0, 1, x, y);
        add_sweep_moves(ans, cur, 0, -1, x, y);
    }

    if (type == QUEEN) {
        // rook moves
        add_sweep_moves(ans, cur, 1, 0, x, y);
        add_sweep_moves(ans, cur, -1, 0, x, y);
        add_sweep_moves(ans, cur, 0, 1, x, y);
        add_sweep_moves(ans, cur, 0, -1, x, y);
        // bishop moves
        add_sweep_moves(ans, cur, 1, 1, x, y);
        add_sweep_moves(ans, cur, 1, -1, x, y);
        add_sweep_moves(ans, cur, -1, 1, x, y);
        add_sweep_moves(ans, cur, -1, -1, x, y);
    }
    return ans;
}

bool position::operator<(const position other) const {
    for (int i = 0; i < SZ; i++) {
        for (int j = 0; j < SZ; j++) {
            if (data[i][j] != other.data[i][j]) {
                return data[i][j] < other.data[i][j];
            }
        }
    }
    return move < other.move;
}

bool position::in_check(bool side) {
    for (int i = 0; i < SZ; i++) {
        for (int j = 0; j < SZ; j++) {
            if (data[i][j].type != EMPTY && data[i][j].side != side) {
                vector <step> all_steps = data[i][j].get_all_steps(*this, i, j);
                for (auto step : all_steps) {
                    if (data[step.to.x][step.to.y].type == KING &&
                        data[step.to.x][step.to.y].side == side) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

bool position::win() {
    bool ch = in_check(move);
    if (!ch) {
        return 0;
    }
    vector <step> moves = find_moves();
    if (moves.empty()) {
        return 1;
    }
    return 0;
}

bool position::can_win(bool side) {
    int found_bishops = 0, found_knights = 0;
    for (int i = 0; i < SZ; i++) {
        for (int j = 0; j < SZ; j++) {
            if (data[i][j].side == side) {
                if (data[i][j].type == KING) {
                    continue;
                }
                if (data[i][j].type == PAWN || data[i][j].type ) {
                    return 1;
                }
                if (data[i][j].type == KNIGHT) {
                    found_knights++;
                }
                if (data[i][j].type == BISHOP) {
                    found_bishops++;
                }
                if (data[i][j].type > BISHOP) {
                    return 1;
                }
            }
        }
    }
    return (found_bishops + found_knights >= 2);
}

bool position::draw(map<position, int> &all_positions) { /// only stalemate
    if (all_positions[*this] >= 3) {
        cout << "CASE 1" << endl;
        return 1;
    }
    if (!can_win(WHITE) && !can_win(BLACK)) {
        cout << "CASE 2" << endl;
        return 1;
    }
    vector <step> moves = find_moves();
    if (!win() && moves.empty()) {
        cout << "CASE 3" << endl;
        return 1;
    }
    return 0;
}

bool position::end_check(map<position, int> &all_positions) {
    if (win()) {
        if (move == WHITE) {
            cout << "BLACK WINS" << endl;
        }
        else {
            cout << "WHITE WINS" << endl;
        }
        return 1;
    }
    if (draw(all_positions)) {
        cout << "DRAW" << endl;
        return 1;
    }
    return 0;
}

position make_move(position cur, step step) {
    position ans = cur;
    // en pasant
    if (ans.data[step.from.x][step.from.y].type == PAWN &&
        (abs(step.from.x - step.to.x) == 1 && abs(step.from.y - step.to.y) == 1)
            && ans.data[step.to.x][step.to.y].type == EMPTY) {

        if (ans.data[step.from.x][step.from.y].side == WHITE) {
            ans.data[step.to.x][step.to.y - 1].type = EMPTY;
        }
        else {
            ans.data[step.to.x][step.to.y + 1].type = EMPTY;
        }
    }
    //promotion
    if (step.promote != 1) {
        ans.data[step.from.x][step.from.y].type = step.promote;
    }

    ans.last_step = step;
    ans.move ^= 1;

    // short castling
    if (ans.data[step.from.x][step.from.y].type == KING && step.to.x == step.from.x + 2) {
        // cout << "SHORT CASTLING" << endl;
        ans.data[step.from.x + 1][step.from.y] = ans.data[step.to.x + 1][step.from.y];
        ans.data[step.to.x][step.from.y] = ans.data[step.from.x][step.from.y];
        ans.data[step.from.x][step.from.y].type = EMPTY;
        ans.data[step.to.x + 1][step.from.y].type = EMPTY;
        return ans;
    }

    // long castling
    if (ans.data[step.from.x][step.from.y].type == KING && step.to.x == step.from.x - 2) {
        ans.data[step.from.x - 1][step.from.y] = ans.data[step.to.x - 2][step.from.y];
        ans.data[step.from.x - 2][step.from.y] = ans.data[step.from.x][step.from.y];
        ans.data[step.from.x][step.from.y].type = EMPTY;
        ans.data[step.to.x - 2][step.from.y].type = EMPTY;
        return ans;
    }

    ans.data[step.to.x][step.to.y] =
        ans.data[step.from.x][step.from.y];
    ans.data[step.from.x][step.from.y].type = EMPTY;
    ans.data[step.to.x][step.to.y].moved = true;
    return ans;
}

vector <step> remove_bad_moves(vector <step> all, position &cur) {
    vector <step> ans;
    for (int i = 0; i < all.size(); i++) {
        position nw = make_move(cur, all[i]);
        if (!nw.in_check(cur.move)) {
            ans.push_back(all[i]);
        }
    }
    return ans;
}

vector <step> position::find_moves() {
    vector <step> all;
    for (int i = 0; i < SZ; i++) {
        for (int j = 0; j < SZ; j++) {
            // cout << cur.data[i][j].type << endl;
            if (data[i][j].type != EMPTY && data[i][j].side == move) {
                // cout << i << " " << j << " " << cur.data[i][j].type << endl;
                vector <step> steps = data[i][j].get_all_steps(*this, i, j);
                // cout << "DEBUG " << steps.size() << endl;
                steps = remove_bad_moves(steps, *this);
                for (auto step : steps) {
                    all.push_back(step);
                }
            }
            // cout << endl;
        }
    }
    return all;
}

vector <step> position::find_all_moves() {
    vector <step> all;
    for (int i = 0; i < SZ; i++) {
        for (int j = 0; j < SZ; j++) {
            // cout << cur.data[i][j].type << endl;
            if (data[i][j].type != EMPTY && data[i][j].side == move) {
                // cout << i << " " << j << " " << cur.data[i][j].type << endl;
                vector <step> steps = data[i][j].get_all_steps(*this, i, j);
                // steps = remove_bad_moves(steps, *this);
                for (auto step : steps) {
                    all.push_back(step);
                }
            }
            // cout << endl;
        }
    }
    return all;
}

double position::eval(map<position, int> &all_positions) {
    double ans = 0;
    if (win()) {
        return -inf;
    }
    if (draw(all_positions)) {
        // cout << "DRARARAW" << endl;
        return 0;
    }
    if (all_positions[*this] >= 2) {
        return 0;
    }
    double strategic_val = 0;
    for (int i = 0; i < SZ; i++) {
        for (int j = 0; j < SZ; j++) {
            if (data[i][j].type == EMPTY) {
                continue;
            }
            if (data[i][j].side == WHITE) {
                strategic_val += strategic_tables.data[data[i][j].type][j][i];
            }
            else {
                strategic_val -= strategic_tables.data[data[i][j].type][SZ - j - 1][i]; 
            }
        }
    }
    strategic_val *= STRATEGIC_MULTIPLIER;
    ans += strategic_val;
    vector <int> cost = {0, 1, 3, 3, 5, 9, 0};
    for (int i = 0; i < SZ; i++) {
        for (int j = 0; j < SZ; j++) {
            if (data[i][j].side == WHITE) {
                ans += cost[data[i][j].type];
            }
            else {
                ans -= cost[data[i][j].type];
            }
        }
    }
    vector <step> moves = find_all_moves();
    position other = *this;
    other.move ^= 1;
    vector <step> moves_1 = other.find_all_moves();
    double dans = (double)moves.size() * MOVE_COST - (double)moves_1.size() * MOVE_COST;
    if (move == BLACK) {
        dans = -dans;
    }
    ans += dans;
    // cout << ((int)moves.size() - (int)moves_1.size()) << " " << ans << " DEBUG EVAL" << endl;
    return ans;
}

pair <step, double> position::search(int depth, double alpha, double beta, bool color, 
    map<position, int> &all_positions) { // returns move and evaluation
    step best;
    if (draw(all_positions)) {
        return {best, 0};
    }
    if (win()) {
        if (color == WHITE) {
            return {best, -inf};
        }
        else {
            return {best, inf};
        }
    }
    // cout << depth << " DEPTH" << endl;
    if (depth == MAX_DEPTH) {
        return {best, eval(all_positions)};
    }
    vector <step> all = find_moves();
    double val;
    vector <vector <step> > M(8);
    for (auto move : all) {
        if (move.check(*this)) {
            M[7].push_back(move);
            continue;
        }
        M[data[move.to.x][move.to.y].type].push_back(move);
    }
    best = all[0];
    // cout << "DEPTH " << depth << " " << all.size() << " " << M[6].size() << " " << M[7].size() << endl;
    vector <int> I;
    all.clear();
    for (int i = M.size() - 1; i >= 0; i--) {
        for (auto move : M[i]) {
            I.push_back(i);
            all.push_back(move);
        }
    }
    if (color == WHITE) {
        val = -inf;
        for (int i = 0; i < all.size(); i++) {
            auto move = all[i];
            if (I[i] == 0 && depth >= MIN_DEPTH && depth % 2 == MIN_DEPTH % 2) {
                continue;
            }
            position nw = make_move(*this, move);
            all_positions[nw]++;
            pair <step, double> best1 = nw.search(depth + 1, alpha, beta, color ^ 1, all_positions);
            all_positions[nw]--;
            // cout << best1.second << " ";
            // move.output();
            if (best1.second == inf) {
                best1.second -= MOVE_COST;
            }
            if (best1.second == -inf) {
                best1.second += MOVE_COST;
            }
            if (val < best1.second) {
                val = best1.second;
                best = move;
            }
            alpha = max(alpha, best1.second);
            if (alpha >= beta) {
                break;
            }
        }
        if (depth >= MIN_DEPTH) {
            val = max(val, eval(all_positions));
        }
    }
    else {
        val = inf;
        for (int i = 0; i < all.size(); i++) {
            auto move = all[i];
            if (I[i] == 0 && depth >= MIN_DEPTH && depth % 2 == MIN_DEPTH % 2) {
                continue;
            }
            position nw = make_move(*this, move);
            all_positions[nw]++;
            pair <step, double> best1 = nw.search(depth + 1, alpha, beta, color ^ 1, all_positions);
            all_positions[nw]--;
            if (best1.second == inf) {
                val -= MOVE_COST;
            }
            if (best1.second == -inf) {
                val += MOVE_COST;
            }
            if (val > best1.second) {
                val = best1.second;
                best = move;
            }
            beta = min(beta, best1.second);
            if (alpha >= beta) {
                break;
            }
        }
        if (depth >= MIN_DEPTH) {
            val = min(val, eval(all_positions));
        }
    }
    return {best, val};
}

step position::choose_move(map<position, int> &all_positions) {
    pair <step, double> best = search(0, -inf, inf, move, all_positions);
    return best.first;
}

bool step::check(position cur) {
    cur = make_move(cur, *this);
    return cur.in_check(cur.move);
}

position::position() {
    move = WHITE;
    for (int i = 0; i < SZ; i++) {
        data[i][1] = {WHITE, PAWN};
    }
    for (int i = 0; i < SZ; i++) {
        data[i][6] = {BLACK, PAWN};
    }
    data[0][0] = {WHITE, ROOK};
    data[1][0] = {WHITE, KNIGHT};
    data[2][0] = {WHITE, BISHOP};
    data[3][0] = {WHITE, QUEEN};
    data[4][0] = {WHITE, KING};
    data[5][0] = {WHITE, BISHOP};
    data[6][0] = {WHITE, KNIGHT};
    data[7][0] = {WHITE, ROOK};

    data[0][7] = {BLACK, ROOK};
    data[1][7] = {BLACK, KNIGHT};
    data[2][7] = {BLACK, BISHOP};
    data[3][7] = {BLACK, QUEEN};
    data[4][7] = {BLACK, KING};
    data[5][7] = {BLACK, BISHOP};
    data[6][7] = {BLACK, KNIGHT};
    data[7][7] = {BLACK, ROOK};
}