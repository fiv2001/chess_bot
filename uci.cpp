#include "constants.h"
#include "uci.h"
#include "engine.h"

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

step string_to_step(string s) {
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
    step ans = string_to_step(s);
    return ans;
}

void output(step last) {
    cout << static_cast<char>(last.from.x + 'A');
    cout << last.from.y + 1;
    cout << static_cast<char>(last.to.x + 'A');
    cout << last.to.y + 1 << endl;
}

void position::output() {
    for (int i = SZ - 1; i >= 0; i--) {
        for (int j = 0; j < SZ; j++) {
            if (data[j][i].type == EMPTY) {
                cout << ".";
                continue;
            }
            char x = ' ';
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
            if (data[j][i].side == BLACK) {
                // cout << j << " " << i << endl;
                x = to_lower(x);
            }
            cout << x;
        }
        cout << endl;
    }
}

void play_vs_ai(map <position, int> &all_positions) {
    position cur;
    all_positions[cur]++;
    for (int it = 0; it < MAX_MOVES; it++) {
        // cout << "EVALUATION: " << cur.eval() << endl;
        if (cur.end_check(all_positions)) {
            break;
        }
        step best = cur.choose_move(all_positions);
        // output(best);
        best.output();
        // cout << cnt_pos << endl;
        // cnt_pos = 0;
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

void play_vs_human(map <position, int> &all_positions) { // used for playing in console
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
    position cur;
    all_positions[cur]++;
    if (s == "black") {
        step best = cur.choose_move(all_positions);
        // output(best);
        best.output();
        cur = make_move(cur, best);
    }
    while (true) {
        if (cur.end_check(all_positions)) {
            break;
        }
        step user_move = input();
        // cout << "YOUR MOVE: ";
        // output(user_move);
        if (!check_move(cur, user_move)) {
            cout << "IMPOSSIBLE MOVE. TRY A DIFFERENT ONE" << endl;
            continue;
        }
        if (cur.end_check(all_positions)) {
            break;
        }
        cur = make_move(cur, user_move);
        all_positions[cur]++;
        step best = cur.choose_move(all_positions);
        // output(best);
        best.output();
        cur = make_move(cur, best);
        all_positions[cur]++;
        // cur.output();
    }
}

// position current_position;

void input_uci() {
    cout << "id name " + ENGINE_NAME << endl;
    cout << "id author pink_bittern" << endl;
    cout << "uciok" << endl;
}

void input_ready() {
    cout << "readyok" << endl;
}

void input_new_game(map <position, int> &all_positions) {
    all_positions.clear();
}

bool starts_with(string s, string pattern) {
    if (s.length() < pattern.length()) {
        return 0;
    }
    return s.substr(0, pattern.length()) == pattern;
}

void input_position(string command, map <position, int> &all_positions, position &current_position) {
    all_positions.clear();
    if (command.find("startpos") != command.npos) {
        current_position = position();
    }
    if (command.find("moves") != command.npos) {
        int i = command.find("moves");
        for (int j = i + 6; j < command.length(); j++) {
            string move;
            for (int i1 = j; i1 < command.length(); i1++, j++) {
                if (command[i1] == ' ') {
                    break;
                }
                move += command[i1];
            }
            step nw = string_to_step(move);
            // cout << "inputted move ";
            // nw.output();
            current_position = make_move(current_position, nw);
            all_positions[current_position]++;
        }
    }
}

void input_go(map <position, int> &all_positions, position &current_position) {
    if (current_position.win()) {
        cout << "WIN" << endl;
    }
    if (current_position.draw(all_positions)) {
        cout << "DRAW" << endl;
    }
    step x = current_position.choose_move(all_positions);
    current_position.output();
    cout << "bestmove ";
    x.output();
}

void uci_communication(map <position, int> &all_positions, position &current_position) {
    while (true) {
        string command;
        getline(cin, command);
        if (command == "uci") {
            input_uci();
        }
        if (command == "isready") {
            input_ready();
        }
        if (command == "ucinewgame") {
            input_new_game(all_positions);
        }
        if (starts_with(command, "position")) {
            input_position(command, all_positions, current_position);
        }
        if (starts_with(command, "go")) {
            input_go(all_positions, current_position);
        }
        if (command == "stop") {
            return;
        }
    }
}