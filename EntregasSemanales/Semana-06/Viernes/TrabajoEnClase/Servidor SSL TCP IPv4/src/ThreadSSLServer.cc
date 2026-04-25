/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  ****** SSLSocket example, server code
  *
  * (Fedora version)
  *
 **/

#include <pthread.h>
#include <cstdlib>   // atoi
#include <cstdio>    // printf
#include <cstring>   // strlen, strcmp

#include "SSLSocket.h"

#define PORT 4321


void * Service( void * arg ) {
   SSLSocket * client = (SSLSocket *) arg;

   char buf[ 1024 ] = { 0 };
   int bytes;

   const char* ServerResponse="\n<Body>\n\
\t<Server>os.ecci.ucr.ac.cr</Server>\n\
\t<dir>ci0123</dir>\n\
\t<Name>Proyecto Integrador Redes y sistemas Operativos</Name>\n\
\t<NickName>PIRO</NickName>\n\
\t<Description>Consolidar e integrar los conocimientos de redes y sistemas operativos</Description>\n\
\t<Author>profesores PIRO</Author>\n\
</Body>\n";

   const char *validMessage = "\n<Body>\n\
\t<UserName>piro</UserName>\n\
\t<Password>ci0123</Password>\n\
</Body>\n";

   client->Accept();
   client->ShowCerts();

   bytes = client->Read( buf, sizeof( buf ) );
   buf[ bytes ] = '\0';
   printf( "Client msg: \"%s\"\n", buf );

   if ( ! strcmp( validMessage, buf ) ) {
      client->Write( ServerResponse, strlen( ServerResponse ) );
   } else {
      client->Write( "Invalid Message", strlen( "Invalid Message" ) );
   }

   client->Close();
   delete client;

   return nullptr;
}


int main( int cuantos, char ** argumentos ) {
   SSLSocket * server;
   SSLSocket * client;
   pthread_t worker;
   int port = PORT;

   if ( cuantos > 1 ) {
      port = atoi( argumentos[ 1 ] );
   }

   server = new SSLSocket( (const char *) "certs/ci0123.pem",(const char *) "certs/key0123.pem",false );

   server->Bind( port );
   server->MarkPassive( 10 );

   for ( ; ; ) {
      client = (SSLSocket *) server->AcceptConnection();
      client->Copy( server );

      pthread_create( &worker, nullptr, Service, client );
      pthread_detach( worker );
   }

   return 0;
}