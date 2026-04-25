
/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2026-i
 *  Grupos: 2 y 3
 *
 ****** VSocket base class implementation
 *
 * (Fedora version)
 *
 **/

#include <sys/socket.h>
#include <arpa/inet.h> // ntohs, htons
#include <stdexcept>   // runtime_error
#include <cstring>     // memset
#include <netdb.h>     // getaddrinfo, freeaddrinfo
#include <unistd.h>    // close
#include <cstddef>
#include <cstdio>

// #include <sys/types.h>

#include "VSocket.h"

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

   int estado = -1;    // contiene el estado de las llamadas al sistema
   int dominio;        // define el dominio del socket
   int tipoSocket = 0; // define el tipo de socket

   // Determina si el socket utiliza IPv6 o IPv4
   if (IPv6)
   {
      dominio = AF_INET6;
   }
   else
   {
      dominio = AF_INET;
   }

   // Guarda los valores en los atributos del objeto
   this->type = t;
   this->IPv6 = IPv6;
   this->port = 0;
   this->sockId = -1;

   // Determina el tipo de socket
   if ('s' == t)
   {
      tipoSocket = SOCK_STREAM;
   }
   else if ('d' == t)
   {
      tipoSocket = SOCK_DGRAM;
   }
   else
   {
      // error si el tipo no es valido
      throw std::runtime_error("VSocket::Init, tipo de socket invalido");
   }

   // Debuging
   printf("t = %c, IPv6 = %d, dominio = %d, tipoSocket = %d\n", t, IPv6, dominio, tipoSocket);

   estado = socket(dominio, tipoSocket, 0); // Se crea el socket

   // si no funciono la creacion del socket da error
   if (-1 == estado)
   {
      perror("socket");
      throw std::runtime_error("VSocket::Init, error al crear el socket");
   }

   // Guarda el id del socket
   this->sockId = estado;
}

/**
 *  Class creator (constructor)
 *     use Unix socket system call
 *
 *  @param     int id: socket identifier
 *
 **/
void VSocket::Init(int id)
{

   // Se Construye el objeto usando un descriptor ya creado
   this->sockId = id;
   this->IPv6 = false;
   this->port = 0;
   this->type = 's'; // se asume TCP

   if (this->sockId < 0)
   {
      throw std::runtime_error("VSocket::Init( int )");
   }
}

/**
 * Class destructor
 *
 **/
VSocket::~VSocket()
{

   this->Close();
}

/**
 * Close method
 *    use Unix close system call (once opened a socket is managed like a file in Unix)
 *
 **/
