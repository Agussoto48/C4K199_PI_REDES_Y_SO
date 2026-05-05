#include "Animal.hpp"

Animal::Animal(std::string nombre, std::vector<Piezas> mitad1, std::vector<Piezas> mitad2)
{
    this->nombre = nombre;
    conjunto[MITAD_1] = mitad1;
    conjunto[MITAD_2] = mitad2;
    longitudes[MITAD_1] = mitad1.size();
    longitudes[MITAD_2] = mitad2.size();
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

    int total = 0;
    for (int i = 0; i < longitudes[mitad]; ++i)
    {
        if (!out.empty())
            out += ",";

        out += conjunto[mitad][i].getSerie() + "," + std::to_string(conjunto[mitad][i].getCantidad());
        total += conjunto[mitad][i].getCantidad();
    }

    out += ",total," + std::to_string(total);

    return out;
}