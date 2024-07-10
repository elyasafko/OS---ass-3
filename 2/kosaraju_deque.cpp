#include <iostream>
#include <vector>
#include <deque>
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
    cout << v + 1 << " ";  // Adjust for 1-based output

    for (int i : adj[v])
        if (!visited[i])
            DFSUtil(i, visited);
}

void Graph::printSCCs()
{
    deque<int> Deque;

    vector<bool> visited(V, false);

    for (int i = 0; i < V; i++)
        if (visited[i] == false)
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
    int vertices, edges;
    cout << "Enter the number of vertices and edges (format: vertices edges): ";
    cin >> vertices >> edges;
    // need to validate the input

    Graph g(vertices);
    for (int i = 0; i < edges; i++)
    {
        //cout << "Enter the source and destination (1-based index): ";
        int src, dest;
        cin >> src >> dest;
        g.addEdge(src - 1, dest - 1); // Adjust for 0-based indexing
    }

    cout << "Strongly Connected Components are:\n";
    //g.printSCCs();

    return 0;
}

