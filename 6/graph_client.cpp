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
    cout << "Waiting for response..." << endl;
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

    try
    {
        clientSocket = socket(PF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0)
        {
            cerr << "Failed to create client socket" << endl;
            return 1;
        }
        cout << "Client socket created: " << clientSocket << endl;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(9034);
        if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0)
        {
            cerr << "Invalid address" << endl;
            close(clientSocket);
            return 1;
        }
        cout << "Connecting to server" << endl;
        if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            cerr << "Failed to connect to server" << endl;
            close(clientSocket);
            return 1;
        }
        cout << "Connected to server" << endl;
        string response = receiveResponse(clientSocket);
        if (response.empty())
        {
            cerr << "Failed to receive response from server" << endl;
            close(clientSocket);
            return 1;
        }
        cout << "response: " << response << endl;

        string line;
        while (getline(cin, line))
        {
            sendCommand(clientSocket, line + "\n");

            response = receiveResponse(clientSocket);
            if (response.empty())
            {
                cerr << "Failed to receive response from server" << endl;
                close(clientSocket);
                return 1;
            }
            cout << response << endl;
        }
    }
    catch (exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    close(clientSocket);
    return 0;
}