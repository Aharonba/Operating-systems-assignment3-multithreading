#include <iostream>
#include <list>
#include <stack>

using namespace std;

// Class representing a directed graph
class Graph_list {
  int V;
  list<int> *adj; // Pointer to an array of adjacency lists (adjacency list representation of the graph)
  void fillOrder(int s, bool visitedV[], stack<int> &Stack);
  void DFS(int s, bool visitedV[]);

   public:
  Graph_list(int V);
  ~Graph_list();
  void addEdge(int s, int d);
  void printSCC();
  Graph_list transpose();
};

