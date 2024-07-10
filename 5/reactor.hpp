#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <unordered_map>
#include <vector>
#include <functional>
#include <poll.h>

using namespace std;

using reactorFunc = function<void(int)>;

class Reactor
{
private:
    unordered_map<int, reactorFunc> callbacks;
    vector<struct pollfd> pollfds;
    bool running = false;

public:
    Reactor();
    ~Reactor();

    void *startReactor();
    int addFdToReactor(void *reactor, int fd, reactorFunc func);
    int removeFdFromReactor(void *reactor, int fd);
    int stopReactor(void *reactor);

    void run();
};

#endif // REACTOR_HPP
