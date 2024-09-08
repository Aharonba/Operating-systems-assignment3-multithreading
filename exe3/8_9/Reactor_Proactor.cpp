#include "Reactor_Proactor.hpp"
#include <iostream>
#include <unistd.h>  
#include <pthread.h>
#include <mutex>

// Reactor Implementation

// Constructor
Reactor::Reactor() {
    running = true;    // Set the reactor to a running state
    FD_ZERO(&readfds); // Initialize the fd_set to an empty set
    max_fd = 0;        
}

// Destructor
Reactor::~Reactor() {
    stop(); // Ensure the reactor is stopped when destructed
}

// Add a file descriptor and its associated callback function to the reactor
void Reactor::addFd(int fd, reactorFunc func) {
    fd_to_func[fd] = func; // Add to the map
    FD_SET(fd, &readfds);  // Add the FD to the fd_set
    if (fd > max_fd) {
        max_fd = fd; // Update the maximum FD if needed
    }
}

// Remove a file descriptor from the reactor
void Reactor::removeFd(int fd) {
    fd_to_func.erase(fd); // Remove from the map
    FD_CLR(fd, &readfds); // Remove the FD from the fd_set
}

// Run the Reactor loop (main event loop)
void Reactor::run() {
    fd_set tempfds; // Temporary fd_set for select()

    while (running) {
        tempfds = readfds; // Copy the original fd_set (select modifies the fd_set)

        // Wait for one or more file descriptors to be ready
        int ret = select(max_fd + 1, &tempfds, NULL, NULL, NULL);
        if (ret < 0) {
            perror("Select error");
            break;
        }

        // Iterate over the file descriptors and check which are ready
        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &tempfds)) { // Check if the FD is ready for reading
                fd_to_func[fd](fd); // Call the corresponding callback
            }
        }
    }
}

// Stops the reactor
void Reactor::stop() {
    running = false; // Set the running flag to false to exit the loop
}

// Proactor Implementation

