/**
 * intermediario para comunicación entre cliente y servidor,
 * capaz de comunicarse con otros intermediarios
 *
 * Se colaboró con Jefferson Marín, Brandon Palacios e Iván Ríos en la
 * implementación de los intermediarios
 *
 */
#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"

#define PUERTO_FORK 2026
#define BUFSIZE 4096
#define DESC_ENVIAR 2028 // El fork va  a hacer broadcast acá y el server de figuras va a escucharFiguras
#define DESC_ESCUCHAR 2029 // el server de figuras escucha acá y el intermediario escucha aca
#define PUERTO_SERVER 3001  // puerto fijo del servidor de figuras
#define DIR_BROADCAST "172.16.123.63" // aqui se tiene que poner el broadcast  de la isla de uno.

char* broadcast_islas[] = {
    (char*)"172.16.123.15",
    (char*)"172.16.123.31",
    (char*)"172.16.123.47",
    (char*)"172.16.123.63", // isla 3
    (char*)"172.16.123.79",
    (char*)"172.16.123.95",
};
int num_islas = 6;

#define PUERTO_JOIN_UDP 3030
#define PUERTO_INTER_TCP 3031

const uint8_t TP_JOIN = 0;
const uint8_t TP_HANDSHAKE = 1;
const uint8_t TP_IR_REQUEST = 2;
const uint8_t TP_IR_RESPONSE = 3;
const uint8_t TP_NOT_FOUND = 4;

std::string local_ip;

std::mutex mtx_server;
std::string server_ip;
std::atomic<bool> server_found(false);

// tabla con los vecinos descubiertos. basado en Jefferson
struct ForkVecino {
    std::string ip;
    std::vector<std::string> figuras;
};
std::mutex mtx_vecinos_fork;
std::vector<ForkVecino> vecinos_fork;


 // escucharFiguras para averiguar IP del servidor de figuras
void escucharFiguras()
{
    Socket sock('d');
    int opt = 1;
    sock.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sock.Bind(DESC_ESCUCHAR);

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
        snprintf(reply, sizeof(reply), "INTER_HERE %d", PUERTO_FORK);
        struct sockaddr_in unicast;
        memset(&unicast, 0, sizeof(unicast));
        unicast.sin_family = AF_INET;
        unicast.sin_port = htons(DESC_ENVIAR);
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
    dest.sin_port = htons(DESC_ENVIAR);
    inet_pton(AF_INET, DIR_BROADCAST, &dest.sin_addr);

    char msg[64];
    snprintf(msg, sizeof(msg), "INTER_HERE %d", PUERTO_FORK);

    while (!server_found)
    {
        sock.sendTo(msg, strlen(msg), &dest);
        std::cout << "[I] Buscando Serv. Fig. (brd en " << DESC_ENVIAR << ")" << "\n";
        sleep(2);
    }
}

 // Cnectarse al servidor de figuras de la isla de nosotros y pedirle la lista
std::string getLegosPropios()
{
    std::string ip;
    {
        std::lock_guard<std::mutex> lock(mtx_server);
        ip = server_ip;
    }
    if (ip.empty()) return "";

    char buf[BUFSIZE];
    VSocket* s = new Socket('s');
    size_t bytes = 0;
    try {
        s->Connect(ip.c_str(), PUERTO_SERVER);
        std::string getReq = "GET /list HTTP/1.1\r\nHost: " + ip + "\r\n\r\n";
        s->Write(getReq.c_str(), getReq.size());
        bytes = s->Read(buf, BUFSIZE - 1);
    } catch (...) {}
    s->Close();
    delete s;
    if (bytes == 0) return "";
    buf[bytes] = '\0';

    char* body = strstr(buf, "\r\n\r\n");
    if (!body) return "";
    body += 4;

    // armar CSV a partir de las líneas del cuerpo
    std::string csv;
    std::istringstream ss(body);
    std::string linea;
    while (std::getline(ss, linea)) {
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.empty()) continue;
        if (!csv.empty()) csv += ',';
        csv += linea;
    }
    return csv;
}

 // guardar y actualizar figruas de intermediarios vecinos
void guardarFigsVecino(const std::string& forkIp, const std::string& csv)
{
    std::vector<std::string> figs;
    std::istringstream ss(csv);
    std::string token;
    while (std::getline(ss, token, ',')) {
        if (!token.empty()) {
            figs.push_back(token);
        }
    }
    std::lock_guard<std::mutex> lock(mtx_vecinos_fork);
    for (auto& v : vecinos_fork)
        if (v.ip == forkIp) {
            v.figuras = figs;
            return;
        }
    vecinos_fork.push_back({forkIp, figs});
    std::cout << "[I] Isla vecina" << forkIp << " con " << figs.size() << " figuras\n";
}

