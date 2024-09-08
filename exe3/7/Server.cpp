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
#include <pthread.h> 
#include <mutex>     
#include "Graph_deque.hpp"  

#define PORT 9034 

using namespace std;

Graph_deque *graph = nullptr;  // Shared graph resource
mutex graph_mutex;  // Mutex for protecting the shared graph


// Function to handle client commands (void* is generic pointer)
void* handleClient(void* client_socket_ptr) {
    int client_socket = *(int*)client_socket_ptr;  // Get the client socket
    free(client_socket_ptr);  // Free the dynamically allocated socket pointer (that allocate by the thread)

    char buffer[1024];

    // Main loop: Receive and process commands from the client
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Clear buffer
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);  // Receive data from client
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                cout << "Connection closed by client on socket " << client_socket << endl;
            } else {
                perror("Receive error");
            }
            close(client_socket);  // Close the socket
            pthread_exit(nullptr);  // Exit the thread
        }

        string command(buffer);  // Convert buffer to string

        // Lock the mutex before accessing the shared graph
        lock_guard<mutex> guard(graph_mutex);

        // Process the received command
        string response;
        if (command.substr(0, 8) == "Newgraph") {
            // If a previous graph exists, delete it
            if (graph != nullptr) {
                graph->clear();
                delete graph;
            }

            int n, m;
            sscanf(command.c_str(), "Newgraph %d,%d", &n, &m);
            graph = new Graph_deque(n);  // Create a new graph with n vertices

            response = "Send the edges (u v pairs).\n";
            send(client_socket, response.c_str(), response.length(), 0);  // Send the response to the client

            // Receive edges from the client
            for (int i = 0; i < m; i++) {
                memset(buffer, 0, sizeof(buffer));  // Clear buffer
                bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0) {
                    response = "Error receiving edges.\n";
                    send(client_socket, response.c_str(), response.length(), 0);
                    pthread_exit(nullptr);  // Exit thread on error
                }

                int u, v;
                sscanf(buffer, "%d %d", &u, &v);
                graph->addEdge(u, v);  // Add edge to the graph
            }

            response = "Graph created.\n";
        } else if (command == "Kosaraju") {
            if (graph != nullptr) {
                ostringstream scc_output;
                streambuf *cout_buf = cout.rdbuf();
                cout.rdbuf(scc_output.rdbuf());
                graph->printSCC();  // Run Kosaraju's algorithm
                cout.rdbuf(cout_buf);  // Restore original buffer
                response = scc_output.str();  // Get the SCC output
            } else {
                response = "No graph available.\n";
            }
        } else if (command.substr(0, 7) == "Newedge") {
            int u, v;
            sscanf(command.c_str(), "Newedge %d,%d", &u, &v);
            if (graph != nullptr) {
                graph->addEdge(u, v);  // Add edge to the graph
                response = "Edge added.\n";
            } else {
                response = "No graph available.\n";
            }
        } else if (command.substr(0, 10) == "Removeedge") {
            int u, v;
            sscanf(command.c_str(), "Removeedge %d,%d", &u, &v);
            if (graph != nullptr) {
                graph->removeEdge(u, v);  // Remove edge from the graph
                response = "Edge removed.\n";
            } else {
                response = "No graph available.\n";
            }
        } else if (command == "exit") {
            close(client_socket);  // Close the socket
            pthread_exit(nullptr);  // Exit thread
        } else {
            response = "Unknown command.\n";
        }

        send(client_socket, response.c_str(), response.length(), 0);  // Send the response to the client
    }
}

int main() {
    int listener_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create listener socket
    listener_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow port reuse
    int opt = 1;
    setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(listener_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(listener_socket);
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(listener_socket, 10) < 0) {
        perror("Listen failed");
        close(listener_socket);
        exit(EXIT_FAILURE);
    }

    cout << "Server is listening on port " << PORT << endl;

    // Main loop: accept new client connections
    while (true) {
        new_socket = accept(listener_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (new_socket < 0) {
            perror("Accept error");
            continue;
        }

        cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << " on socket " << new_socket << endl;

        // Create a new thread to handle this client
        pthread_t client_thread;
        int *client_socket_ptr = (int*)malloc(sizeof(int));
        // Dynamically allocate memory for socket, ensuring each thread gets its own copy of the client's socket. This avoids the risk of overwriting the value in a shared variable.
        *client_socket_ptr = new_socket;
        pthread_create(&client_thread, nullptr, handleClient, client_socket_ptr);  // Start a new thread for the client
        pthread_detach(client_thread);  // Detach the thread so it cleans up after itself
    }

    // Close the listener socket when the server shuts down (not reached in this example)
    close(listener_socket);
    return 0;
}
