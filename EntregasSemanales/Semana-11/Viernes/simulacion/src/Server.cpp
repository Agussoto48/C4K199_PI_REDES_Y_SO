#include "Server.hpp"

Server::Server(Canal *entrada, Canal *salida)
{
    this->entrada = entrada;
    this->salida = salida;
}

void Server::run()
{
    bool activo = true;
    while (activo)
    {
        msg reqRaw = recibir(*entrada);
        std::cout << "[Servidor] Recibe del intermediario: " << reqRaw.raw << "\n";

        Uppercase req;
        bool ok = Uppercase::parse(reqRaw.raw, req);

        Uppercase resp;
        if (!ok)
        {
            resp.tipo = DATA;
            resp.cuerpo = "Formato Invalido";
        }
        else if (req.tipo == REQUEST)
        {
            if (req.cuerpo == "dir")
            {
                resp.tipo = DATA;
                resp.cuerpo = inventario.obtenerLista();
            }
            else
            {
                resp.tipo = DATA;
                resp.cuerpo = "Request invalido";
            }
        }
        else if (req.tipo == GET)
        {
            std::string animal;
            std::string mitad;

            std::stringstream ss(req.cuerpo);

            std::getline(ss, animal, '/');
            std::getline(ss, mitad, '/');

            if (animal.empty() || mitad.empty())
            {
                resp.tipo = DATA;
                resp.cuerpo = "Parametros invalidos";
            }
            else if (mitad != "1" && mitad != "2")
            {
                resp.tipo = DATA;
                resp.cuerpo = "Mitad invalida";
            }
            else
            {
                std::string piezas = inventario.obtenerAnimal(animal, mitad);

                if (piezas.empty())
                {
                    resp.tipo = DATA;
                    resp.cuerpo = "Figura no encontrada";
                }
                else
                {
                    resp.tipo = DATA;
                    resp.cuerpo = piezas;
                }
            }
        }
        else if (req.tipo == QUIT)
        {
            resp.tipo = QUIT;
            resp.cuerpo = "";
            activo = false;
        }
        else
        {
            resp.tipo = DATA;
            resp.cuerpo = "Tipo no soportado";
        }

        msg out;
        out.raw = resp.unificar();

        std::cout << "[Servidor] Envia al intermediario: " << out.raw << "\n";
        enviar(*salida, out);

        if (ok && req.tipo == QUIT)
            activo = false;
    }
}