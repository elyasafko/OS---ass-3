#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <algorithm>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <queue>
#include "proactor.hpp"

using namespace std;

// Graph class to represent a directed graph using adjacency list representation
class Graph
{
    int V;                   // Number of vertices
    vector<vector<int>> adj; // Adjacency list

    void fillOrder(int v, vector<bool> &visited, deque<int> &Deque);
    void DFSUtil(int v, vector<bool> &visited, stringstream &out);

public:
    Graph(int V);
    void addEdge(int v, int w);
    void removeEdge(int v, int w);
    string printSCCs();
    Graph getTranspose();
    bool isLargeSCC();
};

Graph::Graph(int V)
{
    this->V = V;
    adj.resize(V);
}

void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w);
    cout << "Edge added from " << v + 1 << " to " << w + 1 << endl;
}

void Graph::removeEdge(int v, int w)
{
    adj[v].erase(remove(adj[v].begin(), adj[v].end(), w), adj[v].end());
    cout << "Edge removed from " << v + 1 << " to " << w + 1 << endl;
}

void Graph::fillOrder(int v, vector<bool> &visited, deque<int> &Deque)
{
    visited[v] = true;
    for (int i : adj[v])
        if (!visited[i])
            fillOrder(i, visited, Deque);
    Deque.push_back(v);
}

Graph Graph::getTranspose()
{
    Graph g(V);
    for (int v = 0; v < V; v++)
    {
        for (int i : adj[v])
            g.adj[i].push_back(v);
    }
    return g;
}

void Graph::DFSUtil(int v, vector<bool> &visited, stringstream &out)
{
    visited[v] = true;
    out << v + 1 << " ";
    for (int i : adj[v])
        if (!visited[i])
            DFSUtil(i, visited, out);
}

string Graph::printSCCs()
{
    deque<int> Deque;
    vector<bool> visited(V, false);
    stringstream result;

    for (int i = 0; i < V; i++)
        if (!visited[i])
            fillOrder(i, visited, Deque);

    Graph gr = getTranspose();

    fill(visited.begin(), visited.end(), false);

    while (!Deque.empty())
    {
        int v = Deque.back();
        Deque.pop_back();
        if (!visited[v])
        {
            gr.DFSUtil(v, visited, result);
            result << endl;
        }
    }

    return result.str();
}

bool Graph::isLargeSCC()
{
    deque<int> Deque;
    vector<bool> visited(V, false);

    // Fill vertices in stack according to their finishing times
    for (int i = 0; i < V; i++)
        if (!visited[i])
            fillOrder(i, visited, Deque);

    // Create a reversed graph
    Graph gr = getTranspose();

    // Mark all the vertices as not visited (For second DFS)
    fill(visited.begin(), visited.end(), false);

    // Process all vertices in order defined by Deque
    while (!Deque.empty())
    {
        int v = Deque.back();
        Deque.pop_back();
        if (!visited[v])
        {
            stringstream scc;
            gr.DFSUtil(v, visited, scc);

            // Convert stringstream to a vector of nodes
            vector<int> sccNodes;
            int node;
            while (scc >> node)
            {
                sccNodes.push_back(node);
            }
            // Check if SCC contains at least 50% of nodes
            size_t check = V / 2;
            if (sccNodes.size() >= check)
            {
                return true;
            }
        }
    }

    return false;
}

// Global pointer to the graph object
Graph *g = nullptr;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
queue<bool> notificationQueue;
bool done = false;

void *consumer(void *arg)
{
    while (!done)
    {
        pthread_mutex_lock(&mtx);
        while (notificationQueue.empty() && !done)
        {
            pthread_cond_wait(&cv, &mtx);
        }
        if (!notificationQueue.empty())
        {
            bool conditionMet = notificationQueue.front();
            notificationQueue.pop();
            pthread_mutex_unlock(&mtx);

            if (conditionMet)
            {
                cout << "At least 50% of the graph belongs to the same SCC\n";
            }
            else
            {
                cout << "At least 50% of the graph no longer belongs to the same SCC\n";
            }
        }
        else
        {
            pthread_mutex_unlock(&mtx);
        }
    }
    return nullptr;
}

