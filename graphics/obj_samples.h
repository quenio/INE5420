#pragma once

#include <string>

using namespace std;

static const string square
{
    "v -10   -10 100.0\n"
    "v -10  +110 100.0\n"
    "v +110 +110 100.0\n"
    "v +110  -10 100.0\n"
    "f 1 2 3 4"
};

static const string box
{
    "v 10   10 100\n"
    "v 10   10  10\n"
    "v 10  100  10\n"
    "v 10  100 100\n"
    "v 100  10 100\n"
    "v 100  10  10\n"
    "v 100 100  10\n"
    "v 100 100 100\n"
    "f 4 3 2 1\n"
    "f 2 6 5 1\n"
    "f 3 7 6 2\n"
    "f 8 7 3 4\n"
    "f 5 8 4 1\n"
    "f 6 7 8 5"
};

