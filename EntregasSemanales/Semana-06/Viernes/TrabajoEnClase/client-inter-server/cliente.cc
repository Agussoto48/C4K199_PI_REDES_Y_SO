/**
 * Cliente que pide piezas de lego a servidor propio.
 * PI de Redes-Oper CI0123 | I-2026
 *
 * Uso:
 *   ./cliente.out --list
 *   ./cliente.out <figura> <mitad>
 *
 * Ejemplo:
 *   ./cliente.out --list
 *   ./cliente.out pez 1
 *   ./cliente.out pez 0
 *
 * Basado en el cliente de la primera entrega del proyecto
 */

#include <iostream>
#include <string>
#include <cstring>

#include "Socket.h"

#define HOST "127.0.0.1"
#define PORT 2025


std::string ExtraerCuerpoHTTP( const std::string & respuesta ) {
   size_t pos = respuesta.find( "\r\n\r\n" );

   if ( pos == std::string::npos ) {
      return respuesta;
   }

   return respuesta.substr( pos + 4 );
}


void requestList() {
   Socket socket( 's' );
   std::string request;
   std::string response;
   char buffer[4096];
   size_t bytesRead = 0;

   request =
      "GET /list HTTP/1.1\r\n"
      "Host: " HOST "\r\n"
      "Connection: close\r\n"
      "\r\n";

   socket.Connect( HOST, PORT );
   socket.Write( request.c_str() );

   while ( true ) {
      bytesRead = socket.Read( buffer, sizeof( buffer ) - 1 );

      if ( bytesRead == 0 ) {
         break;
      }

      buffer[bytesRead] = '\0';
      response += buffer;
   }

   std::cout << ExtraerCuerpoHTTP( response );
}


void requestPiece( const std::string & figura, const std::string & mitad ) {
   Socket socket( 's' );
   std::string request;
   std::string response;
   std::string cuerpo;
   char buffer[4096];
   size_t bytesRead = 0;

   request =
      "GET /figura/" + figura + "/" + mitad + " HTTP/1.1\r\n"
      "Host: " HOST "\r\n"
      "Connection: close\r\n"
      "\r\n";

   socket.Connect( HOST, PORT );
   socket.Write( request.c_str() );

   while ( true ) {
      bytesRead = socket.Read( buffer, sizeof( buffer ) - 1 );

      if ( bytesRead == 0 ) {
         break;
      }

      buffer[bytesRead] = '\0';
      response += buffer;
   }

   cuerpo = ExtraerCuerpoHTTP( response );
   std::cout << cuerpo;
}


int main( int argc, char * argv[] ) {
   std::string figura;
   std::string mitad;

   if ( argc == 2 && strcmp( argv[1], "--list" ) == 0 ) {
      requestList();
      return 0;
   }

   if ( argc < 3 ) {
      std::cerr << "Uso:\n"
                << "  " << argv[0] << " --list\n"
                << "  " << argv[0] << " <figura> <mitad>\n";
      return 1;
   }

   figura = argv[1];
   mitad = argv[2];

   if ( mitad == "0" ) {
      std::cout << "Figura: " << figura << std::endl;
      std::cout << "Primera mitad:" << std::endl;
      requestPiece( figura, "1" );

      std::cout << "Segunda mitad:" << std::endl;
      requestPiece( figura, "2" );
   } else {
      std::cout << "Figura: " << figura << ". Mitad solicitada: "
      << mitad << std::endl;
      requestPiece( figura, mitad );
   }

   return 0;
}