void VSocket::Close()
{
   int st = 0;

   if (this->sockId != -1)
   {
      st = close(this->sockId);

      if (-1 == st)
      {
         throw std::runtime_error("VSocket::Close()");
      }

      this->sockId = -1;
   }
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

   int estado = -1;                        // contiene el estado de las llamadas al sistema
   struct sockaddr *direccion;             // Puntero a estructura de direccion de socket
   socklen_t longitud;                     // Longitud de la estructura de direccion utilizada
   struct sockaddr_in direccionServidor4;  // Estructura de direccion para IPv4
   struct sockaddr_in6 direccionServidor6; // Estructura de direccion para IPv6

   // Si el socket fue creado para IPv6 se utiliza sockaddr_in6
   if (this->IPv6)
   {
      // Inicializa la estructura de direccion IPv6 en cero
      memset(&direccionServidor6, 0, sizeof(direccionServidor6));

      // Especifica la familia de direcciones IPv6
      direccionServidor6.sin6_family = AF_INET6;

      // Convierte la direccion IPv6 de texto a formato binario
      estado = inet_pton(AF_INET6, hostip, &direccionServidor6.sin6_addr);
      if (estado <= 0)
      {
         throw std::runtime_error("VSocket::TryToConnect, error en inet_pton IPv6");
      }

      // Convierte el puerto al formato de red
      direccionServidor6.sin6_port = htons(port);

      // Asigna la direccion IPv6 al puntero
      direccion = (struct sockaddr *)&direccionServidor6;

      // Guarda el tamaño de la estructura IPv6
      longitud = sizeof(direccionServidor6);
   }
   else
   {

      // Inicializa la estructura de direccion IPv6 en cero
      memset(&direccionServidor4, 0, sizeof(direccionServidor4));

      // Especifica la familia de direcciones IPv4
      direccionServidor4.sin_family = AF_INET;

      // Convierte la direccion IPv6 de texto a formato binario
      estado = inet_pton(AF_INET, hostip, &direccionServidor4.sin_addr);
      if (estado <= 0)
      {
         throw std::runtime_error("VSocket::TryToConnect, error en inet_pton IPv4");
      }

      // Convierte el puerto al formato de red
      direccionServidor4.sin_port = htons(port);

      // Asigna la direccion IPv4 al puntero
      direccion = (struct sockaddr *)&direccionServidor4;

      // Guarda el tamaño de la estructura IPv4
      longitud = sizeof(direccionServidor4);
   }

   // Intenta establecer la conexion con el servidor remoto
   estado = connect(this->sockId, direccion, longitud);

   // Si no se logra la conexion da error
   if (-1 == estado)
   {
      throw std::runtime_error("VSocket::TryToConnect( const char *, int ), error al establecer la conexion");
   }

   this->port = port;

   return estado;
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

   int estado = -1;                  // contiene el estado de las llamadas al sistema
   int tipoSocket;                   // define el tipo de socket
   struct addrinfo pistas;           // contiene pistas para la busqueda de direcciones
   struct addrinfo *resultado;       // Puntero al inicio de la lista de direcciones que devuelve etaddrinfo
   struct addrinfo *direccionActual; // Puntero para recorrer la lista de direcciones

   // Determina el tipo de socket
   if ('s' == this->type)
   {
      tipoSocket = SOCK_STREAM;
   }
   else
   {
      tipoSocket = SOCK_DGRAM;
   }

   // Inicializa la estructura de pistas en cero
   memset(&pistas, 0, sizeof(struct addrinfo));

   // Permitie que getaddrinfo busque direcciones IPv4 o IPv6
   pistas.ai_family = AF_UNSPEC;

   // Indica el tipo de socket requerido
   pistas.ai_socktype = tipoSocket;

   // No hay flags adicionales
   pistas.ai_flags = 0;

   // No se restringe el protocolo
   pistas.ai_protocol = 0;

   // nombre del host y del servicio en una lista de direcciones
   estado = getaddrinfo(host, service, &pistas, &resultado);

   // Si ocurre un error en la resolucion del host da error
   if (0 != estado)
   {
      throw std::runtime_error("VSocket::TryToConnect, error al resolver el nombre del host");
   }

   // Recorre la lista de direcciones intentando conectar con cada una
   for (direccionActual = resultado; direccionActual; direccionActual = direccionActual->ai_next)
   {

      // Intentar establecer conexion con la direccion actual
      estado = connect(this->sockId, direccionActual->ai_addr, direccionActual->ai_addrlen);

      // Si la conexion fue exitosa se sale del ciclo
      if (0 == estado)
      {
         break;
      }
   }

   freeaddrinfo(resultado);

   // Si no se pudo hacer ninguna conexion da error
   if (0 != estado)
   {
      throw std::runtime_error("VSocket::TryToConnect [connect]");
   }

   return estado;
}

/**
 * Bind method
 *    use "bind" Unix system call (man 3 bind) (server mode)
 *
 * @param      int port: bind a unamed socket to a port defined in sockaddr structure
 *
 *  Links the calling process to a service at port
 *
 **/
