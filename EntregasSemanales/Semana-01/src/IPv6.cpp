/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2026-i
  *  Grupos: 2 y 3
  *
  *
  *  Esta prueba solo funciona utilizando un equipo de la red interna de la ECCI, por lo que
  *  deberan realizarlo en la ECCI o  conectarse por la VPN para completarla
  *  La direccion IPv6 provista es una direccion privada
  *  Tambien deben prestar atencion al componente que esta luego del "%" en la direccion y que hace
  *  referencia a la interfaz de red utilizada para la conectividad, en el ejemplo se presenta la interfaz "eno1"
  *  pero es posible que su equipo tenga otra interfaz
  *
 **/

#include <stdio.h>
#include <string.h>
#include "Socket.hpp"

int main( int argc, char * argv[] ) {
   const char * lab = "fe80::8f5a:e2e1:7256:ffe3%enp0s31f6";
   const char * request = "GET / HTTP/1.1\r\nhost: redes.ecci\r\n\r\n";

   Socket s( 's', true );
   char a[512];

   memset( a, 0, 512 );
   s.Connect( lab, (char *) "http" );
   s.Write( request );
   s.Read( a, 512 );
   printf( "%s\n", a);

}
