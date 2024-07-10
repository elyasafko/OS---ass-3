#ifndef PROACTOR_HPP
#define PROACTOR_HPP

#include <pthread.h>
#include <map>
#include <functional>

// Define a function type for the proactor
typedef std::function<void(int)> proactorFunc;

class Proactor 
{
public:
    Proactor();
    ~Proactor();

    // Starts a new proactor and returns the proactor thread
    pthread_t startProactor(int sockfd, proactorFunc threadFunc);

    // Stops the proactor by thread id
    int stopProactor(pthread_t tid);

private:
    std::map<pthread_t, proactorFunc> proactorMap;
};

#endif // PROACTOR_HPP