// Function to handle client requests
void handleClient(int clientSocket)
{
    char buffer[1024];
    string line;

    string instructions = "Please insert one of the following commands:\n"
                          "Newgraph <n> <m> - Create a new graph with n vertices and m edges\n"
                          "Kosaraju - Print SCCs of the graph\n"
                          "Newedge <i> <j> - Add edge from vertex i to vertex j\n"
                          "Removeedge <i> <j> - Remove edge from vertex i to vertex j\n";
    send(clientSocket, instructions.c_str(), instructions.size(), 0);

    while (true)
    {
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived < 1)
        {
            cout << "Client disconnected." << endl;
            close(clientSocket);
            return;
        }
        buffer[bytesReceived] = '\0';
        line = buffer;

        istringstream iss(line);
        string command;
        iss >> command;

        if (command == "Newgraph")
        {
            int n, m;
            iss >> n >> m;
            pthread_mutex_lock(&mtx);
            delete g;
            g = new Graph(n);
            pthread_mutex_unlock(&mtx);
            cout << "New graph created with " << n << " vertices." << endl;
            for (int i = 0; i < m; ++i)
            {
                int src, dest;
                recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                istringstream edgeStream(buffer);
                edgeStream >> src >> dest;
                g->addEdge(src - 1, dest - 1);
            }
            send(clientSocket, "Created new graph\n", 18, 0);
        }
        else if (command == "Kosaraju")
        {
            pthread_mutex_lock(&mtx);
            if (g)
            {
                string result = g->printSCCs();

                // Check and notify about large SCC
                bool conditionMet = g->isLargeSCC();
                notificationQueue.push(conditionMet);
                pthread_cond_signal(&cv);

                pthread_mutex_unlock(&mtx);
                cout << "Kosaraju's algorithm executed." << endl;
                send(clientSocket, result.c_str(), result.size(), 0);
            }
            else
            {
                pthread_mutex_unlock(&mtx);
                send(clientSocket, "No graph created yet.\n", 22, 0);
            }
        }
        else if (command == "Newedge")
        {
            int i, j;
            iss >> i >> j;
            pthread_mutex_lock(&mtx);
            if (g)
            {
                g->addEdge(i - 1, j - 1);
                pthread_mutex_unlock(&mtx);
                send(clientSocket, "Edge added\n", 11, 0);
            }
            else
            {
                pthread_mutex_unlock(&mtx);
                send(clientSocket, "No graph created yet.\n", 22, 0);
            }
        }
        else if (command == "Removeedge")
        {
            int i, j;
            iss >> i >> j;
            pthread_mutex_lock(&mtx);
            if (g)
            {
                g->removeEdge(i - 1, j - 1);
                pthread_mutex_unlock(&mtx);
                send(clientSocket, "Edge removed\n", 13, 0);
            }
            else
            {
                pthread_mutex_unlock(&mtx);
                send(clientSocket, "No graph created yet.\n", 22, 0);
            }
        }
        else
        {
            send(clientSocket, "Invalid command\n", 16, 0);
        }
    }

    close(clientSocket);
}

int main()
{
    int serverSocket;
    sockaddr_in serverAddr;

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("socket");
        return 1;
    }
    cout << "Server socket created: " << serverSocket << endl;

    // Set socket options
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        close(serverSocket);
        return 1;
    }
    cout << "setsockopt for SO_REUSEADDR" << endl;

    // Define server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9034);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("bind");
        close(serverSocket);
        return 1;
    }
    cout << "bind successful" << endl;

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0)
    {
        perror("listen");
        close(serverSocket);
        return 1;
    }

    cout << "Server is running on port 9034..." << endl;

    Proactor proactor;

    pthread_t consumerThread;
    pthread_create(&consumerThread, nullptr, consumer, nullptr);

    while (true)
    {
        int clientSocket;
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);

        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
        if (clientSocket < 0)
        {
            perror("accept");
            continue;
        }

        try
        {
            proactor.startProactor(clientSocket, handleClient);
        }
        catch (const std::exception &e)
        {
            cerr << "Failed to create thread: " << e.what() << endl;
            close(clientSocket);
        }
    }

    done = true;
    pthread_cond_broadcast(&cv);
    pthread_join(consumerThread, nullptr);

    close(serverSocket);
    return 0;
}
