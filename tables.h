#ifndef TABLES_H
#define TABLES_H
#include <vector>

using namespace std;

struct table {
    const int SZ = 8;
    vector <vector <vector <double> > > data;
    table();
};

#endif
