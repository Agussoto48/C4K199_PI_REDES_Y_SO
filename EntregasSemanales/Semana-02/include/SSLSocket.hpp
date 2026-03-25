/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2026-i
 *  Grupos: 2 y 3
 *
 *   SSL Socket class interface
 *
 * (Fedora version)
 *
 **/

#pragma once

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdexcept>
#include "VSocket.hpp"

class SSLSocket : public VSocket
{

public:
   SSLSocket(bool IPv6 = false);            // client constructor
   SSLSocket(char *, char *, bool = false); // server constructor
   SSLSocket(int);
   ~SSLSocket();
   int Connect(const char *, int);
   int Connect(const char *, const char *);
   size_t Write(const char *);
   size_t Write(const void *, size_t);
   size_t Read(void *, size_t);
   void ShowCerts();
   const char *GetCipher();

private:
   void InitSSL(bool = false);
   void InitContext(bool);
   void LoadCertificates(const char *, const char *);

   SSL_CTX *Context; 
   SSL *SSLStruct;
};
