#include "reactor.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <deque>
#include <algorithm>

const int PORT = 9034;
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

Graph *g = nullptr; // Global pointer to the graph object

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

void handleClient(int client_fd)
{
    char buffer[1024];
    string line;

    // Send instructions to the client
    string instructions = "Please insert one of the following commands:\n"
                          "Newgraph <n> <m> - Create a new graph with n vertices and m edges\n"
                          "Kosaraju - Print SCCs of the graph\n"
                          "Newedge <i> <j> - Add edge from vertex i to vertex j\n"
                          "Removeedge <i> <j> - Remove edge from vertex i to vertex j\n";
    send(client_fd, instructions.c_str(), instructions.size(), 0);
    std::cout << "Sent instructions to client_fd: " << client_fd << endl;

    ssize_t bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0)
    {
        if (bytesReceived == 0)
            cout << "Client disconnected, client_fd: " << client_fd << endl;
        else
            perror("recv");
        close(client_fd);
        return;
    }

    buffer[bytesReceived] = '\0';
    line = buffer;
    cout << "Received from client_fd " << client_fd << ": " << line << endl;

    istringstream iss(line);
    string command;
    iss >> command;

    if (command.rfind("Newgraph", 0) == 0)
    {
        // Split the command string into tokens
        istringstream iss(line);
        vector<int> tokens;
        int token;
        string token_str;
        // Skip the "Newgraph" command part
        iss >> token_str;

        // Insert remaining numbers into the tokens vector
        while (iss)
        {
            iss >> token;
            if (iss)
            {
                tokens.push_back(token);
            }
        }
        // Ensure there are enough tokens for the command
        if (tokens.size() < 2) // n, m and at least one edge pair
        {
            send(client_fd, "Invalid input format, please try again\n", 14, 0);
            return;
        }

        // Parse n and m
        int n = tokens[0];
        int m = tokens[1];


        // Calculate the expected number of tokens: n + m + 2 * m (pairs of edges)
        size_t expectedTokens = 2 + 2 * m;
        if (tokens.size() != expectedTokens)
        {
            send(client_fd, "Mismatch in number of edges\n", 28, 0);
            return;
        }

        // Delete the old graph and create a new one
        delete g;
        g = new Graph(n);
        cout << "Creating new graph with " << n << " vertices and " << m << " edges." << endl;

        // Parse and add edges
        for (int i = 0; i < m; ++i)
        {
            int src = tokens[2 + 2 * i];
            int dest = tokens[3 + 2 * i];
            g->addEdge(src - 1, dest - 1);
        }
        cout << "Added edges." << endl;
        send(client_fd, "Created new graph\n", 18, 0);
    }

    else if (command == "Kosaraju")
    {
        if (g)
        {
            string result = g->printSCCs();
            send(client_fd, result.c_str(), result.size(), 0);
            cout << "Sent SCCs to client_fd: " << client_fd << endl;
        }
        else
        {
            send(client_fd, "No graph created yet.\n", 22, 0);
            cout << "No graph created yet for client_fd: " << client_fd << endl;
        }
    }
    else if (command == "Newedge")
    {
        int i, j;
        iss >> i >> j;
        if (g)
        {
            g->addEdge(i - 1, j - 1);
            send(client_fd, "Edge added\n", 11, 0);
            cout << "Added edge from " << i << " to " << j << " for client_fd: " << client_fd << endl;
        }
        else
        {
            send(client_fd, "No graph created yet.\n", 22, 0);
            cout << "No graph created yet for client_fd: " << client_fd << endl;
        }
    }
    else if (command == "Removeedge")
    {
        int i, j;
        iss >> i >> j;
        if (g)
        {
            g->removeEdge(i - 1, j - 1);
            send(client_fd, "Edge removed\n", 13, 0);
            cout << "Removed edge from " << i << " to " << j << " for client_fd: " << client_fd << endl;
        }
        else
        {
            send(client_fd, "No graph created yet.\n", 22, 0);
            cout << "No graph created yet for client_fd: " << client_fd << endl;
        }
    }
    else
    {
        send(client_fd, "Invalid command\n", 16, 0);
        cout << "Invalid command received from client_fd: " << client_fd << endl;
    }
}

int main()
{
    cout << "Server listening on port " << PORT << endl;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }
    cout << "Server socket created: " << server_fd << endl;
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(server_fd);
        return 1;
    }
    cout << "setsockopt for SO_REUSEADDR" << endl;
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }
    cout << "bind successful" << endl;
    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        close(server_fd);
        return 1;
    }
    cout << "listen successful" << endl;
    Reactor reactor;
    reactor.addFdToReactor(server_fd, [&](int fd)
                           {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(fd, (sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            return;
        }

        cout << "New connection, client_fd: " << client_fd << endl;

        reactor.addFdToReactor(client_fd, handleClient); });

    cout << "Reactor started" << endl;
    reactor.run();
    cout << "Reactor stopped" << endl;

    close(server_fd);
    return 0;
}