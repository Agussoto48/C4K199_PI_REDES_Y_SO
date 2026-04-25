#include <iostream>
#include <cstring>
#include <stdexcept>

#include "SSLSocket.h"

using namespace std;

/**
 * Constructor para crear un socket SSL (cliente o servidor base)
 *
 * Inicializa la estructura heredada de Socket (TCP) y prepara
 * los punteros de SSL en null. No crea aun el contexto SSL,
 * esto se hace despues dependiendo si es cliente o servidor.
 *
 * @param t tipo de socket ('s' para stream)
 * @param IPv6 indica si se usa IPv6
 */
SSLSocket::SSLSocket(char t, bool IPv6) : Socket(t, IPv6)
{
   this->ctx = nullptr;
   this->ssl = nullptr;
   this->ownsContext = 1;
}

SSLSocket::SSLSocket(const char *certFile, const char *keyFile, bool IPv6)
    : Socket('s', IPv6)
{

   this->ctx = nullptr;
   this->ssl = nullptr;
   this->ownsContext = 1;

   this->InitServer(certFile, keyFile);
}
/**
 * Constructor usado cuando ya existe una conexion TCP aceptada
 *
 * Este constructor se utiliza en el servidor despues de un accept(),
 * donde ya existe un descriptor de socket valido.
 *
 * @param id descriptor del socket aceptado
 */
SSLSocket::SSLSocket(int id) : Socket(id)
{
   this->ctx = nullptr;
   this->ssl = nullptr;
   this->ownsContext = 1;
}

/**
 * Destructor
 *
 * Libera la memoria asociada al objeto SSL y al contexto.
 * El contexto solo se libera si este objeto es el dueño,
 * evitando liberar contextos compartidos entre multiples clientes.
 */
SSLSocket::~SSLSocket()
{
   if (this->ssl != nullptr)
   {
      SSL_free(this->ssl);
      this->ssl = nullptr;
   }

   if (this->ctx != nullptr && this->ownsContext == 1)
   {
      SSL_CTX_free(this->ctx);
      this->ctx = nullptr;
   }
}

/**
 * Inicializa el contexto SSL para cliente
 *
 * Este metodo prepara el entorno OpenSSL:
 *  - Registra algoritmos de cifrado
 *  - Carga mensajes de error
 *  - Crea un contexto SSL para cliente usando TLS
 *
 * Este contexto se usara luego para crear la conexion segura.
 */
void SSLSocket::InitContext()
{
   SSL_library_init();
   SSL_load_error_strings();
   OpenSSL_add_ssl_algorithms();

   this->ctx = SSL_CTX_new(TLS_client_method());

   if (this->ctx == nullptr)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSLSocket::InitContext()");
   }
}

/**
 * Inicializa la estructura SSL asociada al socket
 *
 * Este metodo crea un objeto SSL a partir del contexto previamente
 * creado y lo asocia al descriptor de socket (sockId).
 *
 * Despues de aqui la comunicacion puede ser cifrada.
 */
void SSLSocket::Init()
{
   this->ssl = SSL_new(this->ctx);

   if (this->ssl == nullptr)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSLSocket::Init()");
   }

   SSL_set_fd(this->ssl, this->sockId);
}

/**
 * Inicializa el contexto SSL para servidor
 *
 * Es parecido a InitContext, pero usando TLS_server_method(),
 * lo cual permite aceptar conexiones seguras entrantes.
 */
void SSLSocket::InitServerContext()
{
   SSL_library_init();
   SSL_load_error_strings();
   OpenSSL_add_ssl_algorithms();

   this->ctx = SSL_CTX_new(TLS_server_method());

   if (this->ctx == nullptr)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSLSocket::InitServerContext()");
   }
}

/**
 * Inicializa el servidor SSL cargando certificados
 *
 * Este metodo configura el servidor para usar SSL:
 *  - Crea el contexto SSL
 *  - Carga el certificado publico
 *  - Carga la llave privada
 *  - Verifica que coincidan
 *
 * @param certFile archivo .pem con certificado
 * @param keyFile archivo .pem con llave privada
 */
void SSLSocket::InitServer(const char *certFile, const char *keyFile)
{
   this->InitServerContext();

   if (SSL_CTX_use_certificate_file(this->ctx, certFile, SSL_FILETYPE_PEM) <= 0)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSLSocket::InitServer() certificate");
   }

   if (SSL_CTX_use_PrivateKey_file(this->ctx, keyFile, SSL_FILETYPE_PEM) <= 0)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSLSocket::InitServer() private key");
   }

   if (!SSL_CTX_check_private_key(this->ctx))
   {
      throw runtime_error("SSLSocket::InitServer() key mismatch");
   }
}

