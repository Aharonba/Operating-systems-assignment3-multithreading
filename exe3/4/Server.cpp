#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <deque>
#include <stack>
#include <algorithm>
#include <vector>
#include "Graph_deque.hpp" 

#define PORT 9034 // Define the port for the server

using namespace std;

// Function to handle various commands related to graph operations
// This function supports creating new graphs, adding edges, removing edges, running the Kosaraju algorithm, etc.
string handleCommand(const string &command, Graph_deque *&graph, int client_socket)
{
    // Create a new graph based on the number of vertices and edges specified
    if (command.substr(0, 8) == "Newgraph")
    {
        // If a previous graph exists, delete it
        if (graph != nullptr)
        {
            graph->clear();
            delete graph;
        }

        int n, m;
        sscanf(command.c_str(), "Newgraph %d,%d", &n, &m);

        graph = new Graph_deque(n); // Create a new graph with n vertices

        // Inform the client that the server is ready to receive the edges
        string response = "Send the edges (u v pairs).\n";
        send(client_socket, response.c_str(), response.length(), 0);

        // Buffer to receive edge input from the client
        char buffer[1024];
        for (int i = 0; i < m; i++)
        {
            // Receive the edge pair (u v) from the client
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0)
            {
                return "Error receiving edges from client.\n";
            }

            // Null-terminate the received string
            buffer[bytes_received] = '\0';

            // Parse the edge from the received buffer
            int u, v;
            if (sscanf(buffer, "%d %d", &u, &v) != 2)
            {
                return "Error parsing edge data.\n";
            }

            // Add edge to the graph
            graph->addEdge(u, v);
        }

        // Send confirmation to the client
        return "Graph created with " + to_string(n) + " vertices and " + to_string(m) + " edges.\n";
    }
    else if (command == "Kosaraju")
    {
        // Check if a graph object is available
        if (graph != nullptr)
        {
            ostringstream scc_output;
            streambuf *cout_buf = cout.rdbuf();
            cout.rdbuf(scc_output.rdbuf());

            // Execute Kosaraju's algorithm to find SCCs
            graph->printSCC();

            cout.rdbuf(cout_buf); // Restore the original buffer

            // Return the SCC results as a string
            return scc_output.str();
        }
        else
        {
            return "No graph available.\n";
        }
    }
    else if (command.substr(0, 7) == "Newedge")
    {
        int u, v;
        sscanf(command.c_str(), "Newedge %d,%d", &u, &v);

        if (graph != nullptr)
        {
            graph->addEdge(u, v); // Add the new edge to the graph
            return "Edge added between " + to_string(u) + " and " + to_string(v) + ".\n";
        }
        else
        {
            return "No graph available.\n";
        }
    }
    else if (command.substr(0, 10) == "Removeedge")
    {
        int u, v;
        sscanf(command.c_str(), "Removeedge %d,%d", &u, &v);

        if (graph != nullptr)
        {
            graph->removeEdge(u, v); // Remove the specified edge
            return "Edge removed between " + to_string(u) + " and " + to_string(v) + ".\n";
        }
        else
        {
            return "No graph available.\n";
        }
    }
    else if (command == "exit")
    {
        return "exit"; // Client requested to exit
    }
    else
    {
        return "Unknown command.\n"; // Unrecognized command
    }
}

int main()
{
    int listener_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    fd_set master, read_fds; // File descriptor sets for select()

    int fdmax; // Maximum file descriptor number

    socklen_t addrlen;
    char buffer[1024]; // Buffer for receiving data

    // Create a socket
    listener_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_socket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set the SO_REUSEADDR option to allow port reuse
    int opt = 1;
    if (setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT); // Convert port number to network byte order

    // Bind the socket to the server address
    if (bind(listener_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(listener_socket, 10) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the file descriptor sets
    FD_ZERO(&master);                 // Clear the master set
    FD_ZERO(&read_fds);               // Clear the temporary set
    FD_SET(listener_socket, &master); // Add the listener socket to the master set
    fdmax = listener_socket;          // Set the maximum file descriptor number

    Graph_deque *graph = nullptr; // Pointer to hold the graph

    // Main loop: Server will keep running to accept and handle client connections
    while (true)
    {
        read_fds = master; // Copy the master set to the temporary set
        if (select(fdmax + 1, &read_fds, nullptr, nullptr, nullptr) == -1)
        {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        // Iterate over the file descriptors
        for (int i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == listener_socket)
                {
                    // Handle new incoming connections
                    addrlen = sizeof(client_addr);
                    new_socket = accept(listener_socket, (struct sockaddr *)&client_addr, &addrlen);
                    if (new_socket < 0)
                    {
                        perror("Accept error");
                    }
                    else
                    {
                        FD_SET(new_socket, &master); // Add the new socket to the master set
                        if (new_socket > fdmax)
                        {
                            fdmax = new_socket; // Update the maximum file descriptor
                        }
                        cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << " on socket " << new_socket << endl;
                    }
                }
                else
                {
                    // Handle data from a client
                    memset(buffer, 0, sizeof(buffer)); // Clear the buffer
                    int bytes_received = recv(i, buffer, sizeof(buffer), 0);
                    if (bytes_received <= 0)
                    {
                        if (bytes_received == 0)
                        {
                            cout << "Connection closed by client on socket " << i << endl;
                        }
                        else
                        {
                            perror("Receive error");
                        }
                        close(i);           // Close the socket
                        FD_CLR(i, &master); // Remove the socket from the master set
                    }
                    else
                    {
                        // Process the received command
                        string command(buffer);
                        string response = handleCommand(command, graph, i);

                        if (response == "exit")
                        {
                            close(i);
                            FD_CLR(i, &master);
                            cout << "Client disconnected on socket " << i << endl;
                        }
                        else
                        {
                            send(i, response.c_str(), response.length(), 0); // Send the response back to the client
                        }
                    }
                }
            }
        }
    }

    // Cleanup: Close the listener socket and delete the graph if necessary
    close(listener_socket);
    if (graph != nullptr)
    {
        delete graph;
    }

    return 0;
}
