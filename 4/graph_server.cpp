#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <algorithm>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

// Graph class to represent a directed graph using adjacency list representation
class Graph
{
    int V;                   // Number of vertices
    vector<vector<int>> adj; // Adjacency list

    // Function to fill the order of vertices
    void fillOrder(int v, vector<bool> &visited, deque<int> &Deque);
    // Function to perform DFS traversal
    void DFSUtil(int v, vector<bool> &visited, stringstream &out);

public:
    Graph(int V);                  // Constructor
    void addEdge(int v, int w);    // Add an edge to the graph
    void removeEdge(int v, int w); // Remove an edge from the graph
    string printSCCs();            // Print Strongly Connected Components
    Graph getTranspose();          // Get the transpose of the graph
};

// Constructor
Graph::Graph(int V)
{
    this->V = V;
    adj.resize(V);
}

// Add an edge to the graph
void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w);
}

// Remove an edge from the graph
void Graph::removeEdge(int v, int w)
{
    adj[v].erase(remove(adj[v].begin(), adj[v].end(), w), adj[v].end());
}

// Fill the order of vertices for SCC
void Graph::fillOrder(int v, vector<bool> &visited, deque<int> &Deque)
{
    visited[v] = true;
    for (int i : adj[v])
        if (!visited[i])
            fillOrder(i, visited, Deque);
    Deque.push_back(v);
}

// Get the transpose of the graph
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

// DFS traversal
void Graph::DFSUtil(int v, vector<bool> &visited, stringstream &out)
{
    visited[v] = true;
    out << v + 1 << " ";
    for (int i : adj[v])
        if (!visited[i])
            DFSUtil(i, visited, out);
}

// Print Strongly Connected Components
string Graph::printSCCs()
{
    deque<int> Deque;
    vector<bool> visited(V, false);
    stringstream result;

    // Fill vertices in stack according to their finishing times
    for (int i = 0; i < V; i++)
        if (!visited[i])
            fillOrder(i, visited, Deque);

    // Create a reversed graph
    Graph gr = getTranspose();

    // Mark all the vertices as not visited (For second DFS)
    fill(visited.begin(), visited.end(), false);

    // Process all vertices in order defined by Stack
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

// Function to handle client requests
void *handleClient(void *arg)
{
    int clientSocket = *(int *)arg;
    delete (int *)arg;

    char buffer[1024];
    string line;

    // Send instructions to the client
    string instructions = "Please insert one of the following commands:\n"
                          "Newgraph <n> <m> - Create a new graph with n vertices and m edges\n"
                          "Kosaraju - Print SCCs of the graph\n"
                          "Newedge <i> <j> - Add edge from vertex i to vertex j\n"
                          "Removeedge <i> <j> - Remove edge from vertex i to vertex j\n";
    send(clientSocket, instructions.c_str(), instructions.size(), 0);

    while (true)
    {
        // Receive command from client
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
            delete g;
            g = new Graph(n);
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

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Create a socket
    //serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9034);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address and port
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Start listening for incoming connections
    listen(serverSocket, 5);

    cout << "Server is running on port 9034..." << endl;

    while (true)
    {
        clientAddrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

        pthread_t threadId;
        int *clientSocketPtr = new int(clientSocket);

        // Create a new thread to handle the client
        pthread_create(&threadId, nullptr, handleClient, clientSocketPtr);

        // Detach the thread to allow it to run independently
        pthread_detach(threadId);
    }

    // Close the server socket
    close(serverSocket);
    return 0;
}
