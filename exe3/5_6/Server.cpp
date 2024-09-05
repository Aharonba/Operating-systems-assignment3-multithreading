#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Reactor.hpp"  
#include "Graph_deque.hpp"  // Import Graph class

#define PORT 9034 

using namespace std;

// Global Reactor object
Reactor reactor;

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

// Function to handle client connections
void handleClientConnection(int client_socket)
{
    static Graph_deque *graph = nullptr; // Global graph object

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer)); // Clear the buffer

    // Receive the command from the client
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0)
    {
        if (bytes_received == 0)
        {
            cout << "Connection closed by client on socket " << client_socket << endl;
        }
        else
        {
            perror("Receive error");
        }
        close(client_socket); // Close the socket
        return;
    }

    // Process the received command
    string command(buffer);
    string response = handleCommand(command, graph, client_socket);

    if (response == "exit")
    {
        close(client_socket); // Close the connection on "exit" command
        cout << "Client disconnected on socket " << client_socket << endl;
    }
    else
    {
        send(client_socket, response.c_str(), response.length(), 0); // Send the response back to the client
    }
}

// Function to handle new incoming connections
void handleNewConnection(int listener_fd)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd >= 0)
    {
        cout << "New client connected: " << client_fd << endl;
        // Add the client socket to the reactor to handle their commands
        reactor.addFd(client_fd, handleClientConnection); // Add client connection handler
    }
}

int main()
{
    int listener_socket;
    struct sockaddr_in server_addr;

    // Create a socket for the server
    listener_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Set socket options to allow port reuse
    int opt = 1;
    setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT); // Convert port number to network byte order

    // Bind the socket to the server address
    if (bind(listener_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(listener_socket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(listener_socket, 10) < 0)
    {
        perror("Listen failed");
        close(listener_socket);
        return 1;
    }

    // Add the listener socket to the reactor
    reactor.addFd(listener_socket, handleNewConnection); 

    // Start the reactor to handle incoming connections and client commands (in the first connection the server will play his function)
    reactor.run();

    // Cleanup the global graph when the server is stopped
    close(listener_socket);
    return 0;
}
