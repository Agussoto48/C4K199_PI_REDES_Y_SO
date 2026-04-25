#ifndef SSLSocket_h
#define SSLSocket_h

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "Socket.h"

class SSLSocket : public Socket {
   public:
      SSLSocket();
      SSLSocket( char, bool = false );
      SSLSocket( int );
      SSLSocket( const char *, const char *, bool = false );
      ~SSLSocket();

      void InitContext();
      void Init();
      void InitServerContext();
      void InitServer( const char *, const char * );
      void LoadCertificates( const char *, const char * );

      void Copy( SSLSocket * );
      void CopyContext( SSLSocket * );

      void Accept();
      SSLSocket * AcceptSSL();

      void ShowCerts();
      const char * GetCipher();

      void MakeConnection( const char *, int );

      int Connect( const char *, int );
      int Connect( const char *, const char * );

      size_t Read( void *, size_t );
      size_t Write( const void *, size_t );
      size_t Write( const char * );

      VSocket * AcceptConnection();

   private:
      SSL_CTX * ctx;
      SSL * ssl;
      int ownsContext;
};

#endif