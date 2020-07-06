#include <bits/stdc++.h>

using namespace std;

mt19937 gen(chrono::steady_clock::now().time_since_epoch().count());

const int inf = 1e9; /// maximum position evaluation
const int SZ = 8; // size of board
const double MOVE_COST = 0.02; // used in evaluation

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

const int MAX_DEPTH = 6;

const int MAX_MOVES = 200; /// maximum amount of moves if bot is playing against itself

struct square {
  int x;
  int y;
  bool operator==(square other) {
    return x == other.x && y == other.y;
  }
};

struct step {
  square from;
  square to;
  int promote = 1;
  step () {
    from.x = from.y = to.x = to.y = 0;
  }
  step (pair <int, int> from_, pair <int, int> to_) {
    from.x = from_.first;
    from.y = from_.second;
    to.x = to_.first;
    to.y = to_.second;
  }
  step (pair <int, int> from_, pair <int, int> to_, int promote_) {
    from.x = from_.first;
    from.y = from_.second;
    to.x = to_.first;
    to.y = to_.second;
    promote = promote_;
  }

  bool operator ==(step other) {
    return from == other.from && to == other.to && promote == other.promote;
  }

  void output() {
    cout << static_cast<char>(from.x + 'A');
    cout << from.y + 1;
    cout << static_cast<char>(to.x + 'A');
    cout << to.y + 1 << endl;
  }
};

char to_lower(char c) {
  if (c >= 'A' && c <= 'Z') {
    return c - 'A' + 'a';
  }
  return c;
}

bool is_digit(char c) {
  return c >= '0' && c <= '9';
}

bool is_letter(char c) {
  c = to_lower(c);
  return c >= 'a' && c <= 'z';
}

bool ok_input(string s) {
  if (s.length() < 4 || s.length() > 5) {
    return 0;
  }
  if (s.length() == 5) {
    char prom = to_lower(s[4]);
    if (prom != 'q' && prom != 'r' && prom != 'b' && prom != 'k') {
      return 0;
    }
  }
  if (is_letter(s[0]) && is_letter(s[2]) && is_digit(s[1]) && is_digit(s[3])) {
    if (to_lower(s[0]) <= 'h' && to_lower(s[2]) <= 'h' && s[1] <= '8' && 
      s[1] >= '1' && s[3] <= '8' && s[3] >= '1') {
      
      return 1;
    }
  }
  return 0;
}

step input() {
  string s;
  if (s == "resign") {
    cout << "WELL PLAYED" << endl;
    exit(0);
  }
  while (cin >> s) {
    if (ok_input(s)) {
      break;
    }
    else {
      cout << "WRONG FORMAT. TRY AGAIN" << endl;
    }
  }
  step ans;
  ans.from.x = to_lower(s[0]) - 'a';
  ans.from.y = to_lower(s[1]) - '1';
  ans.to.x = to_lower(s[2]) - 'a';
  ans.to.y = to_lower(s[3]) - '1';
  if (s.length() == 5) {
    char prom = to_lower(s[4]);
    if (prom == 'q') {
      ans.promote = QUEEN;
    }
    if (prom == 'r') {
      ans.promote = ROOK;
    }
    if (prom == 'b') {
      ans.promote = BISHOP;
    }
    if (prom == 'k') {
      ans.promote = KNIGHT;
    }
  }
  return ans;
}

// void output(step last) {
//   cout << static_cast<char>(last.from.x + 'A');
//   cout << last.from.y + 1;
//   cout << static_cast<char>(last.to.x + 'A');
//   cout << last.to.y + 1 << endl;
// }

bool in(int x, int y) {
  return x >= 0 && x < 8 && y >= 0 && y < 8; 
}

struct position;

map <position, int> all_positions;

struct piece {
  bool side = 0; // 0 - white, 1 - black;
  int type = 0; // 0 - empty, 1 - pawn, 2 - knight, 3 - bishop, 4 - rook,
  // 5 - queen, 6 - king
  bool moved = 0;

  bool operator< (const piece other) const {
    if (side == other.side) {
      return type < other.type;
    }
    return side < other.side;
  }

  bool operator==(const piece other) const {
    return !((*this) < other || other < (*this));
  }

  bool operator!=(const piece other) const {
    return !((*this) == other);
  }
  
  piece() {
    type = 0;
    side = 0;
  }
  
  piece(bool side_, int type_) {
    type = type_;
    side = side_;
  }
  
  void add_sweep_moves(vector <step> &ans, position &cur, int dx, int dy, int x, int y);
  
  vector <step> get_all_steps(position &cur, int x, int y);
};

struct position {
  step last_step;
  piece data[SZ][SZ];
  bool move;

  bool operator<(const position other) const {
    for (int i = 0; i < SZ; i++) {
      for (int j = 0; j < SZ; j++) {
        if (data[i][j] != other.data[i][j]) {
          return data[i][j] < other.data[i][j];
        }
      }
    }
    return move < other.move;
  }

