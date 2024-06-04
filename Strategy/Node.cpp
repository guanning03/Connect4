#include <iostream>
#include "Node.h"
#include "Point.h"
#include "Judge.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <float.h>
#include <cstdio>

int MCTNode::M;
int MCTNode::N;
int MCTNode::noX;
int MCTNode::noY;
clock_t MCTNode::start_time;
MCTNode* MCTNode::sub_tree;

double c = 0.6;
long long time_constrait = 2.5 * CLOCKS_PER_SEC;
std::random_device rd;
std::mt19937 gen(rd());
int get_random(int min, int max) {
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

MCTNode::MCTNode (int _prev_x, int _prev_y, int _prev_side, int ** _board, const int * _top, MCTNode* _father) :
father(_father), children(nullptr), num_child(0), prev_x(_prev_x), prev_y(_prev_y), prev_side(_prev_side),
n(0), W(0.0), winner(0){
    board = new int* [M];
    for (int i = 0; i < M; i++) {
        board[i] = new int [N];
        for (int j = 0; j < N; j++) {
            board[i][j] = _board[i][j];
        }
    }
    
    top = new int [N];
    for (int i = 0; i < N; i++) {
        top[i] = _top[i];
    }

    if (_prev_x >= 0 && _prev_y >= 0) {
        board[_prev_x][_prev_y] = _prev_side;
        top[_prev_y] = _prev_x;
        if (_prev_x - 1 == noX && _prev_y == noY) top[_prev_y]--;
        if (_prev_side == 1) {
            if (userWin(_prev_x, _prev_y, M, N, board)) winner = 1;
            else if (isTie(N, top)) winner = 3;
        } else if (_prev_side == 2) {
            if (machineWin(_prev_x, _prev_y, M, N, board)) winner = 2;
            else if (isTie(N, top)) winner = 3;
        }
    }
}

MCTNode::~MCTNode() {
    for (int i = 0; i < M; i++) {
        delete[] board[i];
    }
    delete[] board;
    delete[] top;
    if (children) {
        for (int i = 0; i < num_child; i++) {
            if (children[i] != sub_tree) delete children[i];
        }
        delete[] children;
    }
}

MCTNode* MCTNode::selection() {
    MCTNode* node = this;
    while (node->children) {
        node = node->best_child();
    }
    printf("我们选中的叶子节点是: %d %d %d\n", node->prev_x, node->prev_y, node->prev_side);
    return node;
}

MCTNode* MCTNode::expansion() {
    if (!winner && !children && n) {
        printf("我要对这个节点进行扩展：%d, %d, %d\n", prev_x, prev_y, prev_side);
        children = new MCTNode* [N];
        for (int play_y = 0; play_y < N; play_y++) {
            if (top[play_y] - 1 >= 0) {
                children[num_child++] = new MCTNode(top[play_y] - 1, play_y, 3 - prev_side, board, top, this);
            }
        }
        return children[0];
    } else {
        printf("这个节点是 %d,%d,%d; 因为其winner,children和n是%d,%d,%d,我不扩展它\n",prev_x,prev_y,prev_side,winner,children,n);
        return this;
    }
}

int MCTNode::simulation() {
    MCTNode* node = new MCTNode(prev_x, prev_y, prev_side, board, top, nullptr);
    while (!node->winner) {
        int play_y = rand() % N;
        while (node->top[play_y] == 0) play_y = rand() % N;
        MCTNode* new_node = new MCTNode(node->top[play_y] - 1, play_y, 3 - node->prev_side, node->board, node->top, nullptr);
        delete node;
        node = new_node;
    }
    int winner = node -> winner;
    delete node;
    return winner;
}

void MCTNode::backpropagation(int result) {
    double reward;
    if (result == 1) reward = 1;
    else if (result == 2) reward = -1;
    else reward = 0;
    MCTNode * node = this;
    printf("before bp\n");
    while (node) {
        printf("bp time\n");
        node->W += reward;
        node->n += 1;
        node = node->father;
    }
    printf("after bp\n");
}

Point MCTNode::MCTS() {
    // while (clock() - start_time < time_constrait) {
    //     MCTNode* node = selection();
    //     node = node->expansion();
    //     int result = node->simulation();
    //     node->backpropagation(result);
    // }   
    printf("flag1\n");
    int i = 0;
    while (clock() - start_time < time_constrait) {
        printf("simu %d\n", i);
        MCTNode* node = selection();
        printf("simu quat %d\n", i);
        node = node->expansion();
        printf("simu half %d\n", i);
        int result = rand() % 3 + 1;
        // int result = node->simulation();
        node->backpropagation(result);
        i++;
    } 
    printf("flag2\n");
    sub_tree = best_child();
    printf("flag3\n");
    int x = sub_tree->prev_x;
    int y = sub_tree->prev_y;
    printf("ready to return decision\n");
    return Point(x, y);
}

MCTNode* MCTNode::best_child() {
    printf("best_child\n");
    double best_ucb1 = -DBL_MAX;
    MCTNode* node = nullptr;
    printf("number of child: %d\n", num_child);
    for (int i = 0; i < num_child; i++) {
        double child_ucb1;
        if (children[i]->n) {
            child_ucb1 = children[i]->W / children[i]->n + c * std::sqrt(2 * log(n) / children[i]->n);
        } else {
            child_ucb1 = DBL_MAX;
        }
        if (child_ucb1 >= best_ucb1) {
            node = children[i];
            best_ucb1 = child_ucb1;
        }
    }
    printf("best_child end\n");
    return node;
}