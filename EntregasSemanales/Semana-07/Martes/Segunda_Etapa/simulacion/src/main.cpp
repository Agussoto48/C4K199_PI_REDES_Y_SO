#include <pthread.h>
#include <cstring>
#include <string>
#include "Inventario.hpp"
#include "Cliente.hpp"
#include "Intermediario.hpp"
#include "Server.hpp"

void *hiloCliente(void *arg)
{
    static_cast<Cliente *>(arg)->run();
    return nullptr;
}

void *hiloIntermediario(void *arg)
{
    static_cast<Intermediario *>(arg)->run();
    return nullptr;
}

void *hiloServidor(void *arg)
{
    static_cast<Server *>(arg)->run();
    return nullptr;
}
int main()
{

    Canal c_i, i_c, i_s, s_i;
    initCanal(c_i);
    initCanal(i_c);
    initCanal(i_s);
    initCanal(s_i);

    Cliente cliente(&c_i, &i_c);
    Intermediario inter(&c_i, &i_c, &i_s, &s_i);
    Server server(&i_s, &s_i);

    pthread_t thCliente, thIntermediario, thServidor;

    pthread_create(&thCliente, nullptr, hiloCliente, &cliente);
    pthread_create(&thIntermediario, nullptr, hiloIntermediario, &inter);
    pthread_create(&thServidor, nullptr, hiloServidor, &server);

    pthread_join(thCliente, nullptr);
    pthread_join(thIntermediario, nullptr);
    pthread_join(thServidor, nullptr);
    
    return 0;
}