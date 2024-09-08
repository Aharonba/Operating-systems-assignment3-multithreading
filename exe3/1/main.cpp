#include "Graph.hpp"
#include <iostream>

using namespace std;

int main()
{
    int n, m;

    // Reading the number of vertices (n) and the number of arcs (m)
    cout << "Enter the number of vertices (n) and arcs (m): ";
    if (!(cin >> n >> m) || n <= 0 || m < 0)
    {
        cerr << "Invalid input. The number of vertices must be greater than 0, and the number of arcs must be non-negative." << endl;
        return 1;
    }

    // Create the graph with 'n' vertices
    Graph g(n);

    // Reading the arcs
    cout << "Enter the arcs (pairs of vertices, 0-based): " << endl;
    for (int i = 0; i < m; i++)
    {
        int u, v;
        if (!(cin >> u >> v) || u < 0 || v < 0 || u >= n || v >= n)
        {
            cerr << "Invalid arc input. Vertices must be between 0 and " << (n - 1) << "." << endl;
            return 1;
        }

        g.addEdge(u, v);
    }

    // Execute Kosaraju's algorithm and print the strongly connected components (SCCs)
    cout << "Strongly Connected Components:" << endl;
    g.printSCC();

    return 0;
}
