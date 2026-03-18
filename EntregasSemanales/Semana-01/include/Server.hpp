#include "Animal.hpp"
#define NUM_ANIMALES 6

std::vector<std::string> animales =
    {
        "Caballo", "Jirafa", "Elefante",
        "Raton", "Mapache", "Paloma"

};
class Server
{
private:
    std::vector<Animal> legos;

public:
    // Como esto es una simulación, ya van a ver legos definidos
    Server()
    {
        for (int i = 0; i < NUM_ANIMALES; ++i)
        {
            legos.push_back(Animal(animales[i]));
        }
    }
    ~Server() = default;

    std::string obtenerLista()
    {
        std::string out;

        for (int i = 0; i < NUM_ANIMALES; ++i)
        {
            out += std::to_string(i + 1) + ") " + legos[i].getNombre() + "\n";
        }

        return out;
    }
    std::string obtenerAnimal(const std::string &animal)
    {
        for (int i = 0; i < NUM_ANIMALES; ++i)
        {
            if (animal == legos[i].getNombre())
            {
                return legos[i].toString();
            }
        }

        return "Animal no encontrado\n";
    }
};