#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

class Graph
{
    int V;                   // Number of vertices
    vector<vector<int>> adj; // Adjacency list

    void fillOrder(int v, vector<bool> &visited, deque<int> &Deque);
    void DFSUtil(int v, vector<bool> &visited);

public:
    Graph(int V);
    void addEdge(int v, int w);
    void removeEdge(int v, int w);
    void printSCCs();
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

void Graph::DFSUtil(int v, vector<bool> &visited)
{
    visited[v] = true;
    cout << v + 1 << " ";
    for (int i : adj[v])
        if (!visited[i])
            DFSUtil(i, visited);
}

void Graph::printSCCs()
{
    deque<int> Deque;
    vector<bool> visited(V, false);
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
            gr.DFSUtil(v, visited);
            cout << endl;
        }
    }
}

int main()
{
    Graph *g = nullptr;
    string line;

    cout << "Please insert one of the following commands:" << endl;
    cout << "Newgraph <n> <m> - Create a new graph with n vertices and m edges" << endl;
    cout << "Kosaraju - Print SCCs of the graph" << endl;
    cout << "Newedge <i> <j> - Add edge from vertex i to vertex j" << endl;
    cout << "Removeedge <i> <j> - Remove edge from vertex i to vertex j" << endl;

    while (getline(cin, line))
    {
        istringstream iss(line);
        string command;
        iss >> command;

        if (command == "Newgraph")
        {
            int n, m;
            iss >> n >> m;
            g = new Graph(n);
            cout << "Created new graph with " << n << " vertices and " << m << " edges" << endl;

            for (int i = 0; i < m; ++i)
            {
                int src, dest;
                cin >> src >> dest;
                g->addEdge(src - 1, dest - 1); // Adjust for 0-based indexing
                cout << "Added edge from " << src << " to " << dest << endl;
            }
        }
        else if (command == "Kosaraju")
        {
            if (g)
            {
                cout << "Running Kosaraju's algorithm..." << endl;
                g->printSCCs();
            }
            else
            {
                cout << "No graph created yet." << endl;
            }
        }
        else if (command == "Newedge")
        {
            int i, j;
            iss >> i >> j;
            if (g)
            {
                g->addEdge(i - 1, j - 1); // Adjust for 0-based indexing
                cout << "Added edge from " << i << " to " << j << endl;
            }
            else
            {
                cout << "No graph created yet." << endl;
            }
        }
        else if (command == "Removeedge")
        {
            int i, j;
            iss >> i >> j;
            if (g)
            {
                g->removeEdge(i - 1, j - 1); // Adjust for 0-based indexing
                cout << "Removed edge from " << i << " to " << j << endl;
            }
            else
            {
                cout << "No graph created yet." << endl;
            }
        }
        else
        {
            cout << "Invalid command" << endl;
        }
    }

    delete g;
    return 0;
}
