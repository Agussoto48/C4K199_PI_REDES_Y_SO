#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include "estructuras.hpp"


class Cliente
{
private:
    Canal *salida;
    Canal *entrada;
    std::string mensaje_http;

    int Menu();
    void pedirEspecificaciones(std::string &animal, int &mitad);

public:
    Cliente(Canal *salida, Canal *entrada);
    void run();
};