// maneja conexiones con otros intermediarios
void manejarConexionTP(int fd, const std::string& forkIp)
{
    uint8_t tipo;
    if (read(fd, &tipo, 1) <= 0) return;

    if (tipo == TP_HANDSHAKE) {

        uint32_t len;
        if (read(fd, &len, 4) <= 0) return;

        std::string datos(len, '\0');
        if (len > 0) read(fd, &datos[0], len);
        guardarFigsVecino(forkIp, datos);

        std::string csv = getLegosPropios();
        uint8_t t = TP_HANDSHAKE;
        uint32_t rlen = (uint32_t)csv.size();
        write(fd, &t, 1);
        write(fd, &rlen, 4);
        if (!csv.empty()) write(fd, csv.c_str(), csv.size());

    } else if (tipo == TP_IR_REQUEST) {
        uint8_t mitad, nlen;
        if (read(fd, &mitad, 1) <= 0 || read(fd, &nlen, 1) <= 0) return;

        std::string figura(nlen, '\0');
        if (nlen > 0) read(fd, &figura[0], nlen);

        std::string ip;
        { std::lock_guard<std::mutex> lk(mtx_server); ip = server_ip; }

        std::string contenido;
        if (!ip.empty()) {
            char req[256];
            snprintf(req, sizeof(req), "GET /figura/%s/%d HTTP/1.1\r\nHost: %s\r\n\r\n",
                     figura.c_str(), (int)mitad, ip.c_str());
            char buf[BUFSIZE];
            size_t n = 0;
            VSocket* s = new Socket('s');
            try {
                s->Connect(ip.c_str(), PUERTO_SERVER);
                s->Write(req, strlen(req));
                n = s->Read(buf, BUFSIZE - 1);
            } catch (...) {}
            s->Close();
            delete s;

            if (n > 0) {
                buf[n] = '\0';
                const char* body = strstr(buf, "\r\n\r\n");
                if (body && !strstr(buf, "404"))
                    contenido = std::string(body + 4, buf + n - body - 4);
            }
        }

        if (contenido.empty()) {
            uint8_t nf = TP_NOT_FOUND;
            uint8_t flen = figura.size();
            write(fd, &nf, 1);
            write(fd, &flen, 1);
            write(fd, figura.c_str(), figura.size());
        } else {
            uint8_t t = TP_IR_RESPONSE;
            uint8_t flen = figura.size();
            uint32_t clen = (uint32_t)contenido.size();
            write(fd, &t, 1);
            write(fd, &mitad, 1);
            write(fd, &flen, 1);
            write(fd, figura.c_str(), figura.size());
            write(fd, &clen, 4);
            write(fd, contenido.c_str(), contenido.size());
        }
    }
}


 // espera conexiones de otros intermediarios. Luego manejarConexionTP las atiende
void escucharFork()
{
    int listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    ::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PUERTO_INTER_TCP);
    ::bind(listenFd, (struct sockaddr*)&addr, sizeof(addr));
    ::listen(listenFd, 5);

    std::cout << "[I] TCP en puerto " << PUERTO_INTER_TCP << "\n";

    while (true) {
        struct sockaddr_in vecinoAddr = {};
        socklen_t vecinoLen = sizeof(vecinoAddr);
        int vecinoFd = ::accept(listenFd, (struct sockaddr*)&vecinoAddr, &vecinoLen);
        if (vecinoFd < 0) continue;

        char ipBuf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &vecinoAddr.sin_addr, ipBuf, sizeof(ipBuf));
        std::string forkIp(ipBuf);

        std::thread([vecinoFd, forkIp]() {
            manejarConexionTP(vecinoFd, forkIp);
            ::close(vecinoFd);
        }).detach();
    }
}


 // inicio handshake hacia un vecino. Para esta función seguí una lógica similar
 // a la que en aquel momento tenía Jefferson, que era con quien estaba probando
 // Lo que se hace es iniciar conexiones a intermediarios vecinos que hayamos descubierto
