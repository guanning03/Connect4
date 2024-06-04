#include <bits/types/clock_t.h>
#include <iostream>
#include "Point.h"
#include "Judge.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <float.h>
#include <random>


class MCTNode {
public:
    static int M, N, noX, noY;
    static clock_t start_time;
    static MCTNode* sub_tree;

    MCTNode * father;
    MCTNode ** children;
    int num_child;
    int ** board;
    int * top;
    int prev_x, prev_y, prev_side;
    int n;
    double W;
    int winner;
    MCTNode(int _prev_x, int _prev_y, int _prev_side, int ** _board, const int * _top, MCTNode* _father);
    ~MCTNode();
    
    MCTNode* check_must();
    MCTNode* best_child();
    MCTNode* selection(MCTNode* must);
    MCTNode* expansion();
    int simulation();
    void backpropagation(int result);
    Point MCTS();
    MCTNode* decision();
};