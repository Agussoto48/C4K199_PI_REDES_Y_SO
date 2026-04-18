#pragma once
#include <string>
#include <random>


int rng(int min, int max);
extern std::vector<std::string> colores;


class Piezas
{
private:
    std::string serie;
    int cantidad;

public:
    Piezas(int cantidad);
    ~Piezas() = default;

    inline std::string getSerie() { return serie; }
    inline int getCantidad() { return cantidad; }
};
//Funcion para generar numeros aleatorios