#include "Graph_deque.hpp"

Graph_deque::Graph_deque(int V)
{
    this->V = V;
    adj = new deque<int>[V]; // Create an array of deques, one for each vertex
}

// Destructor to free dynamically allocated memory for the adjacency list
Graph_deque::~Graph_deque()
{
    delete[] adj; // Deallocate the memory for the adjacency list array
}

void Graph_deque::DFS(int s, bool visitedV[])
{
    visitedV[s] = true;
    cout << s << " "; // Print the node as part of the current SCC

    for (auto i = adj[s].begin(); i != adj[s].end(); ++i)
        if (!visitedV[*i])
            DFS(*i, visitedV);
}

// Function to return the transpose (reverse) of the graph
Graph_deque Graph_deque::transpose()
{
    Graph_deque g(V);
    for (int s = 0; s < V; s++)
    {
        for (auto i = adj[s].begin(); i != adj[s].end(); ++i)
        {
            g.adj[*i].push_back(s);
        }
    }
    return g;
}

// Function to add an edge from vertex s to vertex d
void Graph_deque::addEdge(int s, int d)
{
    adj[s].push_back(d); // Add vertex d to the adjacency list of vertex s
}

// Recursive function to fill the stack with vertices in the order of their finishing times
void Graph_deque::fillOrder(int s, bool visitedV[], stack<int> &Stack)
{
    visitedV[s] = true;

    for (auto i = adj[s].begin(); i != adj[s].end(); ++i)
        if (!visitedV[*i])
            fillOrder(*i, visitedV, Stack);

    Stack.push(s); // After visiting all adjacent vertices, push the current vertex to the stack
}

// Function to print all strongly connected components (SCCs) in the graph
void Graph_deque::printSCC()
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
    Graph_deque gr = transpose();

    // Step 4: Mark all vertices as not visited for the second DFS on the transposed graph
    for (int i = 0; i < V; i++)
        visitedV[i] = false;

    // Step 5: Process all vertices in the order defined by the stack
    while (!Stack.empty())
    {
        int s = Stack.top();
        Stack.pop();

        // Perform DFS on the transposed graph for vertices that have not been visited
        if (!visitedV[s])
        {
            gr.DFS(s, visitedV); // This DFS will print all nodes in the current SCC
            cout << endl;
        }
    }

    delete[] visitedV;
}

void Graph_deque::removeEdge(int s, int d)
{
    // Use std::remove to remove the element
    adj[s].erase(std::remove(adj[s].begin(), adj[s].end(), d), adj[s].end());
}

void Graph_deque::clear()
{
    delete[] adj;
    adj = nullptr;
}