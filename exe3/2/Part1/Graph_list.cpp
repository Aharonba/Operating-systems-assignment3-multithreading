#include "Graph_list.hpp"

Graph_list::Graph_list(int V)
{
    this->V = V;
    adj = new list<int>[V]; // Create an array of lists, one for each vertex
}

Graph_list::~Graph_list()
{
    delete[] adj; // Deallocate the memory for the adjacency list array
}

void Graph_list::DFS(int s, bool visitedV[])
{
    visitedV[s] = true;
    cout << s << " "; // Print the node as part of the current SCC

    list<int>::iterator i;
    for (i = adj[s].begin(); i != adj[s].end(); ++i)
        if (!visitedV[*i])
            DFS(*i, visitedV);
}

// Function to return the transpose (reverse) of the graph
Graph_list Graph_list::transpose()
{
    Graph_list g(V);
    for (int s = 0; s < V; s++)
    {
        list<int>::iterator i;
        for (i = adj[s].begin(); i != adj[s].end(); ++i)
        {
            g.adj[*i].push_back(s);
        }
    }
    return g;
}

void Graph_list::addEdge(int s, int d)
{
    adj[s].push_back(d); // Add vertex d to the adjacency list of vertex s
}

// Recursive function to fill the stack with vertices in the order of their finishing times
void Graph_list::fillOrder(int s, bool visitedV[], stack<int> &Stack)
{
    visitedV[s] = true;

    list<int>::iterator i;
    for (i = adj[s].begin(); i != adj[s].end(); ++i)
        if (!visitedV[*i])
            fillOrder(*i, visitedV, Stack);

    Stack.push(s); // After visiting all adjacent vertices, push the current vertex to the stack
}

// Function to print all strongly connected components (SCCs) in the graph
void Graph_list::printSCC()
{
    stack<int> Stack;

    // Step 1: Mark all vertices as not visited for the first DFS
    bool *visitedV = new bool[V];
    for (int i = 0; i < V; i++)
        visitedV[i] = false;

    // Step 2: Fill vertices in the stack according to their finishing times
    for (int i = 0; i < V; i++)
        if (visitedV[i] == false)
            fillOrder(i, visitedV, Stack);

    // Step 3: Create a transposed (reversed) graph
    Graph_list gr = transpose();

    // Step 4: Mark all vertices as not visited for the second DFS on the transposed graph
    for (int i = 0; i < V; i++)
        visitedV[i] = false;

    // Step 5: Process all vertices in the order defined by the stack
    while (Stack.empty() == false)
    {
        int s = Stack.top();
        Stack.pop();

        // Perform DFS on the transposed graph for vertices that have not been visited
        if (visitedV[s] == false)
        {
            gr.DFS(s, visitedV); // This DFS will print all nodes in the current SCC
            cout << endl;
        }
    }

    delete[] visitedV;
}