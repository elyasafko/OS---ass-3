#include "reactor.hpp"
#include <iostream>
#include <unistd.h>
#include <algorithm>

Reactor::Reactor() {}

Reactor::~Reactor()
{
    stopReactor(this);
}

void *Reactor::startReactor()
{
    return this;
}

int Reactor::addFdToReactor(void *reactor_ptr, int fd, reactorFunc func)
{
    Reactor *reactor = static_cast<Reactor *>(reactor_ptr);
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    reactor->pollfds.push_back(pfd);
    reactor->callbacks[fd] = func;
    return 0;
}

int Reactor::removeFdFromReactor(void *reactor_ptr, int fd)
{
    Reactor *reactor = static_cast<Reactor *>(reactor_ptr);
    reactor->pollfds.erase(std::remove_if(reactor->pollfds.begin(), reactor->pollfds.end(),
                                          [fd](const struct pollfd &pfd)
                                          { return pfd.fd == fd; }),
                           reactor->pollfds.end());
    reactor->callbacks.erase(fd);
    return 0;
}

int Reactor::stopReactor(void *reactor_ptr)
{
    Reactor *reactor = static_cast<Reactor *>(reactor_ptr);
    reactor->running = false;
    return 0;
}

void Reactor::run()
{
    while (running)
    {
        int ready = poll(pollfds.data(), pollfds.size(), 100);
        if (ready < 0)
        {
            perror("poll");
            break;
        }

        for (const auto &pfd : pollfds)
        {
            if (pfd.revents & POLLIN)
            {
                if (callbacks.find(pfd.fd) != callbacks.end())
                {
                    callbacks[pfd.fd](pfd.fd);
                }
            }
        }
    }
}
