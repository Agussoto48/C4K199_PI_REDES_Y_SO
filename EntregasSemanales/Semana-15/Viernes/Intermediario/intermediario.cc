/**
 * intermediario para comunicación entre cliente y servidor,
 * capaz de comunicarse con otros intermediarios
 *
 * Basado en código de Agustín Soto,
 * pero mi implementación no usa IP's quemadas
 * 
 * Basado en código de Iván Ríos para manejo de comunicación entre
 * intermediarios (con él estuve probando esto). También se hicieron pruebas
 * con Juan González.
 */
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"

#define PUERTO_INTERMEDIARIO 2026
#define BUFSIZE 4096
#define DESCUBRIMIENTO_ENVIAR 2028 // El fork va  a hacer broadcast acá y el server de figuras va a escuchar
#define DESCUBRIMIENTO_ESCUCHAR 2029 // el server de figuras escucha acá y el intermediario escucha aca
#define DESCUBRIMIENTO_INTER 2027 // entre intermediarios
#define PUERTO_SERVER 3001  // puerto fijo del servidor de figuras
#define DIR_BROADCAST "127.0.0.1"  // correr local
// #define DIR_BROADCAST "172.16.123.63" // aqui se tiene que poner el broadcast  de la isla de uno.

std::mutex mtx_server;

// ip del servidor de figuras, descubierta por UDP
std::string server_ip;
std::atomic<bool> server_found(false);
std::mutex mtx_intermediarios;
std::map<std::string, int> intermediarios_conocidos;  // ip y puerto

// escuchar para averiguar IP del servidor de figuras
void escuchar()
{
    Socket sock('d');
    int opt = 1;
    sock.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sock.Bind(DESCUBRIMIENTO_ESCUCHAR);

    char buf[128];
    struct sockaddr_in from;
    memset(&from, 0, sizeof(from));

    while (true)
    {
        memset(buf, 0, sizeof(buf));
        sock.recvFrom(buf, sizeof(buf) - 1, &from);

        int port;
        if (sscanf(buf, "FIGURAS_HERE %d", &port) != 1)
            continue;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));

        if (!server_found)
            std::cout << "[I] Serv. Figuras en " << ip << ":" << port << "\n";

        {
            std::lock_guard<std::mutex> lock(mtx_server);
            server_ip = ip;
        }
        server_found = true;

        char reply[64];
        snprintf(reply, sizeof(reply), "INTER_HERE %d", PUERTO_INTERMEDIARIO);
        struct sockaddr_in unicast;
        memset(&unicast, 0, sizeof(unicast));
        unicast.sin_family = AF_INET;
        unicast.sin_port = htons(DESCUBRIMIENTO_ENVIAR);
        unicast.sin_addr = from.sin_addr;
        sock.sendTo(reply, strlen(reply), &unicast);
    }
}

// Anunciarse como intermediario al servidor
void hacerBroadcast()
{
    Socket sock('d');
    int opt = 1;
    sock.SetSockOpt(SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));

    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(DESCUBRIMIENTO_ENVIAR);
    inet_pton(AF_INET, DIR_BROADCAST, &dest.sin_addr);

    char msg[64];
    snprintf(msg, sizeof(msg), "INTER_HERE %d", PUERTO_INTERMEDIARIO);

    while (!server_found)
    {
        sock.sendTo(msg, strlen(msg), &dest);
        std::cout << "[I] Buscando Serv. Fig. (brd en "
            << DESCUBRIMIENTO_ENVIAR << ")" << "\n";
        sleep(2);
    }
}

// esto es para escuchar a otros servidore sintermediarios
void escucharIntermediarios()
{
    Socket sock('d');
    int opt = 1;
    sock.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sock.Bind(DESCUBRIMIENTO_INTER);

    char buf[128];
    struct sockaddr_in from;
    memset(&from, 0, sizeof(from));

    while (true)
    {
        memset(buf, 0, sizeof(buf));
        sock.recvFrom(buf, sizeof(buf) - 1, &from);

        int port;
        if (sscanf(buf, "INTER_HERE %d", &port) != 1)
            continue;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));

        std::lock_guard<std::mutex> lock(mtx_intermediarios);
        if (intermediarios_conocidos.count(ip) == 0)
        {
            intermediarios_conocidos[ip] = port;
            std::cout << "[I] Nuevo intermediario: " << ip << ":" << port << "\n";
        }
    }
}

