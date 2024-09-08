#ifndef REACTOR_PROACTOR_HPP
#define REACTOR_PROACTOR_HPP

#include <pthread.h>
#include <sys/select.h>
#include <unordered_map>
#include <pthread.h>
#include <iostream>

// Reactor function signature (function pointer for the Reactor callback)
typedef void (*reactorFunc)(int sockfd);

// Typedef for the Proactor function signature (must accept void* and return void*)
typedef void *(*proactorFunc)(void *);

class Reactor
{
public:
    Reactor();
    ~Reactor();
    void addFd(int fd, reactorFunc func);
    void removeFd(int fd);
    void run();  // Run the Reactor event loop
    void stop(); // Stop the Reactor

private:
    bool running;   // Reactor running state
    fd_set readfds; // File descriptor set for select()
    int max_fd;
    std::unordered_map<int, reactorFunc> fd_to_func; // Map fd to function callback
};

// Proactor Template Declaration
template <typename Func = proactorFunc>
class Proactor
{
public:
    // Starts the Proactor: creates a thread to handle the client
    pthread_t startProactor(int sockfd, Func threadFunc)
    {
        pthread_t tid; // Thread ID

        // Allocate memory for socket descriptor and pass it as a void* to the thread
        int *sockfd_ptr = (int *)malloc(sizeof(int));
        *sockfd_ptr = sockfd;

        // Create a new thread for the client handler function (void* signature)
        if (pthread_create(&tid, NULL, threadFunc, (void *)sockfd_ptr) != 0)
        {
            std::cerr << "Error creating thread." << std::endl;
            free(sockfd_ptr); // Free memory in case of failure
            return 0;         // Return 0 on failure
        }

        static int counter = 1;

        std::cout << "Thread number: " << counter << std::endl;
        counter++;

        return tid; // Return the thread ID
    }

    // Stops the Proactor: joins the thread and waits for it to finish
    int stopProactor(pthread_t tid)
    {
        // Wait for the thread to finish
        int ret = pthread_join(tid, NULL);
        if (ret != 0)
        {
            std::cerr << "Error stopping thread." << std::endl;
            return ret;
        }

        return 0; // Return success
    }
};

#endif // REACTOR_PROACTOR_HPP
