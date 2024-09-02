#include <iostream>
#include <vector>
#include <stack>
#include <chrono>

using namespace std;

// Class representing a directed graph using an adjacency matrix
class Graph_matrix {
private:
    int V;                     
    vector<vector<bool>> adj;  // Adjacency matrix representation of the graph
    void fillOrder(int s, vector<bool>& visited, stack<int>& Stack);
    void DFS(int s, vector<bool>& visited);

public:
    Graph_matrix(int V);       
    void addEdge(int s, int d); 
    void printSCC();          // Print strongly connected components
    Graph_matrix transpose(); 
};