void handshakeFork(const std::string& forkIp)
{
    VSocket* tcp = new Socket('s');
    try {
        tcp->Connect(forkIp.c_str(), PUERTO_INTER_TCP);

        std::string csv = getLegosPropios();
        uint8_t tipo = TP_HANDSHAKE;
        uint32_t len = (uint32_t)csv.size();
        tcp->Write(&tipo, 1);
        tcp->Write(&len, 4);
        if (!csv.empty()) tcp->Write(csv.c_str(), csv.size());

        // leer handshake de respuesta
        uint8_t rTipo;
        if (tcp->Read(&rTipo, 1) > 0 && rTipo == TP_HANDSHAKE) {
            uint32_t rLen = 0;
            tcp->Read(&rLen, 4);
            std::string payload(rLen, '\0');
            if (rLen > 0) tcp->Read(&payload[0], rLen);
            guardarFigsVecino(forkIp, payload);
        }
    } catch (...) {}
    tcp->Close();
    delete tcp;
}

// escuchar join de otras islas por 3030
void escucharJoin()
{
    Socket sock('d');
    int opt = 1;
    sock.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sock.Bind(PUERTO_JOIN_UDP);
    std::cout << "[I] UDP JOIN en puerto " << PUERTO_JOIN_UDP << "\n";

    while (true) {
        uint8_t pkt[64] = {};
        struct sockaddr_in from = {};
        int r = sock.recvFrom((char*)pkt, sizeof(pkt), &from);
        if (r < 5 || pkt[0] != TP_JOIN) continue;

        struct in_addr srcAddr;
        memcpy(&srcAddr, &pkt[1], 4);
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &srcAddr, ip, sizeof(ip));
        std::string forkIp(ip);

        if (forkIp == local_ip) continue;  // ignorar mi ip

        bool conocido = false;
        {
            std::lock_guard<std::mutex> lk(mtx_vecinos_fork);
            for (auto& v : vecinos_fork)
                if (v.ip == forkIp) {
                    conocido = true; break;
                }
        }
        if (conocido) continue;

        std::cout << "[I] JOIN de " << forkIp << "\n";
        std::thread([forkIp](){ handshakeFork(forkIp); }).detach();
    }
}

 // mandar paq join a las broadcast de las otrsa islas por  3030
 // esto lo que hace es avisarle a los otros intermediarios que nosotros somos un
 // intermediario
void mandarJoin()
{
    Socket sock('d');
    int opt = 1;
    sock.SetSockOpt(SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));

    uint8_t pkt[5];
    pkt[0] = TP_JOIN;
    struct in_addr myAddr; // la direccion de nosotros
    inet_pton(AF_INET, local_ip.c_str(), &myAddr);
    memcpy(&pkt[1], &myAddr, 4);

    struct sockaddr_in dest = {};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PUERTO_JOIN_UDP);
// se lo mandamos a las islas que hay
    while (true) {
        for (int i = 0; i < num_islas; ++i) {
            inet_pton(AF_INET, broadcast_islas[i], &dest.sin_addr);
            sock.sendTo((char*)pkt, sizeof(pkt), &dest);
        }
        sleep(5);
    }
}

 // hacer una solicitud a un intermediario
std::string consultaFork(const std::string& ip, const std::string& figura, uint8_t mitad)
{
    // hacemos una solicitud con la figura y mitad que quiere el cliente
    std::vector<uint8_t> pkt(3 + figura.size());
    pkt[0] = TP_IR_REQUEST;
    pkt[1] = mitad;
    pkt[2] = (uint8_t)figura.size();
    memcpy(pkt.data() + 3, figura.c_str(), figura.size());

    std::string resultado;
    VSocket* tcp = new Socket('s');
    try {
        tcp->Connect(ip.c_str(), PUERTO_INTER_TCP);
        tcp->Write((char*)pkt.data(), pkt.size());

        uint8_t tipo; // si llega respuesta, se lee el contenido de la figura
        if (tcp->Read(&tipo, 1) > 0 && tipo == TP_IR_RESPONSE) {
            uint8_t mitadResp, tamNombre;
            tcp->Read(&mitadResp, 1);
            tcp->Read(&tamNombre, 1);
            std::string nombre(tamNombre, '\0');
            if (tamNombre > 0) tcp->Read(&nombre[0], tamNombre);

            uint32_t tam = 0;
            tcp->Read(&tam, 4);
            resultado.resize(tam);
            if (tam > 0) tcp->Read(&resultado[0], tam);
        }
    } catch (...) {}
    tcp->Close();
    delete tcp;
    return resultado;
}

 // parsear una parte de una figura
