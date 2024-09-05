#include "Graph_deque.hpp"
#include "Graph_list.hpp"
#include <iostream>
#include <list>
#include <deque>
#include <stack>
#include <chrono>
#include <random>

using namespace std;

/** This program profiles the performance of Kosaraju's algorithm for finding
 strongly connected components (SCCs) in two different graph implementations:
 Graph_list and Graph_deque. It measures the time taken to add a specified
 number of edges and the time taken to run Kosaraju's algorithm.
*/

int main()
{
    int n = 1000;  
    int m = 10000; 
    
    // Random number generator for generating edges
    random_device rd;                         // Obtain a random number from hardware
    mt19937 gen(rd());                        // Seed the generator
    uniform_int_distribution<> dis(0, n - 1); // Define the range for vertex indices

    // Function to generate a random graph and profile the algorithm
    auto profileGraph = [&](auto &graph)
    {
        // Measure the time taken to add edges to the graph
        auto edgeStart = chrono::high_resolution_clock::now();

        // Add random edges to the graph
        for (int i = 0; i < m; ++i)
        {
            int u = dis(gen); // Random start vertex
            int v = dis(gen); // Random end vertex
            graph.addEdge(u, v);
        }

        auto edgeEnd = chrono::high_resolution_clock::now();
        chrono::duration<double> edgeElapsed = edgeEnd - edgeStart;
        cout << "Time to add edges: " << edgeElapsed.count() << " seconds" << endl;

        // Measure the time taken to run Kosaraju's algorithm
        auto algoStart = chrono::high_resolution_clock::now();

        // Run Kosaraju's algorithm to find SCCs
        graph.printSCC();

        auto algoEnd = chrono::high_resolution_clock::now();
        chrono::duration<double> algoElapsed = algoEnd - algoStart;
        cout << "Elapsed time for Kosaraju's algorithm: " << algoElapsed.count() << " seconds" << endl;
    };

    // Profile Graph_list implementation
    cout << "Profiling Graph_list:" << endl;
    Graph_list gl(n); // Create a Graph_list object with n vertices
    profileGraph(gl); // Profile the performance of the Graph_list implementation

    // Profile Graph_deque implementation
    cout << "Profiling Graph_deque:" << endl;
    Graph_deque gd(n); // Create a Graph_deque object with n vertices
    profileGraph(gd);  // Profile the performance of the Graph_deque implementation

    return 0;
}
