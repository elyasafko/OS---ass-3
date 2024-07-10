#include "proactor.hpp"
#include <stdexcept>

Proactor::Proactor() {}

Proactor::~Proactor() {}

// Thread starter function
void *threadStart(void *arg)
{
    auto *pair = static_cast<std::pair<int, proactorFunc> *>(arg);
    int sockfd = pair->first;
    proactorFunc func = pair->second;
    func(sockfd);
    delete pair; // Free the allocated memory
    return nullptr;
}

// Starts a new proactor and returns the proactor thread
pthread_t Proactor::startProactor(int sockfd, proactorFunc threadFunc)
{
    pthread_t tid;
    auto *pair = new std::pair<int, proactorFunc>(sockfd, threadFunc);
    if (pthread_create(&tid, nullptr, threadStart, pair) != 0)
    {
        delete pair; // Cleanup in case of error
        throw std::runtime_error("Failed to create thread");
    }
    proactorMap[tid] = threadFunc;
    return tid;
}

// Stops the proactor by thread id
int Proactor::stopProactor(pthread_t tid)
{
    if (proactorMap.find(tid) != proactorMap.end())
    {
        if (pthread_cancel(tid) != 0)
        {
            return -1; // Failed to cancel the thread
        }
        pthread_join(tid, nullptr); // Wait for the thread to finish
        proactorMap.erase(tid);
        return 0; // Success
    }
    return -1; // Thread not found
}
