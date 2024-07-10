#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <algorithm>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <mutex>

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
};

Graph::Graph(int V)
{
    this->V = V;
    adj.resize(V);
}

void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w);
}

void Graph::removeEdge(int v, int w)
{
    adj[v].erase(remove(adj[v].begin(), adj[v].end(), w), adj[v].end());
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

// Global pointer to the graph object
Graph *g = nullptr;

// Mutex to protect the shared graph object
mutex graphMutex;

// Function to handle client requests
void *handleClient(void *arg)
{
    int clientSocket = *(int *)arg;
    delete (int *)arg;

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
            close(clientSocket);
            return nullptr;
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
            lock_guard<mutex> lock(graphMutex);
            delete g;
            g = new Graph(n);
            for (int i = 0; i < m; ++i)
            {
                cout << "insert edge number " << i << endl;
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
            lock_guard<mutex> lock(graphMutex);
            if (g)
            {
                string result = g->printSCCs();
                send(clientSocket, result.c_str(), result.size(), 0);
            }
            else
            {
                send(clientSocket, "No graph created yet.\n", 22, 0);
            }
        }
        else if (command == "Newedge")
        {
            int i, j;
            iss >> i >> j;
            lock_guard<mutex> lock(graphMutex);
            if (g)
            {
                g->addEdge(i - 1, j - 1);
                send(clientSocket, "Edge added\n", 11, 0);
            }
            else
            {
                send(clientSocket, "No graph created yet.\n", 22, 0);
            }
        }
        else if (command == "Removeedge")
        {
            int i, j;
            iss >> i >> j;
            lock_guard<mutex> lock(graphMutex);
            if (g)
            {
                g->removeEdge(i - 1, j - 1);
                send(clientSocket, "Edge removed\n", 13, 0);
            }
            else
            {
                send(clientSocket, "No graph created yet.\n", 22, 0);
            }
        }
        else
        {
            send(clientSocket, "Invalid command\n", 16, 0);
        }
    }

    close(clientSocket);
    return nullptr;
}

int main()
{
    int serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("socket");
        return 1;
    }
    cout << "Server socket created: " << serverSocket << endl;
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9034);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (listen(serverSocket, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    cout << "Server is running on port 9034..." << endl;

    listen(serverSocket, 5);
    if (listen(serverSocket, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    cout << "Server is running on port 9034..." << endl;

    while (true)
    {
        clientAddrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

        pthread_t threadId;
        int *clientSocketPtr = new int(clientSocket);
        pthread_create(&threadId, nullptr, handleClient, clientSocketPtr);

        pthread_detach(threadId);
    }

    close(serverSocket);
    return 0;
}