int VSocket::Bind(int port)
{
   int st = -1;

   // Selecciona la estructura de direccion segun el tipo de IP
   if (this->IPv6)
   {
      struct sockaddr_in6 host6;

      // Inicializa la estructura en cero
      memset(&host6, 0, sizeof(host6));

      // Configura la direccion IPv6
      host6.sin6_family = AF_INET6;
      host6.sin6_addr = in6addr_any;
      host6.sin6_port = htons(port);

      // Asocia el socket al puerto
      st = bind(this->sockId, (struct sockaddr *)&host6, sizeof(host6));
   }
   else
   {
      struct sockaddr_in host4;

      // Configura la direccion IPv4
      host4.sin_family = AF_INET;
      host4.sin_addr.s_addr = htonl(INADDR_ANY);
      host4.sin_port = htons(port);

      // Limpia el campo sin_zero
      memset(host4.sin_zero, '\0', sizeof(host4.sin_zero));

      // Asocia el socket al puerto
      st = bind(this->sockId, (struct sockaddr *)&host4, sizeof(host4));
   }

   // Verifica si hubo error
   if (-1 == st)
   {
      throw std::runtime_error("VSocket::Bind");
   }

   // Guarda el puerto asociado al socket
   this->port = port;

   return st;
}

/**
 * MarkPassive method
 *    use "listen" Unix system call (man listen) (server mode)
 *
 * @param      int backlog: defines the maximum length to which the queue of pending connections for this socket may grow
 *
 *  Establish socket queue length
 *
 **/
int VSocket::MarkPassive(int backlog)
{
   int st = -1;

   // Pone el socket en modo servidor , o sea a escuchar conexiones
   st = listen(this->sockId, backlog);

   if (-1 == st)
   {
      throw std::runtime_error("VSocket::MarkPassive");
   }

   return st;
}

/**
 * WaitForConnection method
 *    use "accept" Unix system call (man 3 accept) (server mode)
 *
 *
 *  Waits for a peer connections, return a sockfd of the connecting peer
 *
 **/
int VSocket::WaitForConnection(void)
{
   int st = -1;
   struct sockaddr_in client_addr;
   socklen_t len = sizeof(client_addr);

   memset(&client_addr, 0, sizeof(client_addr));

   // Espera una conexion entrante y devuelve un nuevo descriptor
   st = accept(this->sockId, (struct sockaddr *)&client_addr, &len);

   if (-1 == st)
   {
      throw std::runtime_error("VSocket::WaitForConnection");
   }

   return st;
}

/**
 * Shutdown method
 *    use "shutdown" Unix system call (man 3 shutdown) (server mode)
 *
 *
 *  cause all or part of a full-duplex connection on the socket associated with the file descriptor socket to be shut down
 *
 **/
int VSocket::Shutdown(int mode)
{
   int st = -1;

   // Cierra parcial o totalmente la conexion (de lectura o escritura
   st = shutdown(this->sockId, mode);

   if (-1 == st)
   {
      throw std::runtime_error("VSocket::Shutdown");
   }

   return st;
}

// UDP methods 2025

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
   ssize_t st = -1;
   socklen_t longitud = 0;

   // Determina el tamano de la estructura de direccion segun el tipo de IP
   if (this->IPv6)
   {
      longitud = sizeof(struct sockaddr_in6);
   }
   else
   {
      longitud = sizeof(struct sockaddr_in);
   }

   // Envia el datagrama al destino especificado
   st = sendto(this->sockId, buffer, size, 0, (struct sockaddr *)addr, longitud);

   if (-1 == st)
   {
      throw std::runtime_error("VSocket::sendTo");
   }

   // Retorna la cantidad de bytes enviados
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
   ssize_t st = -1;
   socklen_t longitud = 0;

   // Determina el tamaño de la estructura de direccion segun el tipo de IP
   if (this->IPv6)
   {
      longitud = sizeof(struct sockaddr_in6);
   }
   else
   {
      longitud = sizeof(struct sockaddr_in);
   }

   // Recibe el datagrama y guarda tambien la direccion del emisor en addr
   st = recvfrom(this->sockId, buffer, size, 0, (struct sockaddr *)addr, &longitud);

   if (-1 == st)
   {
      throw std::runtime_error("VSocket::recvFrom");
   }

   // Retorna la cantidad de bytes recibidos
   return st;
}
