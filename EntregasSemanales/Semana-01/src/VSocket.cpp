#include "VSocket.hpp"

/**
 *  Class creator (constructor)
 *     use Unix socket system call
 *
 *  @param     char t: socket type to define
 *     's' for stream
 *     'd' for datagram
 *  @param     bool ipv6: if we need a IPv6 socket
 *
 **/
void VSocket::Init(char t, bool IPv6)
{

   this->IPv6 = IPv6;
   this->type = t;
   this->port = 0;

   int domain = (IPv6) ? AF_INET6 : AF_INET;
   int socketType = (t == 's') ? SOCK_STREAM : SOCK_DGRAM;

   int st = socket(domain, socketType, 0);

   if (-1 == st)
   {
      throw std::runtime_error("VSocket::Init, (reason)");
   }

   this->sockId = st;
}

VSocket::~VSocket()
{

   this->Close();
}

void VSocket::Close()
{
   if (this->sockId >= 0)
   {
      int st = close(this->sockId);

      if (-1 == st)
      {
         throw std::runtime_error("VSocket::Close()");
      }

      this->sockId = -1;
   }
}

/*
   Funciones para organizar mejor las versiones de protoclos, para el TryToConnect con ip
*/
int VSocket::connectIPv4(const char *hostip, int port)
{
   int st = -1;
   struct sockaddr_in host4;
   socklen_t len;
   memset((char *)&host4, 0, sizeof(host4));
   host4.sin_family = AF_INET;

   st = inet_pton(AF_INET, hostip, &host4.sin_addr);
   if (-1 == st)
   {
      throw std::runtime_error("VSocket::TryToConnect, inet_pton");
   }
   host4.sin_port = htons(port);
   len = sizeof(host4);
   st = connect(this->sockId, (struct sockaddr *)&host4, len);
   if (-1 == st)
   {
      throw std::runtime_error("VSocket::TryToConnect, connect");
   }
   return st;
}
int VSocket::connectIPv6(const char *hostip, int port)
{
   int st = -1;
   struct sockaddr_in6 host6;
   struct sockaddr *ha;
   socklen_t len;

   memset(&host6, 0, sizeof(host6));
   host6.sin6_family = AF_INET6;

   st = inet_pton(AF_INET6, hostip, &host6.sin6_addr);
   if (0 <= st)
   { // 0 means invalid address, -1 means address error
      throw std::runtime_error("Socket::Connect( const char *, int ) [inet_pton]");
   }
   host6.sin6_port = htons(port);
   ha = (struct sockaddr *)&host6;
   len = sizeof(host6);
   st = connect(this->sockId, ha, len);
   if (-1 == st)
   {
      throw std::runtime_error("VSocket::TryToConnect( const char *, int ) [connect]");
   }
   return st;
}

/**
 * TryToConnect method
 *   use "connect" Unix system call
 *
 * @param      char * host: host address in dot notation, example "10.84.166.62"
 * @param      int port: process address, example 80
 *
 **/
int VSocket::TryToConnect(const char *hostip, int port)
{
   this->port = port;

   if (!this->IPv6)
      return connectIPv4(hostip, port);
   else
      return connectIPv6(hostip, port);
}

/**
 * TryToConnect method
 *   use "connect" Unix system call
 *
 * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
 * @param      char * service: process address, example "http"
 *
 **/
int VSocket::TryToConnect(const char *host, const char *service)
{
   int st;
   struct addrinfo hints, *result, *rp;

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = 0;
   hints.ai_protocol = 0;

   st = getaddrinfo(host, service, &hints, &result);
   if (0 != st)
   {
      throw std::runtime_error("VSocket::TryToConnect() getaddrinfo");
   }

   // Lista enlazada
   for (rp = result; rp; rp = rp->ai_next)
   {
      st = connect(this->sockId, rp->ai_addr, rp->ai_addrlen);
      if (0 == st)
      {
         break;
      }
   }

   freeaddrinfo(result);

   if (rp == nullptr)
   {
      throw std::runtime_error("VSocket::TryToConnect() connect");
   }

   return st;
}

int VSocket::Bind(int port)
{
   size_t st = -1;
   if (!this->IPv6)
   {
      struct sockaddr_in host4;
      memset(&host4, 0, sizeof(host4));
      host4.sin_family = AF_INET;
      host4.sin_addr.s_addr = htonl(INADDR_ANY);
      host4.sin_port = htons(port);
      st = bind(this->sockId, (struct sockaddr *)&host4, sizeof(host4));
   }
   else
   {
      struct sockaddr_in6 host6;
      memset(&host6, 0, sizeof(host6));
      host6.sin6_family = AF_INET6;
      host6.sin6_addr = in6addr_any;
      host6.sin6_port = htons(port);
      st = bind(this->sockId, (struct sockaddr *)&host6, sizeof(host6));
   }

   return st;
}

/**
 *  sendTo method
 *
 *  @param	const void * buffer: data to send
 *  @param	size_t size data size to send
 *  @param	void * addr address to send data
 *
 *  Send data to another network point (addr) without connection (Datagram)
 *
 **/
size_t VSocket::sendTo(const void *buffer, size_t size, void *addr)
{
   size_t st = -1;
   socklen_t len;

   if (!this->IPv6)
      len = sizeof(struct sockaddr_in);
   else
      len = sizeof(struct sockaddr_in6);

   st = sendto(this->sockId, buffer, size, 0, (struct sockaddr *)addr, len);

   if (st < 0)
   {
      throw std::runtime_error("VSocket::sendTo() sendto");
   }

   return st;
}

/**
 *  recvFrom method
 *
 *  @param	const void * buffer: data to send
 *  @param	size_t size data size to send
 *  @param	void * addr address to receive from data
 *
 *  @return	size_t bytes received
 *
 *  Receive data from another network point (addr) without connection (Datagram)
 *
 **/
size_t VSocket::recvFrom(void *buffer, size_t size, void *addr)
{
   size_t st = -1;
   socklen_t len;

   if (!this->IPv6)
      len = sizeof(struct sockaddr_in);
   else
      len = sizeof(struct sockaddr_in6);
   
   st = recvfrom(this->sockId, buffer, size, 0,(struct sockaddr *)addr, &len);

   if (st < 0)
   {
      throw std::runtime_error("VSocket::recvFrom() recvFrom");
   }

   return st;
}
