

/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2026-i
 *  Grupos: 2 y 3
 *
 ****** VSocket base class interface
 *
 * (Fedora version)
 *
 **/

#ifndef VSocket_h
#define VSocket_h
#include <cstdlib>

class VSocket
{
public:
   void Init(char, bool = false);
   void Init(int);
   ~VSocket();

   void Close();
   int TryToConnect(const char *, int);
   int TryToConnect(const char *, const char *);
   virtual int Connect(const char *, int) = 0;
   virtual int Connect(const char *, const char *) = 0;

   virtual size_t Read(void *, size_t) = 0;
   virtual size_t Write(const void *, size_t) = 0;
   virtual size_t Write(const char *) = 0;

   int Bind(int);
   int MarkPassive(int);
   int WaitForConnection(void);
   virtual VSocket *AcceptConnection() = 0;
   int Shutdown(int);

   size_t sendTo(const void *, size_t, void *);
   size_t recvFrom(void *, size_t, void *);

protected:
   int sockId; // Socket identifier
   bool IPv6;  // Is IPv6 socket?
   int port;   // Socket associated port
   char type;  // Socket type (datagram, stream, etc.)
};

#endif // VSocket_h