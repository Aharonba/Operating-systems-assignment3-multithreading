#include "Graph_matrix.hpp"

Graph_matrix::Graph_matrix(int V) : V(V)
{
    adj.resize(V, vector<bool>(V, false));
}

void Graph_matrix::addEdge(int s, int d)
{
    adj[s][d] = true;
}

// Perform DFS and fill vertices in stack according to their finishing times
void Graph_matrix::fillOrder(int s, vector<bool> &visited, stack<int> &Stack)
{
    visited[s] = true;

    for (int i = 0; i < V; i++)
    {
        if (adj[s][i] && !visited[i])
        {
            fillOrder(i, visited, Stack);
        }
    }

    Stack.push(s);
}

// Perform DFS on the transposed graph
void Graph_matrix::DFS(int s, vector<bool> &visited)
{
    visited[s] = true;
    cout << s << " ";

    for (int i = 0; i < V; i++)
    {
        if (adj[s][i] && !visited[i])
        {
            DFS(i, visited);
        }
    }
}

// Return the transpose of the graph
Graph_matrix Graph_matrix::transpose()
{
    Graph_matrix g_transpose(V);

    for (int i = 0; i < V; i++)
    {
        for (int j = 0; j < V; j++)
        {
            if (adj[i][j])
            {
                g_transpose.adj[j][i] = true;
            }
        }
    }

    return g_transpose;
}

// Print all strongly connected components (SCCs) using Kosaraju's algorithm
void Graph_matrix::printSCC()
{
    stack<int> Stack;
    vector<bool> visited(V, false);

    // Fill vertices in stack according to their finishing times
    for (int i = 0; i < V; i++)
    {
        if (!visited[i])
        {
            fillOrder(i, visited, Stack);
        }
    }

    // Get the transpose of the graph
    Graph_matrix g_transpose = transpose();

    // Mark all vertices as not visited for second DFS
    fill(visited.begin(), visited.end(), false);

    // Process all vertices in order defined by stack
    while (!Stack.empty())
    {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v])
        {
            g_transpose.DFS(v, visited);
            cout << endl;
        }
    }
}