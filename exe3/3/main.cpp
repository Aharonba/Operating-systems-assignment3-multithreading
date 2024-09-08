#include "Graph_deque.hpp"
#include <iostream>
#include <list>
#include <deque>
#include <stack>
#include <chrono>
#include <random>

using namespace std;

int main()
{
    Graph_deque *graph = nullptr;

    // Variable to store the user input command
    string command;

    // Main loop to process commands from stdin
    while (getline(cin, command))
    {
        if (command.substr(0, 8) == "Newgraph")
        {
            // If a graph already exists, clear it and deallocate memory
            if (graph != nullptr)
            {
                graph->clear();
                delete graph;
            }

            int n, m;

            // Parse the 'Newgraph n,m' command and extract n and m values
            sscanf(command.c_str(), "Newgraph %d,%d", &n, &m);

            graph = new Graph_deque(n);

            for (int i = 0; i < m; i++)
            {
                int u, v;
                cin >> u >> v;
                graph->addEdge(u, v); // Add an edge between vertex u and vertex v
            }

            // Ignore the remaining newline character from the input buffer
            cin.ignore();
        }
        // Handle the 'Kosaraju' command: calculates strongly connected components using Kosaraju's algorithm
        else if (command == "Kosaraju")
        {
            if (graph != nullptr)
            {
                graph->printSCC(); // Print the strongly connected components of the graph
            }
            else
            {
                cout << "No graph available." << endl;
            }
        }
        else if (command.substr(0, 7) == "Newedge")
        {
            int i, j;

            // Parse the 'Newedge i,j' command and extract i and j values
            sscanf(command.c_str(), "Newedge %d,%d", &i, &j);

            if (graph != nullptr)
            {
                graph->addEdge(i, j);
            }
            else
            {
                cout << "No graph available." << endl;
            }
        }
        // Handle the 'Removeedge' command: removes an edge between two vertices
        else if (command.substr(0, 10) == "Removeedge")
        {
            int i, j;

            // Parse the 'Removeedge i,j' command and extract i and j values
            sscanf(command.c_str(), "Removeedge %d,%d", &i, &j);

            if (graph != nullptr)
            {
                graph->removeEdge(i, j); // Remove the edge between vertex i and vertex j
            }
            else
            {
                cout << "No graph available." << endl;
            }
        }
        else if (command == "exit")
        {
            break; // Exit the loop and terminate the program
        }
        // Handle any unknown commands
        else
        {
            cout << "Unknown command." << endl;
        }
    }

    // If a graph exists at the end of the program, deallocate memory to prevent leaks
    if (graph != nullptr)
    {
        delete graph;
    }

    return 0;
}
