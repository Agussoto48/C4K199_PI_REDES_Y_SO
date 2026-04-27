#pragma once
#include "Animal.hpp"
#define NUM_ANIMALES 4

extern std::vector<std::string> animales;
class Inventario
{
private:
    std::vector<Animal> legos;

public:
    Inventario();
    ~Inventario() = default;

    std::string obtenerLista();
    std::string obtenerAnimal(const std::string &animal, const std::string &mitad);
    
};