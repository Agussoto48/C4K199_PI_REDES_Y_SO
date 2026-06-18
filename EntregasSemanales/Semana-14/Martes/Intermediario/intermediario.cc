#include <iostream>
#include <thread>
#include <mutex>
#include <cstring>

#include "Socket.h"

#define PORT_INTERMEDIARIO 8080
#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 3001
#define BUFSIZE 4096

void task(VSocket *client)
{
    char request[BUFSIZE];
    char response[BUFSIZE];

    memset(request, 0, BUFSIZE);
    memset(response, 0, BUFSIZE);

    size_t bytesRead = client->Read(request, BUFSIZE - 1);
    std::cout << "[Intermediario] Solicitud recibida:" << std::endl << request << std::endl;

    if (bytesRead <= 0)
    {
        client->Close();
        delete client;
        return;
    }

    request[bytesRead] = '\0';

    VSocket *server = new Socket('s');

    server->Connect(SERVER_HOST, SERVER_PORT);
    server->Write(request, strlen(request));

    size_t serverBytes = server->Read(response, BUFSIZE - 1);

    if (serverBytes > 0)
    {
        std::cout << "[Intermediario] Respuesta recibida del servidor" << std::endl;
        response[serverBytes] = '\0';
        client->Write(response, serverBytes);
    }
    else
    {
        const char *error =
            "HTTP/1.0 500 Internal Server Error\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Error: el intermediario no recibio respuesta del servidor.\n";

        client->Write(error, strlen(error));
    }

    server->Close();
    client->Close();

    delete server;
    delete client;
}

int main()
{
    VSocket *s1;
    VSocket *client;
    std::thread *worker;

    s1 = new Socket('s');

    s1->Bind(PORT_INTERMEDIARIO);
    s1->MarkPassive(5);

    std::cout << "Intermediario escuchando en puerto "
              << PORT_INTERMEDIARIO << std::endl;

    for (;;)
    {
        client = s1->AcceptConnection();
        std::cout << "[Intermediario] Cliente conectado" << std::endl;
        worker = new std::thread(task, client);
        worker->detach();
    }

    delete s1;
    return 0;
}