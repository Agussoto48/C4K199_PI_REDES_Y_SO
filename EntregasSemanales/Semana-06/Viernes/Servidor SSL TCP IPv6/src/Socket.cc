
/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2026-i
 *  Grupos: 2 y 3
 *
 *  ******   Socket class implementation
 *
 * (Fedora version)
 *
 **/

#include <sys/socket.h> // sockaddr_in
#include <arpa/inet.h>  // ntohs
#include <unistd.h>     // write, read
#include <cstring>
#include <stdexcept>
#include <stdio.h> // printf

#include "Socket.h" // Derived class

/**
 *  Class constructor
 *     use Unix socket system call
 *
 *  @param     char t: socket type to define
 *     's' for stream
 *     'd' for datagram
 *  @param     bool ipv6: if we need a IPv6 socket
 *
 **/
Socket::Socket(char t, bool IPv6)
{

   this->Init(t, IPv6); // Call base class constructor
}

Socket::Socket(int id)
{

   this->Init(id);
}

/**
 *  Class destructor
 *
 **/
Socket::~Socket()
{
}

/**
 * Connect method
 *   use "TryToConnect" in base class
 *
 * @param      char * host: host address in dot notation, example "10.1.166.62"
 * @param      int port: process address, example 80
 *
 **/
int Socket::Connect(const char *hostip, int port)
{

   return this->TryToConnect(hostip, port);
}

/**
 * Connect method
 *   use "TryToConnect" in base class
 *
 * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
 * @param      char * service: process address, example "http"
 *
 **/
int Socket::Connect(const char *host, const char *service)
{

   return this->TryToConnect(host, service);
}

/**
 * Read method
 *   use "read" Unix system call (man 3 read)
 *
 * @param      void * buffer: buffer to store data read from socket
 * @param      int size: buffer capacity, read will stop if buffer is full
 *
 **/
size_t Socket::Read(void *buffer, size_t size)
{

   int st = -1;

   st = read(this->sockId, buffer, size);

   if (-1 == st)
   {
      throw std::runtime_error("Socket::Read( void *, size_t )");
   }

   return st;
}

/**
 * Write method
 *   use "write" Unix system call (man 3 write)
 *
 * @param      void * buffer: buffer to store data write to socket
 * @param      size_t size: buffer capacity, number of bytes to write
 *
 **/
size_t Socket::Write(const void *buffer, size_t size)
{

   int st = -1;

   st = write(this->sockId, buffer, size);

   if (-1 == st)
   {
      throw std::runtime_error("Socket::Write( void *, size_t )");
   }

   return st;
}

/**
 * Write method
 *   use "write" Unix system call (man 3 write)
 *
 * @param      char * text: text to write to socket
 *
 **/
size_t Socket::Write(const char *text)
{

   int st = -1;

   st = write(this->sockId, text, strlen(text));

   if (-1 == st)
   {
      throw std::runtime_error("Socket::Write( char * )");
   }

   return st;
}

/**
 * AcceptConnection method
 *    use base class to accept connections
 *
 *  @returns   a new class instance
 *
 *  Waits for a new connection to service (TCP mode: stream)
 *
 **/
VSocket *Socket::AcceptConnection()
{
   int id;
   VSocket *peer;

   id = this->WaitForConnection();

   peer = new Socket(id);

   return peer;
}