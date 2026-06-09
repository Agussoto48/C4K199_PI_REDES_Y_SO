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
#include <cstring>
#include <cstdio>
#include <ctime>

#include "Socket.h"
#include "SistemaArchivos.h"
#include "mensajes.hpp"

#define PORT 2026
#define BUFSIZE 4096
#define RUTA_FS "lego.dat"
#define RUTA_LOG "servidor.log"

static std::ofstream archivoLog;
static std::mutex mutexLog;

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

   s1 = new Socket('s');

   s1->Bind(PORT);
   s1->MarkPassive(5);

   registrar("Servidor iniciado en puerto " + std::to_string(PORT));
   std::cout << "Servidor de piezas escuchando en puerto " << PORT << std::endl;
   std::cout << "Usando archivo de datos: " << RUTA_FS << std::endl;
   std::cout << "Bitacora: " << RUTA_LOG << std::endl;

   for (;;)
   {
      client = s1->AcceptConnection();
      worker = new std::thread(task, client);
      worker->detach();
   }

   delete s1;
   return 0;
}