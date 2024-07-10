#include "reactor.hpp"
#include <iostream>
#include <unistd.h>
#include <algorithm>

Reactor::Reactor() : running(true) {}

Reactor::~Reactor()
{
    stopReactor();
}

void *Reactor::startReactor()
{
    return this;
}

int Reactor::addFdToReactor(int fd, reactorFunc func)
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pollfds.push_back(pfd);
    callbacks[fd] = func;
    return 0;
}

int Reactor::removeFdFromReactor(int fd)
{
    pollfds.erase(remove_if(pollfds.begin(), pollfds.end(),
                            [fd](const struct pollfd &pfd)
                            { return pfd.fd == fd; }),
                  pollfds.end());
    callbacks.erase(fd);
    return 0;
}

int Reactor::stopReactor()
{
    cout << "Reactor stopped" << endl;
    running = false;
    return 0;
}

/**
 * Runs the reactor, polling for incoming events and executing the corresponding
 * callback functions.
 */
void Reactor::run()
{
    running = true;
    
    // Poll for incoming events
    while (running)
    {
        cout << pollfds.size() << endl;
        int ready = poll(pollfds.data(), pollfds.size(), 1000);
        if (ready < 0)
        {
            perror("poll");
            break;
        }

        // Execute callbacks for incoming events
        for (const auto &pfd : pollfds)
        {
            if (pfd.revents & POLLIN)
            {
                // Check if callback function is registered for this fd
                if (callbacks.find(pfd.fd) != callbacks.end())
                {
                    // Execute the callback function
                    callbacks[pfd.fd](pfd.fd);
                }
            }
        }
    }
}