/**
 * Copia el contexto SSL de otro socket , por lo general del servidor
 *
 * Este metodo se utiliza en servidores concurrentes.
 * Cada cliente obtiene:
 *  - Su propio objeto SSL
 *  - Pero comparte el mismo contexto SSL del servidor
 *
 * @param original socket servidor
 */
void SSLSocket::CopyContext(SSLSocket *original)
{
   this->ctx = original->ctx;
   this->ownsContext = 0;

   this->ssl = SSL_new(this->ctx);

   if (this->ssl == nullptr)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSLSocket::CopyContext()");
   }

   SSL_set_fd(this->ssl, this->sockId);
}

void SSLSocket::Copy(SSLSocket *original)
{
   this->CopyContext(original);
}

/**
 * Muestra informacion del certificado del peer
 *
 * Obtiene el certificado del otro extremo (cliente o servidor)
 * y muestra:
 *  - Subject (identidad)
 *  - Issuer (quien lo firmo)
 *
 * Si el peer no envia certificado, se indcia .
 */
void SSLSocket::ShowCerts()
{
   X509 *cert;
   char *line;

   cert = SSL_get_peer_certificate(this->ssl);

   if (cert != nullptr)
   {
      cout << "Certificado del peer:" << endl;

      line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
      cout << "Subject: " << line << endl;
      OPENSSL_free(line);

      line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
      cout << "Issuer: " << line << endl;
      OPENSSL_free(line);

      X509_free(cert);
   }
   else
   {
      cout << "No hay certificado del peer." << endl;
   }
}

/**
 * Obtiene el algoritmo de cifrado activo en la conexion
 *
 * Este metodo permite conocer que tipo de cifrado TLS
 * se esta utilizando en la comunicacion actual.
 *
 * @return nombre del cipher
 */
const char *SSLSocket::GetCipher()
{
   return SSL_get_cipher(this->ssl);
}

/**
 * Establece una conexion segura con el servidor
 *
 * Realiza:
 *  1. Conexion TCP normal
 *  2. Inicializacion SSL
 *  3. Handshake TLS con SSL_connect
 *
 * @param host direccion del servidor
 * @param port puerto
 */
int SSLSocket::Connect(const char *host, int port)
{
   int st;

   st = Socket::Connect(host, port);

   this->InitContext();
   this->Init();

   if (SSL_connect(this->ssl) <= 0)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSL_connect failed");
   }

   return st;
}

/**
 * Lee datos desde la conexion SSL
 *
 * Utiliza SSL_read para recibir datos cifrados y
 * descifrarlos automaticamente antes de entregarlos.
 *
 * @param buffer destino de los datos
 * @param size tamaño del buffer
 * @return cantidad de bytes leidos
 */
size_t SSLSocket::Read(void *buffer, size_t size)
{
   int st;

   st = SSL_read(this->ssl, buffer, size);

   if (st <= 0)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSL_read failed");
   }

   return st;
}

/**
 * Escribe datos en la conexion SSL
 *
 * Utiliza SSL_write para cifrar los datos antes
 * de enviarlos al otro extremo.
 *
 * @param buffer datos a enviar
 * @param size cantidad de bytes
 * @return cantidad de bytes enviados
 */
size_t SSLSocket::Write(const void *buffer, size_t size)
{
   int st;

   st = SSL_write(this->ssl, buffer, size);

   if (st <= 0)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSL_write failed");
   }

   return st;
}

/**
 * Version simplificada de Write para cadenas de texto
 *
 * @param text texto a enviar
 */
size_t SSLSocket::Write(const char *text)
{
   return this->Write(text, strlen(text));
}

/**
 * Realiza el handshake SSL en el servidor
 *
 * Este metodo inicia el protocolo TLS usando SSL_accept,
 * permitiendo establecer una conexion segura con el cliente.
 */
void SSLSocket::Accept()
{
   if (SSL_accept(this->ssl) <= 0)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSL_accept failed");
   }
}

/**
 * Acepta una conexion TCP entrante
 *
 * Este metodo solo maneja la parte TCP:
 *  - Espera un cliente
 *  - Crea un nuevo objeto SSLSocket
 *
 * El handshake SSL se realiza despues con Accept()
 *
 * @return nuevo socket cliente
 */
VSocket *SSLSocket::AcceptConnection()
{
   int id;
   SSLSocket *peer;

   id = this->WaitForConnection();

   peer = new SSLSocket(id);

   return peer;
}
void SSLSocket::MakeConnection(const char *host, int port)
{
   this->Connect(host, port);
}

int SSLSocket::Connect(const char *host, const char *service)
{
   int st;

   st = Socket::Connect(host, service);

   this->InitContext();
   this->Init();

   if (SSL_connect(this->ssl) <= 0)
   {
      ERR_print_errors_fp(stderr);
      throw runtime_error("SSLSocket::Connect(host, service) SSL_connect");
   }

   return st;
}