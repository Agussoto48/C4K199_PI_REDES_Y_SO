/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2026-i
 *  Grupos: 2 y 3
 *
 *  Servidor de piezas con hilos
 *
 **/

#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <atomic>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"
#include "SistemaArchivos.h"
#include "mensajes.hpp"

#define PORT 3001
#define BUFSIZE 4096
#define RUTA_FS "lego.dat"
#define RUTA_LOG "servidor.log"
#define DESCUBRIMIENTO_ESCUCHAR 2028 // el fork hace broadcast aqui y figuras escucha aqui
#define DESCUBRIMIENTO_ENVIAR 2029 // Figuras hace broadcast aqui y el intermerdiario escucha
#define DIR_BROADCAST "127.0.0.1"  // correr local
// #define DIR_BROADCAST "172.16.123.63"  // el broadcast de mi isla

std::atomic<bool> inter_found(false);

std::ofstream archivoLog;
std::mutex mutexLog;

// escucha para averiguar ip de intermediario
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

        int interPort;
        if (sscanf(buf, "INTER_HERE %d", &interPort) != 1)
            continue;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));

        if (!inter_found.exchange(true))
            std::cout << "[Figuras] Intermediario en " << ip << ":" << interPort << "\n";

        char reply[64];
        snprintf(reply, sizeof(reply), "FIGURAS_HERE %d", PORT);
        struct sockaddr_in unicast;
        memset(&unicast, 0, sizeof(unicast));
        unicast.sin_family = AF_INET;
        unicast.sin_port = htons(DESCUBRIMIENTO_ENVIAR);
        unicast.sin_addr = from.sin_addr;
        sock.sendTo(reply, strlen(reply), &unicast);
    }
}

// hace broadcast para anunciar direccion
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
    snprintf(msg, sizeof(msg), "FIGURAS_HERE %d", PORT);

    while (!inter_found)
    {
        sock.sendTo(msg, strlen(msg), &dest);
        std::cout << "[Figuras] Buscando intermediario (brd en " << DESCUBRIMIENTO_ENVIAR << ")\n";
        sleep(2);
    }
}

static void registrar(const std::string &evento)
{
   time_t ahora = time(nullptr);
   char timestamp[32];
   strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&ahora));

   std::lock_guard<std::mutex> lock(mutexLog);
   archivoLog << "[" << timestamp << "] " << evento << "\n";
   archivoLog.flush();
}

void task(VSocket *client)
{
   char buffer[BUFSIZE];
   char linea[BUFSIZE];
   char nombreFigura[128];
   int mitad = 0;
   int tipoHTTP = 0;
   size_t bytesLeidos = 0;
   SistemaArchivos fs;
   std::string respuesta;

   memset(buffer, 0, BUFSIZE);
   memset(linea, 0, BUFSIZE);
   memset(nombreFigura, 0, 128);

   registrar("Nueva conexion aceptada");

   bytesLeidos = client->Read(buffer, BUFSIZE - 1);
   buffer[bytesLeidos] = '\0';

   ExtraerPrimeraLinea(buffer, linea);
   registrar(std::string("Request: ") + linea);

   tipoHTTP = ParsearSolicitudHTTP(linea, nombreFigura, mitad);

   if (tipoHTTP == 0)
   {
      registrar("400 Bad Request - solicitud invalida");
      respuesta = MensajeInvalido("Solicitud HTTP invalida.\n");
      client->Write(respuesta.c_str(), respuesta.length());
      client->Close();
      delete client;
      return;
   }

   if (fs.AbrirSistema(RUTA_FS) != 0)
   {
      registrar("500 Error - no se pudo abrir " + std::string(RUTA_FS));
      respuesta = MensajeError("Error: no se pudo abrir lego.dat\n");
      client->Write(respuesta.c_str(), respuesta.length());
      client->Close();
      delete client;
      return;
   }

   if (tipoHTTP == LIST_QUERY)
   {
      registrar("200 OK - listado de figuras enviado");
      respuesta = MensajeExito(fs.ConstruirTextoListadoFiguras());
   }
   else
   {
      if (mitad != 1 && mitad != 2)
      {
         registrar("400 Bad Request - mitad invalida para figura: " + std::string(nombreFigura));
         respuesta = MensajeInvalido("Error: mitad invalida.\n");
      }
      else
      {
         std::string texto = fs.ConstruirTextoFigura(nombreFigura, mitad);
         if (texto == "Error: figura no encontrada.\n")
         {
            registrar("404 Not Found - figura: " + std::string(nombreFigura) + " mitad: " + std::to_string(mitad));
            respuesta = MensajeError(texto);
         }
         else
         {
            registrar("200 OK - figura: " + std::string(nombreFigura) + " mitad: " + std::to_string(mitad));
            respuesta = MensajeExito(texto);
         }
      }
   }

   fs.CerrarSistema();

   client->Write(respuesta.c_str(), respuesta.length());
   client->Close();
   delete client;
}

/**
 * Se espera la conexion del cliente
 */
int main(int argc, char **argv)
{
   std::thread *worker;
   VSocket *s1;
   VSocket *client;

   archivoLog.open(RUTA_LOG, std::ios::app);
   if (!archivoLog.is_open())
   {
      std::cerr << "Error: no se pudo abrir " << RUTA_LOG << std::endl;
      return 1;
   }

   std::thread(escuchar).detach();
   std::thread(hacerBroadcast).detach();

   s1 = new Socket('s');

   s1->Bind(PORT);
   s1->MarkPassive(5);

   registrar("Servidor iniciado en puerto " + std::to_string(PORT));
   std::cout << "Servidor de piezas escuchando en puerto " << PORT << std::endl;
   std::cout << "Usando archivo de datos: " << RUTA_FS << std::endl;
   std::cout << "Bitacora: " << RUTA_LOG << std::endl;

   while (true)
   {
      client = s1->AcceptConnection();
      worker = new std::thread(task, client);
      worker->detach();
   }

   delete s1;
   return 0;
}