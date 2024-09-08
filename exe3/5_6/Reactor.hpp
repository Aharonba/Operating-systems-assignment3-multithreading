#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <sys/select.h>
#include <unistd.h>
#include <map>

// Function pointer type for callback functions
typedef void (*reactorFunc)(int fd); 

class Reactor
{
public:
    Reactor();  
    ~Reactor(); 

    void addFd(int fd, reactorFunc func);
    void removeFd(int fd);
    void run();  // Main loop to run the reactor
    void stop(); // Stop the reactor

private:
    std::map<int, reactorFunc> fd_to_func; // Map of file descriptors to their callback functions
    fd_set readfds;                        // Set of file descriptors to monitor
    int max_fd;
    bool running;
};

#endif 
