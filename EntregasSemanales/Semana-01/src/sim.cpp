#include <pthread.h>
#include <cstring>
#include <string>
#include "Server.hpp"
#include "estructuras.hpp"

#define ELECCION_ANIMAL "Elefante"
#define OPCION (true)? PEDIR_ANIMAL : PEDIR_LISTA

void *cliente(void *arg)
{
    Contexto *contexto = (Contexto *)arg;

    msg req{};
    req.opcion = OPCION;
    std::strncpy(req.animal, ELECCION_ANIMAL, ANIMAL_SIZE - 1);
    req.animal[ANIMAL_SIZE - 1] = '\0';

    std::cout << "[Cliente] Enviando solicitud\n";
    enviar(*contexto->c_i, req);

    msg final = recibir(*contexto->i_c);

    std::cout << "[Cliente] Respuesta final:\n";
    std::cout << final.resp << std::endl;
    std::cout << "[Cliente] termina\n";

    return nullptr;
}
void *intermediario(void *arg)
{
    Contexto *contexto = (Contexto *)arg;

    msg req = recibir(*contexto->c_i);
    std::cout << "[Intermediario] Recibió del cliente\n";
    std::cout << "[Intermediario] Comunicando con el servidor\n";
    enviar(*contexto->i_s, req);

    msg final = recibir(*contexto->s_i);
    std::cout << "[Intermediario] Recibió del servidor\n";
    enviar(*contexto->i_c, final);

    std::cout << "[Intermediario] termina\n";
    return nullptr;
}
void *servidor(void *arg)
{
    Contexto *contexto = (Contexto *)arg;
    Server server;

    msg req = recibir(*contexto->i_s);
    std::cout << "[Servidor] Procesando solicitud\n";

    msg final{};
    std::string resultado;

    if (req.opcion == PEDIR_LISTA)
        resultado = server.obtenerLista();

    else if (req.opcion == PEDIR_ANIMAL)
        resultado = server.obtenerAnimal(req.animal);

    else
        resultado = "Opcion invalida";
    
    std::strncpy(final.resp, resultado.c_str(), RESP_SIZE - 1);
    final.resp[RESP_SIZE - 1] = '\0';

    enviar(*contexto->s_i, final);

    std::cout << "[Servidor] termina\n";
    return nullptr;
}

int main()
{
    Canal c_i, i_s, s_i, i_c;
    initCanal(c_i);
    initCanal(i_s);
    initCanal(s_i);
    initCanal(i_c);

    Contexto contexto;
    contexto.c_i = &c_i;
    contexto.i_s = &i_s;
    contexto.s_i = &s_i;
    contexto.i_c = &i_c;

    pthread_t thCliente, thInter, thServer;

    pthread_create(&thCliente, nullptr, cliente, &contexto);
    pthread_create(&thInter, nullptr, intermediario, &contexto);
    pthread_create(&thServer, nullptr, servidor, &contexto);

    pthread_join(thCliente, nullptr);
    pthread_join(thInter, nullptr);
    pthread_join(thServer, nullptr);

    return 0;
}