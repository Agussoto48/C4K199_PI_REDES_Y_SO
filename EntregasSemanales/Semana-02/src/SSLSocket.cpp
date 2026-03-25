/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2026-i
 *  Grupos: 2 y 3
 *
 *  SSL Socket class implementation
 *
 * (Fedora version)
 *
 **/
#include "SSLSocket.hpp"
#include "Socket.hpp"

/**
 *  Class constructor: se base class
 *  @param char t: socket type to define
 *     's' for stream
 *     'd' for datagram
 *  @param bool ipv6: if we need a IPv6 socket
 **/
SSLSocket::SSLSocket(bool IPv6)
{

   this->Init('s', IPv6);

   this->Context = nullptr;
   this->SSLStruct = nullptr;

   this->InitSSL();
}

/**
 *  Class constructor: use base class
 *  @param char t: socket type to define
 *     's' for stream
 *     'd' for datagram
 *  @param bool IPv6: if we need a IPv6 socket
 *
 **/
SSLSocket::SSLSocket(char *certFileName, char *keyFileName, bool IPv6)
{
}

/**
 *  Class constructor
 *  @param int id: socket descriptor
 **/
SSLSocket::SSLSocket(int id)
{
   this->Init(id);
}

/**
 * Class destructor
 *
 **/
SSLSocket::~SSLSocket()
{
   if (this->SSLStruct != nullptr)
   {
      SSL_shutdown(this->SSLStruct);
      SSL_free(this->SSLStruct);
      this->SSLStruct = nullptr;
   }

   if (this->Context != nullptr)
   {
      SSL_CTX_free(this->Context);
      this->Context = nullptr;
   }

   this->Close();
}

/**
 *  InitSSL: use SSL_new with a defined context
 *  Create a SSL object
 **/
void SSLSocket::InitSSL(bool serverContext)
{
   SSL *ssl = nullptr;

   this->InitContext(serverContext);
   this->SSLStruct = SSL_new(this->Context);

   if (this->SSLStruct == nullptr)
   {
      throw std::runtime_error("Error creating SSL structure");
   }
}

/**
 *  InitContext: use SSL_library_init, OpenSSL_add_all_algorithms, SSL_load_error_strings, TLS_server_method, SSL_CTX_new
 *  Creates a new SSL server context to start encrypted comunications, this context is stored in class instance
 **/
void SSLSocket::InitContext(bool serverContext)
{
   const SSL_METHOD *method;

   if (serverContext)
      method = TLS_server_method();
   else
      method = TLS_client_method();

   this->Context = SSL_CTX_new(method);

   if (nullptr == method)
   {
      throw std::runtime_error("SSLSocket::InitContext( bool )");
   }
}

/**
 *  Load certificates: verify and load certificates
 *  @param	const char * certFileName, file containing certificate
 *  @param	const char * keyFileName, file containing keys
 **/
void SSLSocket::LoadCertificates(const char *certFileName, const char *keyFileName)
{
}

/**
 *  Connect: use SSL_connect to establish a secure conection
 *  Create a SSL connection
 *  @param	char * hostName, host name
 *  @param	int port, service number
 **/
int SSLSocket::Connect(const char *hostName, int port)
{
   int st;

   st = this->TryToConnect(hostName, port);
   if (st == -1)
   {
      throw std::runtime_error("SSLSocket::Read(const char*, int)");
   }
   st = SSL_set_fd(this->SSLStruct, this->sockId);
   if (st != 1)
   {
      throw std::runtime_error("SSLSocket::Connect(const char*, int): SSL_set_fd failed");
   }
   st = SSL_connect(this->SSLStruct);
   if (st != 1)
   {
      throw std::runtime_error("SSLSocket::Connect(const char*, int): SSL_connect failed");
   }
   
   return st;
}

/**
 *  Connect: use SSL_connect to establish a secure conection
 *  Create a SSL connection
 *  @param	char * hostName, host name
 *  @param	char * service, service name
 **/
int SSLSocket::Connect(const char *host, const char *service)
{
   int st;
   st = this->TryToConnect(host, service);

   if (-1 == st)
   {
      throw std::runtime_error("SSLSocket::Read(const char*, const char*)");
   }
   st = SSL_set_fd(this->SSLStruct, this->sockId);
   if (st != 1)
   {
      throw std::runtime_error("SSLSocket::Connect(const char*, const char*): SSL_set_fd failed");
   }
   st = SSL_connect(this->SSLStruct);
   if (st != 1)
   {
      throw std::runtime_error("SSLSocket::Connect(const char*, const char*): SSL_connect failed");
   }
   return st;
}

/**
 *  Read: use SSL_read to read data from an encrypted channel
 *  @param	void * buffer to store data read
 *  @param	size_t size, buffer's capacity
 *  @return	size_t byte quantity read
 *  Reads data from secure channel
 *
 **/
size_t SSLSocket::Read(void *buffer, size_t size)
{
   int st = -1;
   st = SSL_read(this->SSLStruct, buffer, size);

   if (st <= 0)
      throw std::runtime_error("SSLSocket::Read(void*, size_t)");

   return st;
}

/**
 *  Write: use SSL_write to write data to an encrypted channel
 *  @param	const void * buffer to store data to write
 *  @param	size_t size, buffer's capacity
 *  @return	size_t byte quantity written
 *  Reads data from secure channel
 *
 **/
size_t SSLSocket::Write(const void *buffer, size_t size)
{
   int st = -1;
   st = SSL_write(this->SSLStruct, buffer, size);

   if (st <= 0)
      throw std::runtime_error("SSLSocket::Write(const void*, size_t)");

   return st;
}

/**
 *  Write: use SSL_write to write data to an encrypted channel
 *  @param	void * buffer to store data read
 *  @param	size_t size, buffer's capacity
 *  @return	size_t byte quantity written
 *  Writes data to a secure channel
 **/

size_t SSLSocket::Write(const char *string)
{
   return Write(string, strlen(string));
}
/**
 *   Show SSL certificates
 *
 **/
void SSLSocket::ShowCerts()
{
   X509 *cert;
   char *line;

   cert = SSL_get_peer_certificate(this->SSLStruct);
   if (nullptr != cert)
   {
      printf("Server certificates:\n");
      line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
      printf("Subject: %s\n", line);
      free(line);
      line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
      printf("Issuer: %s\n", line);
      free(line);
      X509_free(cert);
   }
   else
   {
      printf("No certificates.\n");
   }
}

/**
 *   Return the name of the currently used cipher
 *
 **/
const char *SSLSocket::GetCipher()
{
   return SSL_get_cipher(this->SSLStruct);
}