bool parsearFigura(const char* request, std::string& figura, uint8_t& mitad)
{
    const char* fig = strstr(request, "figure=");
    const char* par = strstr(request, "part=");
    if (fig && par) {
        fig += 7;
        const char* end = strpbrk(fig, "& \t\r\n");
        figura = end ? std::string(fig, end - fig) : std::string(fig);
        mitad = (uint8_t)atoi(par + 5);
        return !figura.empty();
    }
    char nombre[128];
    int parte;
    const char* path = strstr(request, "/figura/");
    if (path && sscanf(path, "/figura/%127[^/]/%d", nombre, &parte) == 2) {
        figura = nombre;
        mitad = (uint8_t)parte;
        return true;
    }
    return false;
}

 // Se busca una figura en servidor local
bool probarFiguras(const char* request, size_t n, VSocket* client)
{
    if (!server_found) return false;

    std::string ip;
    { std::lock_guard<std::mutex> lk(mtx_server); ip = server_ip; }

    char response[BUFSIZE];
    VSocket* server = new Socket('s');
    size_t m = 0;
    try {
        server->Connect(ip.c_str(), PUERTO_SERVER);
        server->Write(request, n);
        m = server->Read(response, BUFSIZE - 1);
    } catch (...) {}
    server->Close();
    delete server;

    if (m > 0) {
        response[m] = '\0';
        if (!strstr(response, "404 Not Found")) {
            client->Write(response, m);
            std::cout << "[I] Respuesta del servidor local\n";
            return true;
        }
    }
    return false;
}

 // Función de manejo de solicitudes de parte de los clientes
void task(VSocket *client)
{
    char request[BUFSIZE];
    memset(request, 0, BUFSIZE);

    size_t n = client->Read(request, BUFSIZE - 1);
    if (n <= 0) { client->Close(); delete client; return; }
    request[n] = '\0';
    std::cout << "[I] Solicitud recibida\n";

    if (strstr(request, "/list")) {
        std::string body;
        std::string localCsv = getLegosPropios();
        if (!localCsv.empty()) {
            std::istringstream ss(localCsv);
            std::string fig;
            while (std::getline(ss, fig, ','))
                if (!fig.empty()) body += fig + "\n";
        }
        std::vector<ForkVecino> copia;
        { std::lock_guard<std::mutex> lk(mtx_vecinos_fork); copia = vecinos_fork; }
        for (const auto& v : copia)
            for (const auto& fig : v.figuras)
                body += fig + "\n";
        std::string resp = "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\n" + body;
        client->Write(resp.c_str(), resp.size());
        client->Close();
        delete client;
        return;
    }

    // primero se busca la figura en el servidor local
    bool respondido = probarFiguras(request, n, client);

    // Si la figura no se encuentra en la isla mia, entonces tengo que ver
    // si está en otra de las islas.
    if (!respondido) {
        std::string figura;
        uint8_t mitad = 0;
        if (parsearFigura(request, figura, mitad)) {
            std::vector<ForkVecino> copia;
            { std::lock_guard<std::mutex> lk(mtx_vecinos_fork); copia = vecinos_fork; }

            for (const auto& vecino : copia) {
                auto& figs = vecino.figuras;
                if (std::find(figs.begin(), figs.end(), figura) == figs.end()) continue;
                // se consulta a los inter.
                std::string contenido = consultaFork(vecino.ip, figura, mitad);
                if (contenido.empty()) continue;

                std::string resp = "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\n" + contenido;
                client->Write(resp.c_str(), resp.size());
                std::cout << "[I] Figura de vecino " << vecino.ip << "\n";
                respondido = true;
                break;
            }
        }
    }

    // Si ya la figura del todo no aparece por ningun lado, entonces se lo indico al cliente
    if (!respondido) {
        const char* err = "HTTP/1.0 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFigura no encontrada en ningún intermediario.\n";
        client->Write(err, strlen(err));
    }

    client->Close();
    delete client;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <ip_propia>\n";
        return 1;
    }
    local_ip = argv[1];

    std::thread(escucharFiguras).detach();
    std::thread(hacerBroadcast).detach();
    std::thread(escucharJoin).detach();
    std::thread(mandarJoin).detach();
    std::thread(escucharFork).detach();

    VSocket *s1 = new Socket('s');
    s1->Bind(PUERTO_FORK);
    s1->MarkPassive(5);

    std::cout << "Fork escuchando puerto " << PUERTO_FORK << " (IP: " << local_ip << ")\n";

    while (true)
    {
        VSocket *client = s1->AcceptConnection();
        std::cout << "[I] Cliente conectado" << "\n";
        std::thread(task, client).detach();
    }

    delete s1;
    return 0;
}
