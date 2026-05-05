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

        LPIP req;
        bool ok = LPIP::parse(reqRaw.raw, req);

        LPIP resp;
        if (!ok)
        {
            resp.tipo = FORMATO_INVALIDO;
            resp.cuerpo = "Formato Invalido";
        }
        else if (req.tipo == LIST_QUERY)
        {
            resp.tipo = LIST_RESULT;
            resp.cuerpo = inventario.obtenerLista();
        }
        else if (req.tipo == FIGURE_QUERY)
        {
            std::string animal;
            std::string mitad;
            std::stringstream ss(req.cuerpo);
            std::string token;

            while (std::getline(ss, token, ';'))
            {
                size_t pos = token.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = token.substr(0, pos);
                    std::string value = token.substr(pos + 1);

                    if (key == "figura")
                        animal = value;
                    else if (key == "mitad")
                        mitad = value;
                }
            }
            std::string piezas = inventario.obtenerAnimal(animal, mitad);

            if (mitad != "1" && mitad != "2")
            {
                resp.tipo = MITAD_INVALIDA;
                resp.cuerpo = "Mitad invalida";
            }
            else if (piezas.empty())
            {
                resp.tipo = FIGURE_NOT_FOUND;
                resp.cuerpo = "figura=" + animal;
            }
            else
            {
                resp.tipo = FIGURE_RESULT;
                resp.cuerpo = piezas;
            }
        }
        else if (req.tipo == EXIT)
        {
            resp.tipo = UNREGISTER;
            resp.cuerpo = "cerrando";
        }
        else
        {
            resp.tipo = FIGURE_NOT_FOUND;
            resp.cuerpo = "Tipo no soportado";
        }

        msg out;
        out.raw = resp.unificar();

        std::cout << "[Servidor] Envia al intermediario: " << out.raw << "\n";
        enviar(*salida, out);

        if (ok && req.tipo == EXIT)
            activo = false;
    }
}