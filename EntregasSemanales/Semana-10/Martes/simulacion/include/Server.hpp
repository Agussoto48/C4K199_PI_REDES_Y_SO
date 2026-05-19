#pragma once
#include "estructuras.hpp"
#include "Inventario.hpp"
#include "LPIP.hpp"
#include <iostream>

/** Procesa las solicitudes recibidas por medio del intermediario.
 * Manda la respuesta al intermediario.
 */
class Server
{
private:
    Canal *entrada;
    Canal *salida;
    Inventario inventario;

public:
    Server(Canal *entrada, Canal *salida);
    void run();
};