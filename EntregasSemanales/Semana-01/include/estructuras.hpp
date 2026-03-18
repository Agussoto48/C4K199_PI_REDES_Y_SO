#pragma once
#define ANIMAL_SIZE 64
#define RESP_SIZE 1028

enum OPCIONES
{
   PEDIR_LISTA = 1,
   PEDIR_ANIMAL = 2
};
enum Etapa
{
   HACIA_INTERMEDIARIO = 1,
   HACIA_SERVIDOR = 2,
   HACIA_CLIENTE = 3
};
struct msg
{
   int opcion;
   int etapa;
   char animal[ANIMAL_SIZE];
   char resp[RESP_SIZE];
};
struct Canal
{
   msg mensaje;
   bool lleno;
   pthread_mutex_t mutex;
   pthread_cond_t cond;
};
struct Contexto
{
   Canal *c_i;
   Canal *i_s;
   Canal *s_i;
   Canal *i_c;
};

// Funciones para el paso de mensajes
void initCanal(Canal &c)
{
   c.lleno = false;
   pthread_mutex_init(&c.mutex, nullptr);
   pthread_cond_init(&c.cond, nullptr);
}

void enviar(Canal &c, const msg &m)
{
   pthread_mutex_lock(&c.mutex);

   while (c.lleno)
   {
      pthread_cond_wait(&c.cond, &c.mutex);
   }

   c.mensaje = m;
   c.lleno = true;

   pthread_cond_broadcast(&c.cond);
   pthread_mutex_unlock(&c.mutex);
}

msg recibir(Canal &c)
{
   pthread_mutex_lock(&c.mutex);

   while (!c.lleno)
   {
      pthread_cond_wait(&c.cond, &c.mutex);
   }
   msg m = c.mensaje;
   c.lleno = false;

   pthread_cond_broadcast(&c.cond);
   pthread_mutex_unlock(&c.mutex);

   return m;
}