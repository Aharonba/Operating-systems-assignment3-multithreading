#include <iostream>
#include <deque>
#include <stack>
#include <string>
#include <algorithm>

using namespace std;

class Graph_deque
{
    int V;
    deque<int> *adj; // Pointer to an array of adjacency lists (deque representation of the graph)
    void fillOrder(int s, bool visitedV[], stack<int> &Stack);
    void DFS(int s, bool visitedV[]);
    void DFSCount(int s, bool visitedV[], int &count); // Helper function to count the size of SCC

public:
    Graph_deque(int V);
    ~Graph_deque();
    void addEdge(int s, int d);
    void removeEdge(int s, int d);
    void printSCC();
    Graph_deque transpose();
    void clear();

    int getNumVertices();
    int getLargestSCCSize(); // Returns the size of the largest SCC
};
