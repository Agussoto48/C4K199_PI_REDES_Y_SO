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

/** Clase que representa una figura de lego */
class Animal
{
private:
    std::string nombre;
    std::vector<Piezas> conjunto[NUM_MITADES];
    int longitudes[2];

public:
    Animal(std::string nombre, std::vector<Piezas> mitad1, std::vector<Piezas> mitad2);
    ~Animal() = default;

    std::string getMitad(int mitad);
    inline std::string getNombre() { return nombre; }
};