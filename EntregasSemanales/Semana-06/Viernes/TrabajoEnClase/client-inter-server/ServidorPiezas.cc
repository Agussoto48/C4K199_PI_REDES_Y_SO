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
#include <thread>
#include <cstring>
#include <cstdio>

#include "Socket.h"
#include "SistemaArchivos.h"

#define PORT 2026
#define BUFSIZE 1024
#define RUTA_FS "lego.dat"
enum ID_TIPO
{
   LIST_QUERY = 10,
   LIST_RESULT = 11,
   FIGURE_QUERY = 14,
   FIGURE_RESULT = 15,
   FIGURE_NOT_FOUND = 16
};

int LeerMensajeIntermediario(const char *buffer, char *nombreFigura, int &mitad)
{
   std::string msg = buffer;

   size_t p1 = msg.find('|');
   if (p1 == std::string::npos)
   {
      return 0;
   }

   int tipo = atoi(msg.substr(0, p1).c_str());

   if (tipo == LIST_QUERY)
   {
      return LIST_QUERY;
   }

   if (tipo == FIGURE_QUERY)
   {
      size_t p2 = msg.find('|', p1 + 1);
      if (p2 == std::string::npos)
      {
         return 0;
      }

      std::string figura = msg.substr(p1 + 1, p2 - p1 - 1);
      strcpy(nombreFigura, figura.c_str());

      mitad = atoi(msg.substr(p2 + 1).c_str());

      return FIGURE_QUERY;
   }

   return 0;
}

void task(VSocket *client)
{
   char buffer[BUFSIZE];
   char linea[BUFSIZE];
   char nombreFigura[128];
   int mitad = 0;
   int tipoSolicitud = 0;
   size_t bytesLeidos = 0;
   SistemaArchivos fs;
   std::string respuesta;

   memset(buffer, 0, BUFSIZE);
   memset(linea, 0, BUFSIZE);
   memset(nombreFigura, 0, 128);

   bytesLeidos = client->Read(buffer, BUFSIZE - 1);
   buffer[bytesLeidos] = '\0';

   std::cout << "MENSAJE DEL INTERMEDIARIO" << std::endl;
   std::cout << buffer << std::endl;

   if (fs.AbrirSistema(RUTA_FS) != 0)
   {
      respuesta = "16|Error: no se pudo abrir lego.dat\n";
      client->Write(respuesta.c_str(), respuesta.length());
      client->Close();
      delete client;
      return;
   }
   tipoSolicitud = LeerMensajeIntermediario(buffer, nombreFigura, mitad);

   if (tipoSolicitud == LIST_QUERY)
   {
      respuesta = "11|" + fs.ConstruirTextoListadoFiguras();
   }
   else if (tipoSolicitud == FIGURE_QUERY)
   {
      respuesta = fs.ConstruirRespuestaHTTPFigura(nombreFigura, mitad);
      if (respuesta.empty())
      {
         respuesta = "16|Figura no encontrada\n";
      }
      else
      {
         respuesta = "15|" + respuesta;
         std::cout << "RESPUESTA: " << respuesta << std::endl;
      }
   }
   else
   {
      respuesta = "16|Solicitud invalida\n";
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

   s1 = new Socket('s');

   s1->Bind(PORT);
   s1->MarkPassive(5);

   std::cout << "Servidor de piezas escuchando en puerto " << PORT << std::endl;
   std::cout << "Usando archivo de datos: " << RUTA_FS << std::endl;

   for (;;)
   {
      client = s1->AcceptConnection();
      worker = new std::thread(task, client);
      worker->detach();
   }

   delete s1;
   return 0;
}