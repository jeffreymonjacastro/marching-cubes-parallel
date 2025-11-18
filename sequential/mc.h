#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define pii pair<int, int>

const double EPSILON = 1e-8;

using namespace std;

//** Los 256 casos posibles de triangulaciones
// Axes are:
//
//      y
//      |     z
//      |   /
//      | /
//      +----- x
//
// Vertex and edge layout:
//
//            7             6
//            +-------------+               +-----6-------+
//          / |           / |             / |            /|
//        /   |         /   |          11   7         10   5
//    3 +-----+-------+  2  |         +-----+2------+     |
//      |   4 +-------+-----+ 5       |     +-----4-+-----+
//      |   /         |   /           3   8         1   9
//      | /           | /             | /           | /
//    0 +-------------+ 1             +------0------+
//

int triTable[256][16] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
    {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
    {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
    {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
    {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
    {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
    {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
    {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
    {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
    {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
    {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
    {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
    {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
    {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
    {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
    {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
    {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
    {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
    {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
    {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
    {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
    {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
    {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
    {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
    {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
    {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
    {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
    {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
    {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
    {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
    {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
    {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
    {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

// Pares de índices de vértices para cada arista del cubo
vector<pii> edge_vertice_mapper{
    {0, 1},
    {1, 2},
    {2, 3},
    {3, 0},
    {4, 5},
    {5, 6},
    {6, 7},
    {7, 4},
    {0, 4},
    {1, 5},
    {2, 6},
    {3, 7}};

class Point {
private:
  double x, y, z;

public:
  Point() : x(0), y(0), z(0) {}
  Point(double x, double y, double z) : x(x), y(y), z(z) {}

  double X() const { return x; }
  double Y() const { return y; }
  double Z() const { return z; }

  Point operator+(const Point &p) {
    return Point(this->x + p.x, this->y + p.y, this->z + p.z);
  }

  Point operator-(const Point &p) {
    return Point(this->x - p.x, this->y - p.y, this->z - p.z);
  }

  Point operator*(double scalar) {
    return Point(this->x * scalar, this->y * scalar, this->z * scalar);
  }

  Point operator/(double scalar) {
    return Point(this->x / scalar, this->y / scalar, this->z / scalar);
  }

  friend ostream &operator<<(ostream &plyfile, const Point &p) {
    plyfile << p.x << " " << p.y << " " << p.z;
    return plyfile;
  }
};

class Triangle {
private:
  Point p1, p2, p3;

public:
  Triangle() {}
  Triangle(Point p1, Point p2, Point p3) : p1(p1), p2(p2), p3(p3) {}

  Point P1() { return p1; }
  Point P2() { return p2; }
  Point P3() { return p3; }

  void getPly(fstream &plyfile) {
    plyfile << p1 << "\n";
    plyfile << p2 << "\n";
    plyfile << p3 << "\n";
  }
};

// Clase abstracta para funciones implicitas
class ImplicitFunction {
public:
  virtual ~ImplicitFunction() = default;
  virtual double evaluate(double x, double y, double z) const = 0;
};

// Funcion de la esfera: (x-cx)^2 + (y-cy)^2 + (z-cz)^2 - r^2 = 0
class Sphere : public ImplicitFunction {
private:
  Point center;
  double radius;     
public:
  Sphere() {}
  Sphere(double cx, double cy, double cz, double r) : center(cx, cy, cz), radius(r) {}
  
  double evaluate(double x, double y, double z) const override {
    return pow(x - center.X(), 2) + pow(y - center.Y(), 2) + pow(z - center.Z(), 2) - pow(radius, 2);
  }
};

// Funcion de un toro: ((x-cx)^2 + (y-cy)^2 + (z-cz)^2 + R^2 - r^2)^2 - 4*R^2*((x-cx)^2 + (z-cz)^2) = 0
class TorusFunction : public ImplicitFunction {
private:
  double cx, cy, cz;  // Centro
  double R;           // Radio mayor (del centro del toro al centro del tubo)
  double r;           // Radio menor (radio del tubo)
  
public:
  TorusFunction(double cx, double cy, double cz, double major_radius, double minor_radius)
    : cx(cx), cy(cy), cz(cz), R(major_radius), r(minor_radius) {}
  
  double evaluate(double x, double y, double z) const override {
    double dx = x - cx;
    double dy = y - cy;
    double dz = z - cz;
    double sum_sq = dx*dx + dy*dy + dz*dz;
    return pow(sum_sq + R*R - r*r, 2) - 4*R*R*(dx*dx + dz*dz);
  }
};

// Funcion de un cubo redondeado
class RoundedCubeFunction : public ImplicitFunction {
private:
  double cx, cy, cz;  // Centro
  double size;        // Tamaño del cubo
  double radius;      // Radio de redondeo
  
public:
  RoundedCubeFunction(double cx, double cy, double cz, double size, double radius)
    : cx(cx), cy(cy), cz(cz), size(size), radius(radius) {}
  
  double evaluate(double x, double y, double z) const override {
    double dx = abs(x - cx);
    double dy = abs(y - cy);
    double dz = abs(z - cz);
    double half_size = size / 2.0;
    
    double qx = dx - half_size;
    double qy = dy - half_size;
    double qz = dz - half_size;
    
    double max_q = max(max(qx, qy), qz);
    double length_pos = sqrt(max(qx, 0.0)*max(qx, 0.0) + 
                             max(qy, 0.0)*max(qy, 0.0) + 
                             max(qz, 0.0)*max(qz, 0.0));
    
    return length_pos + min(max_q, 0.0) - radius;
  }
};

// Función de Gyroid (superficie mínima periódica triply periodic minimal surface)
class GyroidFunction : public ImplicitFunction {
private:
  double cx, cy, cz;  // Centro
  double scale;       // Escala de la estructura
  double thickness;   // Grosor de la superficie
  
public:
  GyroidFunction(double cx, double cy, double cz, double scale, double thickness)
    : cx(cx), cy(cy), cz(cz), scale(scale), thickness(thickness) {}
  
  double evaluate(double x, double y, double z) const override {
    double dx = (x - cx) * scale;
    double dy = (y - cy) * scale;
    double dz = (z - cz) * scale;
    
    double gyroid = sin(dx) * cos(dy) + sin(dy) * cos(dz) + sin(dz) * cos(dx);
    return abs(gyroid) - thickness;
  }
};

// Función Metaball (blobs orgánicos con smooth blending)
class MetaballFunction : public ImplicitFunction {
private:
  vector<Point> centers;
  vector<double> radii;
  double threshold;
  
public:
  MetaballFunction(const vector<Point>& centers, const vector<double>& radii, double threshold = 1.0)
    : centers(centers), radii(radii), threshold(threshold) {}
  
  double evaluate(double x, double y, double z) const override {
    double sum = 0.0;
    for (size_t i = 0; i < centers.size(); ++i) {
      double dx = x - centers[i].X();
      double dy = y - centers[i].Y();
      double dz = z - centers[i].Z();
      double dist_sq = dx*dx + dy*dy + dz*dz;
      double r_sq = radii[i] * radii[i];
      
      if (dist_sq < r_sq * 4.0) {  // Influencia limitada
        sum += r_sq / (dist_sq + 0.0001);  // Evitar división por cero
      }
    }
    return threshold - sum;
  }
};

// Función Mandelbulb simplificada (fractal 3D)
class MandelbulbFunction : public ImplicitFunction {
private:
  double cx, cy, cz;
  double power;
  int iterations;
  double bailout;
  
public:
  MandelbulbFunction(double cx, double cy, double cz, double power = 8.0, 
                     int iterations = 10, double bailout = 2.0)
    : cx(cx), cy(cy), cz(cz), power(power), iterations(iterations), bailout(bailout) {}
  
  double evaluate(double x, double y, double z) const override {
    double dx = x - cx;
    double dy = y - cy;
    double dz = z - cz;
    
    double zx = dx, zy = dy, zz = dz;
    double dr = 1.0;
    double r = 0.0;
    
    for (int i = 0; i < iterations; i++) {
      r = sqrt(zx*zx + zy*zy + zz*zz);
      if (r > bailout) break;
      
      // Convertir a coordenadas esféricas
      double theta = acos(zz / r);
      double phi = atan2(zy, zx);
      dr = pow(r, power - 1.0) * power * dr + 1.0;
      
      // Escalar y rotar el punto
      double zr = pow(r, power);
      theta = theta * power;
      phi = phi * power;
      
      // Convertir de vuelta a coordenadas cartesianas
      zx = zr * sin(theta) * cos(phi) + dx;
      zy = zr * sin(theta) * sin(phi) + dy;
      zz = zr * cos(theta) + dz;
    }
    
    return 0.5 * log(r) * r / dr - 0.01;  // Ajustar threshold
  }
};

// Función Heart (corazón 3D) - Versión mejorada y más estable
class HeartFunction : public ImplicitFunction {
private:
  double cx, cy, cz;
  double scale;
  
public:
  HeartFunction(double cx, double cy, double cz, double scale = 1.0)
    : cx(cx), cy(cy), cz(cz), scale(scale) {}
  
  double evaluate(double x, double y, double z) const override {
    // Normalizar coordenadas
    double dx = (x - cx) / scale;
    double dy = (y - cy) / scale;
    double dz = (z - cz) / scale;
    
    // Fórmula alternativa más suave del corazón
    // Esta versión evita las singularidades problemáticas
    double x2 = dx * dx;
    double y2 = dy * dy;
    double z2 = dz * dz;
    
    // Versión mejorada que reduce las singularidades
    // Fórmula: (x² + (9/4)y² + z² - 1)³ - x²z³ - (9/80)y²z³ = 0
    double sum = x2 + 2.25 * y2 + z2 - 1.0;
    
    // Suavizar la función para evitar gradientes extremos
    // Usar una versión más controlada de la cúbica
    double term1;
    if (abs(sum) < 2.0) {
      // En zonas críticas, usar una aproximación más suave
      term1 = sum * sum * sum;
    } else {
      // Fuera de zonas críticas, comportamiento normal
      term1 = sum * sum * sum;
    }
    
    double z3 = dz * z2;
    double term2 = x2 * z3 + 0.1125 * y2 * z3;
    
    // Ajuste de escala y offset para mejor comportamiento numérico
    double result = term1 - term2;
    
    // Normalizar para evitar valores extremos
    return result * 0.3;
  }
};

// Función Heart alternativa (más simple y robusta)
class HeartFunctionSimple : public ImplicitFunction {
private:
  double cx, cy, cz;
  double scale;
  
public:
  HeartFunctionSimple(double cx, double cy, double cz, double scale = 1.0)
    : cx(cx), cy(cy), cz(cz), scale(scale) {}
  
  double evaluate(double x, double y, double z) const override {
    double dx = (x - cx) / scale;
    double dy = (y - cy) / scale;
    double dz = (z - cz) / scale;
    
    // Versión simplificada tipo "globo de corazón"
    // Más estable numéricamente
    double x2 = dx * dx;
    double y2 = dy * dy;
    double z2 = dz * dz;
    
    // Base esférica
    double sphere = x2 + y2 + z2 - 1.0;
    
    // Deformación en forma de corazón
    double heart_deform = -dx * dx * abs(dz) * 0.5 - 
                          abs(dy) * dz * dz * 0.3;
    
    return sphere + heart_deform;
  }
};

// Función compleja que combina múltiples formas con operaciones booleanas suaves
class ComplexHybridFunction : public ImplicitFunction {
private:
  double cx, cy, cz;
  double time;  // Para animaciones o variaciones
  
  // Operación de unión suave (smooth union)
  double smoothMin(double d1, double d2, double k) const {
    double h = max(k - abs(d1 - d2), 0.0) / k;
    return min(d1, d2) - h * h * k * 0.25;
  }
  
  // Operación de substracción suave
  double smoothSubtract(double d1, double d2, double k) const {
    double h = max(k - abs(-d1 - d2), 0.0) / k;
    return max(-d1, d2) + h * h * k * 0.25;
  }
  
  // Función de torsión
  Point twist(double x, double y, double z, double amount) const {
    double angle = amount * y;
    double c = cos(angle);
    double s = sin(angle);
    return Point(c * x - s * z, y, s * x + c * z);
  }
  
public:
  ComplexHybridFunction(double cx, double cy, double cz, double time = 0.0)
    : cx(cx), cy(cy), cz(cz), time(time) {}
  
  double evaluate(double x, double y, double z) const override {
    // Aplicar torsión al espacio
    Point twisted = twist(x - cx, y - cy, z - cz, 0.5);
    double tx = twisted.X();
    double ty = twisted.Y();
    double tz = twisted.Z();
    
    // 1. Toro principal
    double R = 40.0;  // Radio mayor
    double r = 15.0;  // Radio menor
    double dx_torus = tx;
    double dy_torus = ty;
    double dz_torus = tz;
    double sum_sq = dx_torus*dx_torus + dy_torus*dy_torus + dz_torus*dz_torus;
    double torus = pow(sum_sq + R*R - r*r, 2) - 4*R*R*(dx_torus*dx_torus + dz_torus*dz_torus);
    torus = pow(abs(torus), 0.25) - 8.0;  // Normalizar y ajustar
    
    // 2. Esferas orbitales con modulación sinusoidal
    double sphere1_x = tx - 35.0 * cos(time);
    double sphere1_y = ty - 25.0;
    double sphere1_z = tz - 35.0 * sin(time);
    double sphere1 = sphere1_x*sphere1_x + sphere1_y*sphere1_y + sphere1_z*sphere1_z - 400.0;
    
    double sphere2_x = tx + 35.0 * cos(time + 2.094);  // 120 grados
    double sphere2_y = ty + 25.0;
    double sphere2_z = tz + 35.0 * sin(time + 2.094);
    double sphere2 = sphere2_x*sphere2_x + sphere2_y*sphere2_y + sphere2_z*sphere2_z - 400.0;
    
    double sphere3_x = tx + 35.0 * cos(time + 4.189);  // 240 grados
    double sphere3_y = ty;
    double sphere3_z = tz + 35.0 * sin(time + 4.189);
    double sphere3 = sphere3_x*sphere3_x + sphere3_y*sphere3_y + sphere3_z*sphere3_z - 300.0;
    
    // 3. Componente gyroid para textura
    double scale = 0.1;
    double gyroid = sin(tx * scale) * cos(ty * scale) + 
                    sin(ty * scale) * cos(tz * scale) + 
                    sin(tz * scale) * cos(tx * scale);
    double gyroid_shell = abs(gyroid) - 0.3;
    gyroid_shell *= 100.0;  // Escalar para que sea un detalle
    
    // 4. Cubo redondeado central
    double cube_size = 25.0;
    double cube_x = abs(tx) - cube_size;
    double cube_y = abs(ty) - cube_size;
    double cube_z = abs(tz) - cube_size;
    double cube = sqrt(max(cube_x, 0.0)*max(cube_x, 0.0) + 
                       max(cube_y, 0.0)*max(cube_y, 0.0) + 
                       max(cube_z, 0.0)*max(cube_z, 0.0)) + 
                  min(max(max(cube_x, cube_y), cube_z), 0.0) - 5.0;
    
    // Combinar todas las formas con operaciones suaves
    double result = smoothMin(torus, sphere1, 10.0);
    result = smoothMin(result, sphere2, 10.0);
    result = smoothMin(result, sphere3, 10.0);
    result = smoothSubtract(cube, result, 8.0);  // Restar el cubo
    result = smoothMin(result, gyroid_shell, 5.0);  // Añadir textura gyroid
    
    // Modulación final con ondulaciones
    double wave = sin(tx * 0.2) * sin(ty * 0.2) * sin(tz * 0.2) * 3.0;
    
    return result + wave;
  }
};

class MarchingCubes {
private:
  vector<Triangle> triangles;
  int domain;
  int delta;
  string filename;
  ImplicitFunction* func;

public:
  MarchingCubes() {}
  MarchingCubes(int domain, int delta, const string &filename, ImplicitFunction* func)
      : domain(domain), delta(delta), filename(filename), func(func) {}

  void exportPly() {
    fstream plyfile(this->filename, ios::out);
    plyfile << "ply\n";
    plyfile << "format ascii 1.0\n";
    plyfile << "element vertex " << this->triangles.size() * 3 << "\n";
    plyfile << "property float x" << "\n";
    plyfile << "property float y" << "\n";
    plyfile << "property float z" << "\n";
    plyfile << "element face " << this->triangles.size() << "\n";
    plyfile << "property list uchar int vertex_indices" << "\n";
    plyfile << "end_header" << "\n";

    for (auto &triangle : triangles) {
      triangle.getPly(plyfile);
    }

    for (int i = 0; i < this->triangles.size(); i++) {
      plyfile << "3 " << i * 3 << " " << i * 3 + 1 << " " << i * 3 + 2 << "\n";
    }

    plyfile.close();
  }

  int generateCase(double x, double y, double z, double delta) {
    int whichCase = 0;

    double vertices[8][3] = {
      {x, y, z}, 
      {x + delta, y, z}, 
      {x + delta, y + delta, z}, 
      {x, y + delta, z}, 
      {x, y, z + delta}, 
      {x + delta, y, z + delta}, 
      {x + delta, y + delta, z + delta}, 
      {x, y + delta, z + delta}
    };

    for (int i = 0; i < 8; ++i) {
      if (this->func->evaluate(vertices[i][0], vertices[i][1], vertices[i][2]) > 0) {
        whichCase |= (1 << i);
      }
    }

    return whichCase;
  }

  Point findIntersection(Point p0, Point p1) {
    double v0 = this->func->evaluate(p0.X(), p0.Y(), p0.Z());
    double v1 = this->func->evaluate(p1.X(), p1.Y(), p1.Z());
    
    if (abs(v0) < EPSILON) return p0;
    if (abs(v1) < EPSILON) return p1;

    if (v0 * v1 > 0) return (p0 + p1) * 0.5;

    double t = v0 / (v0 - v1);
    
    t = max(0.0, min(1.0, t));

    return p0 + (p1 - p0) * t;
  }

  void generatePoints(double x, double y, double z, double delta) {
    int whichCase = generateCase(x, y, z, delta);
    if (whichCase == 0 || whichCase == 255) return;

    Point cubeVertices[8] = {
      Point(x, y, z),
      Point(x + delta, y, z),
      Point(x + delta, y + delta, z),
      Point(x, y + delta, z),
      Point(x, y, z + delta),
      Point(x + delta, y, z + delta),
      Point(x + delta, y + delta, z + delta),
      Point(x, y + delta, z + delta)
    };

    Point edgeIntersections[12];
    for (int i = 0; i < 12; ++i) {
      int v0 = edge_vertice_mapper[i].first;
      int v1 = edge_vertice_mapper[i].second;
      edgeIntersections[i] = findIntersection(cubeVertices[v0], cubeVertices[v1]);
    }

    for (int i = 0; triTable[whichCase][i] != -1; i += 3) {
      Point p1 = edgeIntersections[triTable[whichCase][i]];
      Point p2 = edgeIntersections[triTable[whichCase][i + 1]];
      Point p3 = edgeIntersections[triTable[whichCase][i + 2]];
      triangles.emplace_back(p1, p2, p3);
    }
  }

  void generateMesh() {

    auto start = chrono::high_resolution_clock::now();

    int divisions = domain / delta;

    for (int i = 0; i < divisions; ++i) {
      for (int j = 0; j < divisions; ++j) {
        for (int k = 0; k < divisions; ++k) {
          double x = i * delta;
          double y = j * delta;
          double z = k * delta;
          generatePoints(x, y, z, delta);
        }
      }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Mesh generated with " << triangles.size() << " triangles in " << elapsed.count() << " seconds.\n";
  }
};

