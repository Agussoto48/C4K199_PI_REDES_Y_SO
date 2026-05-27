#include "Inventario.hpp"

std::vector<std::string> animales = { "pez", "caballo", "tortuga", "serpiente" };

Inventario::Inventario()
{
    legos.push_back(Animal("pez",
        {
            Piezas("brick1x1 blue", 4),
            Piezas("brick2x2 blue", 2),
            Piezas("plate1x4 yellow", 1)
        },
        {
            Piezas("brick1x2 blue", 5),
            Piezas("plate2x2 blue", 2),
            Piezas("tile1x1 gray", 1)
        }
    ));

    legos.push_back(Animal("caballo",
        {
            Piezas("brick2x3 yellow", 1),
            Piezas("brick1x1 eye yellow", 2),
            Piezas("brick2x1 yellow", 2),
            Piezas("brick2x2 white", 1)
        },
        {
            Piezas("brick2x2 yellow", 5),
            Piezas("brick2x4 white", 4),
            Piezas("brick2x4 yellow", 2),
            Piezas("brick2x6 yellow", 1)
        }
    ));

    legos.push_back(Animal("tortuga",
        {
            Piezas("plate2x4 black", 2),
            Piezas("brick2x2 yellow", 1),
            Piezas("brick1x2 yellow", 2),
            Piezas("brick1x2 slope yellow", 2)
        },
        {
            Piezas("brick2x2 slope inv yellow", 1),
            Piezas("brick2x2 trans blue", 2),
            Piezas("brick1x1 round eye yellow", 2)
        }
    ));

    legos.push_back(Animal("serpiente",
        {
            Piezas("brick2x6 green", 1),
            Piezas("brick2x2 black", 1)
        },
        {
            Piezas("brick2x2 green", 2),
            Piezas("brick1x1 black", 2),
            Piezas("brick1x1 green", 2)
        }
    ));
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
