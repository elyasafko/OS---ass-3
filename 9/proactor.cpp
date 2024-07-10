#include "proactor.hpp"
#include <stdexcept>
#include <iostream>

Proactor::Proactor() {}

Proactor::~Proactor() {}

// Thread starter function
void *threadStart(void *arg)
{
    // Unpack the argument pair (socket file descriptor and the function to handle it)
    auto *pair = static_cast<std::pair<int, proactorFunc> *>(arg);
    int sockfd = pair->first;
    proactorFunc func = pair->second;
    std::cout << "Thread started for socket: " << sockfd << std::endl;
    func(sockfd); // Execute the function with the socket descriptor
    delete pair;  // Free the allocated memory for the argument pair
    std::cout << "Thread finished for socket: " << sockfd << std::endl;
    return nullptr;
}

// Starts a new proactor and returns the proactor thread
pthread_t Proactor::startProactor(int sockfd, proactorFunc threadFunc)
{
    pthread_t tid;
    // Create a pair of the socket descriptor and the function to handle it
    auto *pair = new std::pair<int, proactorFunc>(sockfd, threadFunc);
    // Create a new thread to run the threadStart function
    if (pthread_create(&tid, nullptr, threadStart, pair) != 0)
    {
        delete pair; // Cleanup in case of error
        std::cerr << "Failed to create thread for socket: " << sockfd << std::endl;
        throw std::runtime_error("Failed to create thread");
    }
    proactorMap[tid] = threadFunc; // Store the thread function in the map
    std::cout << "Thread created with ID: " << tid << " for socket: " << sockfd << std::endl;
    return tid; // Return the thread ID
}

// Stops the proactor by thread id
int Proactor::stopProactor(pthread_t tid)
{
    // Check if the thread ID exists in the map
    if (proactorMap.find(tid) != proactorMap.end())
    {
        std::cout << "Stopping thread with ID: " << tid << std::endl;
        // Attempt to cancel the thread
        if (pthread_cancel(tid) != 0)
        {
            std::cerr << "Failed to cancel thread with ID: " << tid << std::endl;
            return -1; // Failed to cancel the thread
        }
        pthread_join(tid, nullptr); // Wait for the thread to finish
        proactorMap.erase(tid);     // Remove the thread from the map
        std::cout << "Thread with ID: " << tid << " has been stopped and removed" << std::endl;
        return 0; // Success
    }
    std::cerr << "Thread with ID: " << tid << " not found" << std::endl;
    return -1; // Thread not found
}
