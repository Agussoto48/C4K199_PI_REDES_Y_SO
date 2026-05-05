#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include "estructuras.hpp"

/** Se mestra menú  y el usuario pide lista o figura específica.
 * Manda peticiin a intermediario
 * Recibe las respuestas del intermediario y se las muestra al usuario*/
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
