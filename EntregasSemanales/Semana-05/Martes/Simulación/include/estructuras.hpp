#pragma once
#include<string>
#include<unistd.h>
#include<pthread.h>
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
   REGISTER = 1,
   UNREGISTER = 2,
   HEARTBEAT = 3,

   LIST_QUERY = 10,
   LIST_RESULT = 11,
   FIGURE_QUERY = 14,
   FIGURE_RESULT = 15,
   FIGURE_NOT_FOUND = 16,

   ACK = 90,
   EXIT = 99,

   FORMATO_INVALIDO = 101,
   DESTINO_NO_DISPONIBLE = 102,
   VERSION_NO_SOPORTADA = 103,
   FIGURA_NO_EXISTE = 104,
   MITAD_INVALIDA = 105,
   TIMEOUT = 106,
   TIPO_NO_SOPORTADO = 107,
   PARAMETROS_INVALIDOS = 108,
   NODO_NO_REGISTRADO = 109,
   OPERACION_NO_PERMITIDA = 110
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