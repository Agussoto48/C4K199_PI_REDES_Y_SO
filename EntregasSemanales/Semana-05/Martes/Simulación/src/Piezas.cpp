#include "Piezas.hpp"

Piezas::Piezas(int cantidad)
{

    serie.push_back(rng(50, 56));
    serie.push_back('x');
    serie.push_back(rng(50, 56));

    int num_color = rng(0, colores.size() - 1);
    serie = serie + " " + colores[num_color];

    this->cantidad = cantidad;
}

int rng(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    int random_num = distrib(gen);

    return random_num;
}
std::vector<std::string> colores  =
{
    "Rojo", "Azul", "Amarillo",
    "Rosado", "Morado", "Verde",
    "Celeste", "Negro", "Blanco"
};