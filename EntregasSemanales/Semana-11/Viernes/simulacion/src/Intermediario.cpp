#include "Intermediario.hpp"

Intermediario::Intermediario(Canal *dc, Canal *hc, Canal *hs, Canal *ds)
{
    this->entrada_c = dc;
    this->salida_c = hc;
    this->salida_s = hs;
    this->entrada_s = ds;
    setLPIP();
}

void Intermediario::setLPIP()
{
    this->solicitud.cuerpo = "";
}

void Intermediario::run()
{
    bool activo = true;
    while (activo)
    {
        msg reqCliente = recibir(*entrada_c);
        std::cout << "[Intermediario] Recibe del cliente: " << reqCliente.raw << "\n";

        if (reqCliente.raw[ELECCION] == 'l')
        {
            solicitud.tipo = REQUEST;
            solicitud.cuerpo = "dir";
        }
        else if (reqCliente.raw[ELECCION] == 'a')
        {
            solicitud.tipo = GET;

            std::string resto = reqCliente.raw.substr(7);
            std::stringstream ss(resto);
            std::string animal, mitad;

            std::getline(ss, animal, '/');
            std::getline(ss, mitad, '/');

            if (animal.empty() || mitad.empty())
            {
                msg error;
                error.raw = "P/D/Solicitud invalida, animal o mitad incorrecta";
                enviar(*salida_c, error);
                continue;
            }

            solicitud.cuerpo = animal + "/" + mitad;
        }
        else if (reqCliente.raw[ELECCION] == 's')
        {
            solicitud.tipo = QUIT;
            solicitud.cuerpo = "";
        }
        else
        {
            msg error;
            error.raw = "P/D/Solicitud invalida";
            enviar(*salida_c, error);
            continue;
        }

        msg reqServer;
        reqServer.raw = solicitud.unificar();

        std::cout << "[Intermediario] Envia al servidor: " << reqServer.raw << "\n";

        enviar(*salida_s, reqServer);

        msg respServer = recibir(*entrada_s);
        std::cout << "[Intermediario] Recibe del servidor: " << respServer.raw << "\n";

        Uppercase respuesta;
        if (!Uppercase::parse(respServer.raw, respuesta))
        {
            msg error;
            error.raw = "P/D/Respuesta invalida del servidor";
            enviar(*salida_c, error);
            continue;
        }
        msg respCliente;

        if (respuesta.tipo == DATA)
        {
            respCliente.raw = respuesta.cuerpo;
        }
        else if (respuesta.tipo == QUIT)
        {
            respCliente.raw = "Cerrando sistema...";
            activo = false;
        }
        else
        {
            respCliente.raw = "Respuesta desconocida";
        }

        enviar(*salida_c, respCliente);
    }
}