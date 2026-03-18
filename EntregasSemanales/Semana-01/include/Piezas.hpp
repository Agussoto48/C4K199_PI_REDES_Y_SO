#include <string>
#include <random>


int rng(int min, int max);
class Piezas
{
private:
    std::string serie;
    int cantidad;

public:
    Piezas(int cantidad)
    {
        for (int i = 0; i < 5; ++i)
        {
            serie.push_back(rng(65, 90));
        }
        this->cantidad = cantidad;
    }
    ~Piezas() = default;

    inline std::string getSerie() { return serie; }
    inline int getCantidad() { return cantidad; }
};
//Funcion para generar numeros aleatorios
int rng(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    int random_num = distrib(gen);

    return random_num;
}