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
 */

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include "Socket.h"

#define HOST "127.0.0.1" // servidor de piezas 192.168.0.105 es el mio
#define PORT 2026


std::string ExtraerCuerpoHTTP( const std::string & respuesta ) {
   size_t pos = respuesta.find( "\r\n\r\n" );
   if ( pos == std::string::npos ) return respuesta;
   return respuesta.substr( pos + 4 );
}

int ExtraerCodigoHTTP( const std::string & respuesta ) {
   size_t space = respuesta.find( ' ' );
   if ( space == std::string::npos ) return 0;
   return atoi( respuesta.substr( space + 1, 3 ).c_str() );
}

// Se muestra lista de figuras
void mostrarListado( const std::string & cuerpo ) {
   std::istringstream stream( cuerpo );
   std::string linea;

   std::cout << "Figuras disponibles:\n";

   while ( std::getline( stream, linea ) ) {
      if ( !linea.empty() && linea.back() == '\r' ) linea.pop_back();
      if ( linea.empty() ) continue;
      std::cout << "  " << linea << "\n";
   }
}

// Mostrar piezas de figuras solicitada
void mostrarFigura( const std::string & cuerpo ) {
   std::istringstream stream( cuerpo );
   std::string linea;

   while ( std::getline( stream, linea ) ) {
      if ( !linea.empty() && linea.back() == '\r' ) linea.pop_back();
      if ( linea.empty() ) continue;

      size_t eq = linea.find( '=' );
      if ( eq == std::string::npos ) continue;

      std::string clave = linea.substr( 0, eq );
      std::string valor = linea.substr( eq + 1 );

      if ( clave == "figura" || clave == "mitad" ) continue;

      if ( clave == "total" ) {
         std::cout << "Total de piezas: " << valor << "\n";
      } else {
         std::cout << valor << "\t" << clave << "\n";
      }
   }
}

std::string enviarRequest( const std::string & request ) {
   Socket socket( 's' );
   std::string response;
   char buffer[4096];
   size_t bytesRead = 0;

   socket.Connect( HOST, PORT );
   socket.Write( request.c_str() );

   while ( true ) {
      bytesRead = socket.Read( buffer, sizeof( buffer ) - 1 );
      if ( bytesRead == 0 ) break;
      buffer[bytesRead] = '\0';
      response += buffer;
   }

   return response;
}

void requestList() {
   std::string request =
      "GET /list HTTP/1.1\r\n"
      "Host: " HOST "\r\n"
      "Connection: close\r\n"
      "\r\n";

   std::string response = enviarRequest( request );
   int codigo = ExtraerCodigoHTTP( response );

   if ( codigo == 200 ) {
      mostrarListado( ExtraerCuerpoHTTP( response ) );
   } else {
      std::cerr << "Error del servidor: " << ExtraerCuerpoHTTP( response );
   }
}

void requestPiece( const std::string & figura, const std::string & mitad ) {
   std::string request =
      "GET /figura/" + figura + "/" + mitad + " HTTP/1.1\r\n"
      "Host: " HOST "\r\n"
      "Connection: close\r\n"
      "\r\n";

   std::string response = enviarRequest( request );
   int codigo = ExtraerCodigoHTTP( response );

   if ( codigo == 200 ) {
      mostrarFigura( ExtraerCuerpoHTTP( response ) );
   } else {
      std::cerr << ExtraerCuerpoHTTP( response );
   }
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
         << "  " << argv[0] << " --list\n" << "  " << argv[0] << " <figura> <mitad>\n";
      return 1;
   }

   figura = argv[1];
   mitad  = argv[2];

   if ( mitad == "0" ) {
      std::cout << "Figura: " << figura << std::endl;
      std::cout << "Primera mitad:" << std::endl;
      requestPiece( figura, "1" );

      std::cout << "\nSegunda mitad:" << std::endl;
      requestPiece( figura, "2" );
   } else {
      std::cout << "Figura: " << figura << ". Mitad solicitada: "
         << mitad << std::endl;
      requestPiece( figura, mitad );
   }

   return 0;
}
