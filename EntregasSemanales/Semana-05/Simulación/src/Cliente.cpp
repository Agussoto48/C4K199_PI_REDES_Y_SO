#include "Cliente.hpp"

int Cliente::Menu()
{
    int opcion;
    std::cout << "\n--- Lego Store ---\n";
    std::cout << "1. Pedir lista\n";
    std::cout << "2. Pedir animal\n";
    std::cout << "3. Salir\n";
    std::cout << "Opcion: ";
    std::cin >> opcion;
    return opcion;
}
void Cliente::pedirEspecificaciones(std::string &animal, int &mitad)
{
    std::cout << "Que animal desea buscar?: ";
    std::getline(std::cin, animal, '\n');
    std::getline(std::cin, animal, '\n');
    std::cout << "Que mitad busca?: ";
    std::cin >> mitad;
}
Cliente::Cliente(Canal *salida, Canal *entrada)
{
    this->salida = salida;
    this->entrada = entrada;
}

void Cliente::run()
{
    bool activo = true;
    int opcion = 1;
    while (activo)
    {
        opcion = Menu();
        msg m{};

        if (opcion == PEDIR_LISTA)
        {
            m.raw = "GET /l";
        }
        else if (opcion == PEDIR_ANIMAL)
        {
            m.raw = "GET /a";
            std::string animal;
            int mitad;
            pedirEspecificaciones(animal, mitad);
            m.raw = m.raw + "/" + animal + "/" + std::to_string(mitad);
        }
        else if (opcion == SALIR)
        {
            m.raw = "GET /s";
            activo = false;
        }
        else
        {
            std::cout << "Opcion invalida\n";
            continue;
        }

        std::cout << "[Cliente] Envía al intermediario: " << m.raw << std::endl;
        // usleep(500000);
        enviar(*salida, m);
        msg r = recibir(*entrada);
        std::cout << "[Cliente] Recive del intermediario: " << r.raw << std::endl;
        std::cout << "\nRespuesta: " << r.raw << "\n";
    }
}