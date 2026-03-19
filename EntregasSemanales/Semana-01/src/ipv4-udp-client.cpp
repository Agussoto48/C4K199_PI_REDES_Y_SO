/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-6
  *  Grupos: 2 y 3
  *
  ****** IPv4 test
  *
  * (Fedora version)
  *
  *   Client side implementation of UDP client-server model 
  *
 **/

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include<iostream>

#include "VSocket.hpp"
#include "Socket.hpp"

#define PORT    1234 
#define MAXLINE 1024 

int main() {
   VSocket * client;
   int sockfd; 
   int n, len; 
   char buffer[MAXLINE]; 
   char *hello = (char *) "Hello from CI0123 client"; 
   struct sockaddr_in other;

   client = new Socket( 'd' );	// Creates an UDP socket: datagram

   memset( &other, 0, sizeof( other ) ); 


   std::cout << "Quiere LocalHost o la IP del lab?: \n";
   std::cout << "\t1) LocalHost\n\t2)Lab\n";
   int opcion = 1;
   std::cin >> opcion;   
   
   const char* localhost = "127.0.0.1";
   const char* lab = "10.1.35.50";
   const char* eleccion = (opcion == 1)? localhost : lab;

   other.sin_family = AF_INET; 
   other.sin_port = htons( PORT ); 
   //Local: 127.0.0.1, con el original: 10.1.35.50
   n = inet_pton( AF_INET, eleccion, &other.sin_addr );	// IP address to test our client with a Python server on lab 3-5
   if ( 1 != n ) {
      printf( "Error converting from IP address\n" );
      exit( 23 );
   }

   n = client->sendTo( (void *) hello, strlen( hello ), (void *) & other ); 
   printf("Client: Hello message sent.\n"); 
   
   n = client->recvFrom( (void *) buffer, MAXLINE, (void *) & other );
   buffer[n] = '\0'; 
   printf("Client message received: %s\n", buffer); 

   client->Close(); 

   return 0;
 
} 