  bool in_check(bool side) {
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

  vector <step> find_moves();
  vector <step> find_all_moves();

  bool win() {
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

  bool draw() { /// only stalemate
    vector <step> moves = find_moves();
    if (!win() && moves.empty()) {
      return 1;
    }
    if (all_positions[*this] >= 3) {
      return 1;
    }
    return 0;
  }

  bool end_check() {
    if (win()) {
      if (move == WHITE) {
        cout << "BLACK WINS" << endl;
      }
      else {
        cout << "WHITE WINS" << endl;
      }
      return 1;
    }
    if (draw()) {
      cout << "DRAW" << endl;
      return 1;
    }
    return 0;
  }

  void output() {
    for (int i = SZ - 1; i >= 0; i--) {
      for (int j = 0; j < SZ; j++) {
        if (data[j][i].type == EMPTY) {
          cout << ".";
          continue;
        }
        char x;
        if (data[j][i].type == PAWN) {
          x = 'P';
        }
        if (data[j][i].type == KNIGHT) {
          x = 'N';
        }
        if (data[j][i].type == BISHOP) {
          x = 'B';
        }
        if (data[j][i].type == ROOK) {
          x = 'R';
        }
        if (data[j][i].type == QUEEN) {
          x = 'Q';
        }
        if (data[j][i].type == KING) {
          x = 'K';
        }
        if (data[j][i].type == BLACK) {
          x = to_lower(x);
        }
        cout << x;
      }
      cout << endl;
    }
  }

  double eval() {
    double ans = 0;
    if (win()) {
      return -inf;
    }
    if (draw()) {
      // cout << "DRARARAW" << endl;
      return 0;
    }
    if (all_positions[*this] >= 2) {
      return 0;
    }
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

  pair <step, double> search(int depth, double alpha, double beta, bool color);

  step choose_move() {
    pair <step, double> best = search(0, -inf, inf, move);
    return best.first;
  }

  bool cmp(const step a, const step b) const {
    return data[a.to.x][a.to.y].type > data[b.to.x][b.to.y].type;
  }
};


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
  if (ans.data[step.from.x][step.from.y].type == KING && step.to.x == step.from.x + 3) {
    ans.data[step.from.x + 1][step.from.y] = ans.data[step.to.x][step.from.y];
    ans.data[step.from.x + 2][step.from.y] = ans.data[step.from.x][step.from.y];
    ans.data[step.from.x][step.from.y].type = EMPTY;
    ans.data[step.to.x][step.from.y].type = EMPTY;
    return ans;
  }

  // long castling
  if (ans.data[step.from.x][step.from.y].type == KING && step.to.x == step.from.x - 4) {
    ans.data[step.from.x - 1][step.from.y] = ans.data[step.to.x][step.from.y];
    ans.data[step.from.x - 2][step.from.y] = ans.data[step.from.x][step.from.y];
    ans.data[step.from.x][step.from.y].type = EMPTY;
    ans.data[step.to.x][step.from.y].type = EMPTY;
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

pair <step, double> position::search(int depth, double alpha, double beta, bool color) { // returns move and evaluation
  step best;
  if (draw()) {
    return {best, 0};
  }
  if (win()) {
    return {best, -inf};
  }
  // cout << depth << " DEPTH" << endl;
  if (depth == MAX_DEPTH) {
    return {best, eval()};
  }
  vector <step> all = find_moves();
  double val;
  vector <step> M[7];
  for (auto move : all) {
    M[data[move.to.x][move.to.y].type].push_back(move);
  }
  all.clear();
  for (int i = 0; i < 7; i++) {
    for (auto move : M[i]) {
      all.push_back(move);
    }
  }
  reverse(all.begin(), all.end());
  if (color == WHITE) {
    val = -inf;
    for (auto move : all) {
      if (data[move.to.x][move.to.y].type == EMPTY && depth >= MIN_DEPTH) {
        continue;
      }
      position nw = make_move(*this, move);
      all_positions[nw]++;
      pair <step, double> best1 = nw.search(depth + 1, alpha, beta, color ^ 1);
      all_positions[nw]--;
      // cout << best1.second << " ";
      // move.output();
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
      val = max(val, eval());
    }
  }
  else {
    val = inf;
    for (auto move : all) {
      if (data[move.to.x][move.to.y].type == EMPTY && depth >= MIN_DEPTH) {
        continue;
      }
      position nw = make_move(*this, move);
      all_positions[nw]++;
      pair <step, double> best1 = nw.search(depth + 1, alpha, beta, color ^ 1);
      all_positions[nw]--;
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
      val = min(val, eval());
    }
  }
  return {best, val};
}

// step position::choose_move() {
//   vector <step> all = find_moves();
//   double mx = -inf;
//   step ans;
//   for (auto move : all) {
//     double mx1 = -inf;
//     position nw = make_move(*this, move);
//     vector <step> all1 = nw.find_moves();
//     for (auto move1 : all1) {
//       position nw1 = make_move(nw, move1);
//       double val = nw1.eval();
//       mx1 = max(mx1, -val);
//       // output(move1);
//       // cout << val << endl << endl;
//     }
//     double dmx = -mx1;
//     // cout << "DMX " << dmx << endl;
//     if (mx < dmx) {
//       mx = dmx;
//       ans = move;
//     }
//     // output(move);
//     // cout << "DMX " << dmx << " " << mx << endl << endl;
//   }
//   return ans;
// }

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
        if (y == 6) {
          for (int i = 2; i <= 5; i++) {
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
          ans.push_back({{x, y}, {x + 1, y + 1}});
      }
      // takes left
      if (in(x - 1, y + 1) && cur.data[x - 1][y + 1].type != EMPTY &&
        cur.data[x - 1][y + 1].side != side) {
          ans.push_back({{x, y}, {x - 1, y + 1}});
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
        if (y == 2) {
          for (int i = 2; i <= 5; i++) {
            ans.push_back({{x, y}, {x, y + 1}, i});
          }
        }
        else {
          ans.push_back({{x, y}, {x, y - 1}});
        }
      }
      // takes right
      if (in(x + 1, y - 1) && cur.data[x + 1][y - 1].type != EMPTY &&
        cur.data[x + 1][y - 1].side != side) {
          ans.push_back({{x, y}, {x + 1, y - 1}});
      }
      // takes left
      if (in(x - 1, y - 1) && cur.data[x - 1][y - 1].type != EMPTY &&
        cur.data[x - 1][y - 1].side != side) {
          ans.push_back({{x, y}, {x - 1, y - 1}});
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
        ans.push_back({{x, y}, {x + 3, y}});
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
        ans.push_back({{x, y}, {x - 4, y}});
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

position starting;

void play_vs_ai() {
  position cur = starting;
  all_positions[cur]++;
  for (int it = 0; it < MAX_MOVES; it++) {
    // cout << "EVALUATION: " << cur.eval() << endl;
    if (cur.end_check()) {
      break;
    }
    step best = cur.choose_move();
    // output(best);
    best.output();
    cur = make_move(cur, best);
    all_positions[cur]++;
  }
}

bool check_move(position &cur, step last) {
  vector <step> possible_moves = 
    cur.data[last.from.x][last.from.y].get_all_steps(cur, last.from.x, last.from.y);
  possible_moves = remove_bad_moves(possible_moves, cur);
  for (step x : possible_moves) {
    if (x == last) {
      return 1;
    }
  }
  return 0;
}

void play_vs_human() {
  cout << "CHOOSE YOUR SIDE: TYPE WHITE OR BLACK" << endl;
  string s;
  while (cin >> s) {
    for (int i = 0; i < s.length(); i++) {
      s[i] = to_lower(s[i]);
    }
    if (s != "white" && s != "black") {
      cout << "WRONG FORMAT. TRY AGAIN" << endl;
      continue;
    }
    else {
      break;
    }
  }
  position cur = starting;  
  all_positions[cur]++;
  if (s == "black") {
    step best = cur.choose_move();
    // output(best);
    best.output();
    cur = make_move(cur, best);
  }
  while (1) {
    if (cur.end_check()) {
      break;
    }
    step user_move = input();
    // cout << "YOUR MOVE: ";
    // output(user_move);
    if (!check_move(cur, user_move)) {
      cout << "IMPOSSIBLE MOVE. TRY A DIFFERENT ONE" << endl;
      continue;
    }
    if (cur.end_check()) {
      break;
    }
    cur = make_move(cur, user_move);
    all_positions[cur]++;
    step best = cur.choose_move();
    // output(best);
    best.output();
    cur = make_move(cur, best);
    all_positions[cur]++;
    // cur.output();
  }
}

int main() {
  for (int i = 0; i < SZ; i++) {
    starting.data[i][1] = {WHITE, PAWN};
  }
  for (int i = 0; i < SZ; i++) {
    starting.data[i][6] = {BLACK, PAWN};
  }
  starting.data[0][0] = {WHITE, ROOK};
  starting.data[1][0] = {WHITE, KNIGHT};
  starting.data[2][0] = {WHITE, BISHOP};
  starting.data[3][0] = {WHITE, QUEEN};
  starting.data[4][0] = {WHITE, KING};
  starting.data[5][0] = {WHITE, BISHOP};
  starting.data[6][0] = {WHITE, KNIGHT};
  starting.data[7][0] = {WHITE, ROOK};

  starting.data[0][7] = {BLACK, ROOK};
  starting.data[1][7] = {BLACK, KNIGHT};
  starting.data[2][7] = {BLACK, BISHOP};
  starting.data[3][7] = {BLACK, QUEEN};
  starting.data[4][7] = {BLACK, KING};
  starting.data[5][7] = {BLACK, BISHOP};
  starting.data[6][7] = {BLACK, KNIGHT};
  starting.data[7][7] = {BLACK, ROOK};

  play_vs_ai();

  return 0;
}
