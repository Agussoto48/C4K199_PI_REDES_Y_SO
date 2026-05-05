#pragma once
#include <string>


class Piezas
{
private:
    std::string serie;
    int cantidad;

public:
    Piezas(std::string serie, int cantidad);
    ~Piezas() = default;

    inline std::string getSerie() { return serie; }
    inline int getCantidad() { return cantidad; }
};
//Funcion para generar numeros aleatorios