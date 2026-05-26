#include "estructuras.hpp"

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