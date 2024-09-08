#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <mutex>
#include "Graph_deque.hpp"
#include "Reactor_Proactor.hpp"

#define PORT 9034

using namespace std;

Graph_deque *graph = nullptr;
mutex graph_mutex; // Mutex for protecting the shared graph

pthread_mutex_t scc_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for SCC condition, ensures that no two threads can check or update the scc_condition_met or scc_active flags at the same time, thus preventing race conditions.
pthread_cond_t scc_cond = PTHREAD_COND_INITIALIZER;    // Condition variable for SCC notification
bool scc_condition_met = false;                        // To track if 50% of the graph is in one SCC
bool scc_active = false;                               // To track if SCC has already been met, tracks whether the server has already printed a message about the 50% SCC condition being met.


void *handleClient(void *client_socket_ptr)
{
    int sockfd = *(int *)client_socket_ptr; // Cast void* back to int*
    free(client_socket_ptr);                // Free the dynamically allocated socket pointer

    char buffer[1024];

    // Main loop: Receive and process commands from the client
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));                            // Clear buffer
        int bytes_received = recv(sockfd, buffer, sizeof(buffer), 0); // Receive data from client
        if (bytes_received <= 0)
        {
            if (bytes_received == 0)
            {
                cout << "Connection closed by client on socket " << sockfd << endl;
            }
            else
            {
                perror("Receive error");
            }
            close(sockfd); // Close the socket
            return NULL;   // Exit the thread
        }

        string command(buffer); // Convert buffer to string

        // Lock the mutex before accessing the shared graph
        lock_guard<mutex> guard(graph_mutex);

        // Process the received command
        string response;
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

            response = "Send the edges (u v pairs).\n";
            send(sockfd, response.c_str(), response.length(), 0); // Send the response to the client

            // Receive edges from the client
            for (int i = 0; i < m; i++)
            {
                memset(buffer, 0, sizeof(buffer)); // Clear buffer
                bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0)
                {
                    response = "Error receiving edges.\n";
                    send(sockfd, response.c_str(), response.length(), 0);
                    return NULL; // Exit thread on error
                }

                int u, v;
                sscanf(buffer, "%d %d", &u, &v);
                graph->addEdge(u, v); // Add edge to the graph
            }

            response = "Graph created.\n";
        }
        else if (command == "Kosaraju")
        {
            if (graph != nullptr)
            {
                ostringstream scc_output;
                streambuf *cout_buf = cout.rdbuf();
                cout.rdbuf(scc_output.rdbuf());
                graph->printSCC();           // Run Kosaraju's algorithm
                cout.rdbuf(cout_buf);        // Restore original buffer
                response = scc_output.str(); // Get the SCC output

                // --- SCC Condition Checking ---
                pthread_mutex_lock(&scc_mutex);
                int largest_scc_size = graph->getLargestSCCSize(); 
                int num_vertices = graph->getNumVertices();        

                // Check if the largest SCC is at least 50% of the vertices
                if (largest_scc_size >= num_vertices / 2)
                {
                    if (!scc_condition_met)
                    {
                        scc_condition_met = true;
                        pthread_cond_signal(&scc_cond); // Signal the consumer thread
                    }
                }
                else
                {
                    if (scc_condition_met)
                    {
                        scc_condition_met = false;
                        pthread_cond_signal(&scc_cond); // Signal the consumer thread
                    }
                }
                pthread_mutex_unlock(&scc_mutex);
            }
            else
            {
                response = "No graph available.\n";
            }
        }
        else if (command.substr(0, 7) == "Newedge")
        {
            int u, v;
            sscanf(command.c_str(), "Newedge %d,%d", &u, &v);
            if (graph != nullptr)
            {
                graph->addEdge(u, v); // Add edge to the graph
                response = "Edge added.\n";
            }
            else
            {
                response = "No graph available.\n";
            }
        }
        else if (command.substr(0, 10) == "Removeedge")
        {
            int u, v;
            sscanf(command.c_str(), "Removeedge %d,%d", &u, &v);
            if (graph != nullptr)
            {
                graph->removeEdge(u, v); // Remove edge from the graph
                response = "Edge removed.\n";
            }
            else
            {
                response = "No graph available.\n";
            }
        }
        else if (command == "exit")
        {
            close(sockfd); // Close the socket
            return NULL;   // Exit thread
        }
        else
        {
            response = "Unknown command.\n";
        }

        send(sockfd, response.c_str(), response.length(), 0); // Send the response to the client
    }
}

// --- Consumer Thread for SCC ---
void *scc_consumer(void *arg)
{
    while (true)
    {
        pthread_mutex_lock(&scc_mutex);
        pthread_cond_wait(&scc_cond, &scc_mutex);

        if (scc_condition_met && !scc_active)
        {
            std::cout << "At least 50% of the graph belongs to the same SCC\n";
            scc_active = true;
        }
        else if (!scc_condition_met && scc_active)
        {
            std::cout << "At least 50% of the graph no longer belongs to the same SCC\n";
            scc_active = false;
        }

        pthread_mutex_unlock(&scc_mutex);
    }
    return NULL;
}
// --- End of Consumer Thread ---

int main()
{
    int listener_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create listener socket
    listener_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_socket < 0)
    {
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
    if (bind(listener_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(listener_socket);
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(listener_socket, 10) < 0)
    {
        perror("Listen failed");
        close(listener_socket);
        exit(EXIT_FAILURE);
    }

    cout << "Server is listening on port " << PORT << endl;

    // Start the consumer thread
    pthread_t consumer_thread;
    pthread_create(&consumer_thread, NULL, scc_consumer, NULL);

    // Instantiate the Proactor
    Proactor<proactorFunc> proactor;

    // Main loop: accept new client connections
    while (true)
    {
        client_socket = accept(listener_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0)
        {
            perror("Accept error");
            continue;
        }

        cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << " on socket " << client_socket << endl;

        // Start a new Proactor thread for the client
        pthread_t client_thread = proactor.startProactor(client_socket, (proactorFunc)handleClient);
        if (client_thread == 0)
        {
            cerr << "Failed to create client handler thread." << endl;
            close(client_socket); // Close the client socket on failure
        }
    }

    // Close the listener socket when the server shuts down (not reached in this example)
    close(listener_socket);
    return 0;
}
