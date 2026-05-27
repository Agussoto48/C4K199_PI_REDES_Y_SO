#pragma once
#include "estructuras.hpp"
#include "LPIP.hpp"
#include <iostream>
#define ELECCION 5

/** Recibe peticiones del cliente, manda mensajes lpip al servidor de piezas.
 * Recibe las respuestas y se las pasa al cliente.
 */
class Intermediario
{
private:
    Canal *entrada_c;
    Canal *salida_c;
    Canal *entrada_s;
    Canal *salida_s;
    LPIP solicitud;
    void setLPIP();

public:
    Intermediario(Canal *dc, Canal *hc, Canal *hs, Canal *ds);
    void run();
};