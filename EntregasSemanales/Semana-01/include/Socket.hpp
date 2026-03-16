
#pragma once
#include <sys/socket.h> // sockaddr_in
#include <arpa/inet.h>  // ntohs
#include <unistd.h>     // write, read
#include <cstring>
#include <stdexcept>
#include <stdio.h>      // printf
#include "VSocket.hpp"

class Socket : public VSocket
{
public:
   Socket(char, bool = false);
   ~Socket();

   int Connect(const char *, int);
   int Connect(const char *, const char *);

   size_t Read(void *, size_t);
   size_t Write(const void *, size_t);
   size_t Write(const char *);
};
