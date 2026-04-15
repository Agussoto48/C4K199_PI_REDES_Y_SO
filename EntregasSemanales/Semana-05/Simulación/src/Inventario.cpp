#include "Inventario.hpp"

std::vector<std::string> animales =
    {
        "Caballo", "Jirafa", "Elefante",
        "Raton", "Mapache", "Paloma"
    };

Inventario::Inventario()
{
    for (int i = 0; i < NUM_ANIMALES; ++i)
    {
        legos.push_back(Animal(animales[i]));
    }
}

std::string Inventario::obtenerLista()
{
    std::string out;

    for (int i = 0; i < NUM_ANIMALES; ++i)
    {
        if (!out.empty())
            out += ",";

        out += legos[i].getNombre();
    }

    return out;
}
std::string Inventario::obtenerAnimal(const std::string &animal, const std::string &mitad)
{
    for (int i = 0; i < NUM_ANIMALES; ++i)
    {
        if (animal == legos[i].getNombre())
        {
            return legos[i].getMitad(stoi(mitad));
        }
    }

    return "";
}