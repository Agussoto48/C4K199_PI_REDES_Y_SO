#pragma once
#include "estructuras.hpp"
#include "Inventario.hpp"
#include "LPIP.hpp"
#include <iostream>

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