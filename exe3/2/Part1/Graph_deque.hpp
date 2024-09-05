#include <iostream>
#include <deque>
#include <stack>

using namespace std;

// Class representing a directed graph
class Graph_deque {
    int V;
    deque<int> *adj; // Pointer to an array of adjacency lists (deque representation of the graph)
    void fillOrder(int s, bool visitedV[], stack<int> &Stack);
    void DFS(int s, bool visitedV[]);

public:
    Graph_deque(int V);
    ~Graph_deque();
    void addEdge(int s, int d);
    void printSCC();
    Graph_deque transpose();
};
