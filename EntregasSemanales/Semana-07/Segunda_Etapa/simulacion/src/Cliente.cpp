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
    std::cout << "Ingrese el animal y la mitad (0=ambas, 1=primera, 2=segunda) (ej: pez 1): ";
    std::cin >> animal >> mitad;
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
            std::string animal;
            int mitad;
            pedirEspecificaciones(animal, mitad);

            if (mitad == 0)
            {
                for (int i = 1; i <= 2; ++i)
                {
                    m.raw = "GET /a/" + animal + "/" + std::to_string(i);
                    std::cout << "[Cliente] Envía al intermediario: " << m.raw << std::endl;
                    enviar(*salida, m);
                    msg r = recibir(*entrada);
                    std::cout << "[Cliente] Recive del intermediario: " << r.raw << std::endl;
                    std::cout << "\nMitad " << i << ": " << r.raw << "\n";
                }
                continue;
            }

            m.raw = "GET /a/" + animal + "/" + std::to_string(mitad);
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
        enviar(*salida, m);
        msg r = recibir(*entrada);
        std::cout << "[Cliente] Recive del intermediario: " << r.raw << std::endl;
        std::cout << "\nRespuesta: " << r.raw << "\n";
    }
}