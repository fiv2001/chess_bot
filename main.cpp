#include <bits/stdc++.h>
#include "tables.h"
#include "constants.h"
#include "engine.h"
#include "uci.h"

int main() {
    map <position, int> all_positions;
    position current_position;
    // play_vs_ai();
    uci_communication(all_positions, current_position);
    return 0;
}
