
#include <iostream>
#include <string>
#include <cstring>
#include "Socket.h"
#include "../include/mensajes.hpp"

#define HOST "127.0.0.1"
#define PORT_CLIENTE 2025
#define PORT_SERVER 2026
#define BUFSIZE 1024

void AtenderCliente(VSocket *cliente)
{
    char buffer[BUFSIZE];
    char linea[BUFSIZE];
    char nombreFigura[128];
    int mitad = 0;
    int tipoHTTP = 0;
    size_t bytesLeidos = 0;

    memset(buffer, 0, BUFSIZE);
    memset(linea, 0, BUFSIZE);
    memset(nombreFigura, 0, sizeof(nombreFigura));

    bytesLeidos = cliente->Read(buffer, BUFSIZE - 1);
    buffer[bytesLeidos] = '\0';

    std::cout << "REQUEST DEL CLIENTE" << std::endl;
    std::cout << buffer << std::endl;

    ExtraerPrimeraLinea(buffer, linea);
    tipoHTTP = ParsearSolicitudHTTP(linea, nombreFigura, mitad);

    if (tipoHTTP == 0)
    {
        std::string error = MensajeInvalido("Solicitud HTTP invalida.\n");
        cliente->Write(error.c_str(), error.length());
        cliente->Close();
        delete cliente;
        return;
    }

    std::string mensajeServidor = ConstruirMensajeServidor(tipoHTTP, nombreFigura, mitad);

    std::cout << "MENSAJE AL SERVIDOR" << std::endl;
    std::cout << mensajeServidor << std::endl;

    Socket socketServidor('s');
    socketServidor.Connect(HOST, PORT_SERVER);
    socketServidor.Write(mensajeServidor.c_str(), mensajeServidor.length());

    memset(buffer, 0, BUFSIZE);
    bytesLeidos = socketServidor.Read(buffer, BUFSIZE - 1);
    buffer[bytesLeidos] = '\0';

    std::string respuestaServidor = buffer;
    std::string cuerpo;
    int tipoRespuesta = ObtenerTipoRespuesta(respuestaServidor, cuerpo);

    std::cout << "RESPUESTA DEL SERVIDOR" << std::endl;
    std::cout << respuestaServidor << std::endl;

    std::string respuestaHTTP;

    if (tipoRespuesta == LIST_RESULT || tipoRespuesta == FIGURE_RESULT)
    {
        respuestaHTTP = MensajeExito(cuerpo);
    }
    else if (tipoRespuesta == FIGURE_NOT_FOUND)
    {
        respuestaHTTP = MensajeError(cuerpo);
    }
    else
    {
        respuestaHTTP = MensajeInvalido("Respuesta invalida del servidor.\n");
    }

    cliente->Write(respuestaHTTP.c_str(), respuestaHTTP.length());

    socketServidor.Close();
    cliente->Close();
    delete cliente;
}

int main()
{
    VSocket *servidor = new Socket('s');
    VSocket *cliente = nullptr;

    servidor->Bind(PORT_CLIENTE);
    servidor->MarkPassive(5);

    std::cout << "Intermediario escuchando en puerto " << PORT_CLIENTE << std::endl;
    std::cout << "Conectando al servidor en puerto " << PORT_SERVER << std::endl;

    while (true)
    {
        cliente = servidor->AcceptConnection();
        AtenderCliente(cliente);
    }

    delete servidor;
    return 0;
}