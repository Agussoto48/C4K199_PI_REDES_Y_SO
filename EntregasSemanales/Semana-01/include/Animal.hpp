#include "Piezas.hpp"
#include <vector>
#include <iostream>

class Animal
{
private:
    std::string nombre;
    std::vector<Piezas> conjunto;

public:
    Animal(std::string nombre)
    {
        this->nombre = nombre;
        int len = rng(5, 8);
        for (int i = 0; i < len; ++i)
        {
            conjunto.push_back(Piezas(rng(1, 9)));
        }
    }
    ~Animal() = default;

    //Esta funcion no la tuve en mente con paso de mensajes, el toString si logra esa falla
    void print_info()
    {
        std::cout << "Información de " << nombre << std::endl;
        for (int i = 0; i < conjunto.size(); ++i)
        {
            std::cout << "\tSerie: " << conjunto[i].getSerie() << std::endl;
            std::cout << "\t\tCant. de piezas: " << conjunto[i].getCantidad() << std::endl;
        }
    }
    std::string toString()
    {
        std::string out = "Información de " + nombre + "\n";

        for (int i = 0; i < conjunto.size(); ++i)
        {
            out += "\tSerie: " + conjunto[i].getSerie() + "\n";
            out += "\tCant. de piezas: " + std::to_string(conjunto[i].getCantidad()) + "\n";
        }

        return out;
    }
    inline std::string getNombre() { return nombre; }
};