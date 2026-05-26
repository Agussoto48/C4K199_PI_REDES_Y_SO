#pragma once
#include <string>
#include <unistd.h>
#include <pthread.h>
#define ANIMAL_SIZE 64
#define RESP_SIZE 1028

enum OPCIONES
{
   PEDIR_LISTA = 1,
   PEDIR_ANIMAL = 2,
   SALIR = 3
};

enum ID_TIPO
{
   ACK = 'A',
   CONNECT = 'C',
   DATA = 'D',
   FORK = 'F',
   GET = 'G',
   KILL = 'K',
   PATCH = 'P',
   QUIT = 'Q',
   REQUEST = 'R',
   SERVER_DATA = 'S'
};
struct msg
{
   std::string raw = "";
};
struct Canal
{
   msg mensaje;
   bool lleno;
   pthread_mutex_t mutex;
   pthread_cond_t cond;
};

// Funciones para el paso de mensajes
void initCanal(Canal &c);
void enviar(Canal &c, const msg &m);
msg recibir(Canal &c);