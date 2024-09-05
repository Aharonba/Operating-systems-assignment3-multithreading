#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9034 // Define the port to connect to

using namespace std;

/**
 * Main function: Connects to the server, sends commands, and receives responses.
 */
int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0}; // Buffer to receive data from the server

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        cout << "Socket creation error" << endl;
        return -1;
    }

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT); // Convert port number to network byte order

    // Convert the server IP address from text to binary format
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        cout << "Invalid address or address not supported" << endl;
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cout << "Connection Failed" << endl;
        return -1;
    }

    // Main loop: Keep sending commands to the server and receive responses
    while (true)
    {
        string command;
        cout << "> ";          // Command prompt
        getline(cin, command); // Read the command from the user

        // Send the command to the server
        send(sock, command.c_str(), command.length(), 0);

        // Receive the response from the server
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0)
        {
            buffer[bytes_received] = '\0'; // Null-terminate the received data
            cout << buffer;                // Print the server's response
        }

        // If the command is Newgraph, send the edges
        if (command.substr(0, 8) == "Newgraph")
        {
            int n, m;
            sscanf(command.c_str(), "Newgraph %d,%d", &n, &m);

            // Send edge pairs (u v) to the server
            for (int i = 0; i < m; i++)
            {
                int u, v;
                cin >> u >> v;
                string edge = to_string(u) + " " + to_string(v) + "\n";
                send(sock, edge.c_str(), edge.length(), 0); // Send edge to server
            }
        }

        // If the command is "exit", break out of the loop
        if (command == "exit")
        {
            break;
        }
    }

    // Close the socket after exiting the loop
    close(sock);
    return 0;
}
