#include <bits/stdc++.h>
#include "tables.h"
#include "constants.h"
#include "engine.h"
#include "uci.h"

int main() {
    // play_vs_ai();
    map <position, int> all_positions;
    position current_position;
    uci_communication(all_positions, current_position);
    return 0;
}
