/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2026-i
 *  Grupos: 2 y 3
 *
 *  ******   VSocket base class interface
 *
 * (Fedora version)
 *
 **/

#pragma once
#include <sys/socket.h>
#include <arpa/inet.h> // ntohs, htons
#include <stdexcept>   // runtime_error
#include <cstring>     // memset
#include <netdb.h>     // getaddrinfo, freeaddrinfo
#include <unistd.h>    // close
#include <cstddef>
#include <cstdio>
#include <sys/types.h>

class VSocket
{
private:
   int connectIPv4(const char*, int);
   int connectIPv6(const char*, int);
   
protected:
   int sockId; // Socket identifier
   bool IPv6;  // Is IPv6 socket?
   int port;   // Socket associated port
   char type;  // Socket type (datagram, stream, etc.)

public:
   void Init(char, bool = false);
   ~VSocket();

   void Close();
   int TryToConnect(const char *, int);
   int TryToConnect(const char *, const char *);
   virtual int Connect(const char *, int) = 0;
   virtual int Connect(const char *, const char *) = 0;

   virtual size_t Read(void *, size_t) = 0;
   virtual size_t Write(const void *, size_t) = 0;
   virtual size_t Write(const char *) = 0;
};
