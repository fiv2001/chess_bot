#ifndef UCI_H_INCLUDED
#define UCI_H_INCLUDED
#include "engine.h"

char to_lower(char c);

bool is_digit(char c);

bool is_letter(char c);

bool ok_input(string s);

step string_to_step(string s);

step input();

void output(step last);

void play_vs_ai(map <position, int> &all_positions);

bool check_move(position &cur, step last);

void play_vs_human(map <position, int> &all_positions); // used for playing in console

void input_uci();

void input_ready();

void input_new_game(map <position, int> &all_positions);

bool starts_with(string s, string pattern);

void input_position(string command, map <position, int> &all_positions, position &current_position);

void input_go(map <position, int> &all_positions, position &current_position);

void uci_communication(map <position, int> &all_positions, position &current_position);

#endif