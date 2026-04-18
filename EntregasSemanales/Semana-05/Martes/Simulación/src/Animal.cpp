#include "Animal.hpp"

Animal::Animal(std::string nombre)
{
    this->nombre = nombre;

    for (int j = 0; j < NUM_MITADES; ++j)
    {
        longitudes[j] = rng(5, 8);
        for (int i = 0; i < longitudes[j]; ++i)
        {
            conjunto[j].push_back(Piezas(rng(1, 9)));
        }
    }
}

std::string Animal::getMitad(int mitad)
{
    std::string out;

    if (mitad - 1 == MITAD_1)
        mitad = MITAD_1;
    else if (mitad - 1 == MITAD_2)
        mitad = MITAD_2;
    else
        return " ";

    for (int i = 0; i < longitudes[mitad]; ++i)
    {
        if (!out.empty())
            out += ",";

        out += conjunto[mitad][i].getSerie() + "," + std::to_string(conjunto[mitad][i].getCantidad());
    }

    return out;
}