#include <iostream>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

void sendCommand(int socket, const string &command)
{
    send(socket, command.c_str(), command.size(), 0);
}

string receiveResponse(int socket)
{
    char buffer[1024];
    ssize_t bytesReceived = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived < 1)
    {
        return "";
    }
    buffer[bytesReceived] = '\0';
    return string(buffer);
}

int main()
{
    cout << "Client started" << endl;
    int clientSocket;
    sockaddr_in serverAddr;

    // Create a socket
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        perror("socket");
        return 1;
    }

    // Set up the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9034);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(clientSocket);
        return 1;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("connect");
        close(clientSocket);
        return 1;
    }

    // Receive and print the initial response
    cout << receiveResponse(clientSocket) << endl;

    string line;
    while (getline(cin, line))
    {
        sendCommand(clientSocket, line + "\n");

        if (line.rfind("Newgraph", 0) == 0)
        {
            istringstream iss(line);
            string command;
            int n, m;
            if (!(iss >> command >> n >> m))
            {
                cout << "Invalid command format" << endl;
                continue;
            }
            for (int i = 0; i < m; ++i)
            {
                if (!getline(cin, line))
                {
                    cout << "Input error" << endl;
                    close(clientSocket);
                    return 1;
                }
                sendCommand(clientSocket, line + "\n");
            }
        }

        cout << receiveResponse(clientSocket) << endl;
    }

    close(clientSocket);
    return 0;
}