// esto es para anunciarse a otros intermediarios periódicamente
void anunciarToForks()
{
    const char* broadcasts[] = {
        // "172.16.123.15", // isla 1
        // "172.16.123.31", // isla 2
        // "172.16.123.47", // isla 3
        // "172.16.123.63", // isla 4
        // "172.16.123.79", // isla 5
        // "172.16.123.95", // isla 6
        "192.168.1.255", // prueba local
    };
    const int n = sizeof(broadcasts) / sizeof(broadcasts[0]);

    Socket sock('d');
    int opt = 1;
    sock.SetSockOpt(SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));

    char msg[64];
    snprintf(msg, sizeof(msg), "INTER_HERE %d", PUERTO_INTERMEDIARIO);

    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(DESCUBRIMIENTO_INTER);

    while (true)
    {
        for (int i = 0; i < n; ++i)
        {
            inet_pton(AF_INET, broadcasts[i], &dest.sin_addr);
            sock.sendTo(msg, strlen(msg), &dest);
        }
        sleep(5);
    }
}


bool probarFiguras(const char* request, size_t bytesRead, VSocket* client, bool forwarded)
{
    if (!server_found)
        return false;

    std::string host;
    {
        std::lock_guard<std::mutex> lock(mtx_server);
        host = server_ip;
    }

    char response[BUFSIZE];
    VSocket *server = new Socket('s');
    size_t bytes = 0;
    try
    {
        server->Connect(host.c_str(), PUERTO_SERVER);
        server->Write(request, bytesRead);
        bytes = server->Read(response, BUFSIZE - 1);
    }
    catch (...) {}
    server->Close();
    delete server;

    if (bytes > 0)
    {
        response[bytes] = '\0';
        if (strstr(response, "404 Not Found") == nullptr || forwarded)
        {
            client->Write(response, bytes);
            std::cout << "[I] Respuesta del servidor local\n";
            return true;
        }
    }
    return false;
}

bool probarFork(const char* request, size_t bytesRead, VSocket* client)
{
    std::map<std::string, int> copia;
    {
        std::lock_guard<std::mutex> lock(mtx_intermediarios);
        copia = intermediarios_conocidos;
    }

    std::string reenvio(request, bytesRead);
    size_t pos = reenvio.find("\r\n\r\n");
    if (pos != std::string::npos) {
        reenvio.insert(pos, "\r\nVia: intermediario"); // para evitar que ande dando vueltas
    }
    char response[BUFSIZE];

    for (auto it = copia.begin(); it != copia.end(); ++it)
    {
        const std::string& ip = it->first;
        int port = it->second;
        memset(response, 0, BUFSIZE);
        VSocket *inter = new Socket('s');
        size_t bytes = 0;
        try
        {
            inter->Connect(ip.c_str(), port);
            inter->Write(reenvio.c_str(), reenvio.size());
            bytes = inter->Read(response, BUFSIZE - 1);
        }
        catch (...) {}
        inter->Close();
        delete inter;

        if (bytes > 0)
        {
            response[bytes] = '\0';
            if (strstr(response, "404 Not Found") == nullptr)
            {
                client->Write(response, bytes);
                std::cout << "[I] Figura de " << ip << ":" << port << "\n";
                return true;
            }
        }
    }
    return false;
}

void task(VSocket *client)
{
    char request[BUFSIZE];
    memset(request, 0, BUFSIZE);

    size_t bytesRead = client->Read(request, BUFSIZE - 1);
    if (bytesRead <= 0)
    {
        client->Close();
        delete client;
        return;
    }
    request[bytesRead] = '\0';
    std::cout << "[I] Solicitud recibida\n";

    bool forwarded = strstr(request, "Via: intermediario") != nullptr;
    // primero se prueba con el servidor de figuras
    bool respondido = probarFiguras(request, bytesRead, client, forwarded);
    // si no se logra, se prueba con los otros intermediarios
    if (!respondido && !forwarded)
        respondido = probarFork(request, bytesRead, client);

    // esto lo tengo que probar
    if (!respondido)
    {
        const char *err =
            "HTTP/1.0 404 Not Found\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Figura no encontrada en ningún intermediario.\n";
        client->Write(err, strlen(err));
    }

    client->Close();
    delete client;
}

int main()
{
    std::thread(escuchar).detach();
    std::thread(hacerBroadcast).detach();
    std::thread(escucharIntermediarios).detach();
    std::thread(anunciarToForks).detach();

    VSocket *s1 = new Socket('s');
    s1->Bind(PUERTO_INTERMEDIARIO);
    s1->MarkPassive(5);

    std::cout << "Intermediario escuchando puerto "
        << PUERTO_INTERMEDIARIO << "\n";

    while (true)
    {
        VSocket *client = s1->AcceptConnection();
        std::cout << "[I] Cliente conectado" << "\n";
        std::thread(task, client).detach();
    }

    delete s1;
    return 0;
}
