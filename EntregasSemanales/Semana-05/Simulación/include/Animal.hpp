#pragma once
#include "Piezas.hpp"
#include <vector>
#include <iostream>
#define NUM_MITADES 2


enum Mitades
{
    MITAD_1 = 0,
    MITAD_2 = 1
};

class Animal
{
private:
    std::string nombre;
    std::vector<Piezas> conjunto[NUM_MITADES];
    int longitudes[2];

public:
    Animal(std::string nombre);
    ~Animal() = default;

    std::string getMitad(int mitad);
    inline std::string getNombre() { return nombre; }
};