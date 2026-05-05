#include <string>
#include <cstring>
#include "Socket.h"

enum ID_TIPO
{
    LIST_QUERY = 10,
    LIST_RESULT = 11,
    FIGURE_QUERY = 14,
    FIGURE_RESULT = 15,
    FIGURE_NOT_FOUND = 16
};

int EmpiezaCon(const char *texto, const char *prefijo)
{
    int i = 0;

    while (prefijo[i] != '\0')
    {
        if (texto[i] != prefijo[i])
        {
            return 0;
        }
        i++;
    }

    return 1;
}
void ExtraerPrimeraLinea(const char *request, char *linea)
{
    int i = 0;

    while (request[i] != '\0' && request[i] != '\r' && request[i] != '\n')
    {
        linea[i] = request[i];
        i++;
    }

    linea[i] = '\0';
}
int ParsearSolicitudHTTP(const char *linea, char *nombreFigura, int &mitad)
{
    char metodo[16];
    char ruta[256];
    char version[32];
    int leidos = 0;

    leidos = sscanf(linea, "%15s %255s %31s", metodo, ruta, version);

    if (leidos != 3)
    {
        return 0;
    }

    if (strcmp(metodo, "GET") != 0)
    {
        return 0;
    }

    if (strcmp(ruta, "/list") == 0)
    {
        return LIST_QUERY;
    }

    if (EmpiezaCon(ruta, "/figura/") == 0)
    {
        return 0;
    }

    if (sscanf(ruta, "/figura/%127[^/]/%d", nombreFigura, &mitad) != 2)
    {
        return 0;
    }

    return FIGURE_QUERY;
}
std::string ConstruirMensajeServidor(int tipo, const char *nombreFigura, int mitad)
{
    if (tipo == LIST_QUERY)
    {
        return "10|";
    }

    if (tipo == FIGURE_QUERY)
    {
        return "14|" + std::string(nombreFigura) + "|" + std::to_string(mitad);
    }

    return "";
}
std::string MensajeExito(const std::string &cuerpo)
{
    std::string respuesta;
    respuesta += "HTTP/1.1 200 OK\r\n";
    respuesta += "Content-Type: text/plain\r\n";
    respuesta += "Content-Length: " + std::to_string(cuerpo.length()) + "\r\n";
    respuesta += "Connection: close\r\n";
    respuesta += "\r\n";
    respuesta += cuerpo;
    return respuesta;
}

std::string MensajeInvalido(const std::string &cuerpo)
{
    std::string respuesta;
    respuesta += "HTTP/1.1 400 Bad Request\r\n";
    respuesta += "Content-Type: text/plain\r\n";
    respuesta += "Content-Length: " + std::to_string(cuerpo.length()) + "\r\n";
    respuesta += "Connection: close\r\n";
    respuesta += "\r\n";
    respuesta += cuerpo;
    return respuesta;
}

std::string MensajeError(const std::string &cuerpo)
{
    std::string respuesta;
    respuesta += "HTTP/1.1 404 Not Found\r\n";
    respuesta += "Content-Type: text/plain\r\n";
    respuesta += "Content-Length: " + std::to_string(cuerpo.length()) + "\r\n";
    respuesta += "Connection: close\r\n";
    respuesta += "\r\n";
    respuesta += cuerpo;
    return respuesta;
}
int ObtenerTipoRespuesta(const std::string &mensaje, std::string &cuerpo)
{
    size_t pos = mensaje.find('|');

    if (pos == std::string::npos)
    {
        cuerpo = "Respuesta invalida del servidor.\n";
        return 0;
    }

    int tipo = atoi(mensaje.substr(0, pos).c_str());
    cuerpo = mensaje.substr(pos + 1);

    return tipo;
}
