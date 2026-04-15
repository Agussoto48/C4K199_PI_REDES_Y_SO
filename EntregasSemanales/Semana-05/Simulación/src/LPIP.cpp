#include "LPIP.hpp"


std::string LPIP::unificar() const
{
    return std::to_string(tipo) + "|" + cuerpo;
}

bool LPIP::parse(const std::string &raw, LPIP &out)
{
    std::vector<std::string> partes;
    std::string actual;

    for (char c : raw)
    {
        if (c == '|')
        {
            partes.push_back(actual);
            actual.clear();
        }
        else
        {
            actual += c;
        }
    }

    partes.push_back(actual);

    if (partes.size() != 2)
        return false;

    out.tipo = static_cast<ID_TIPO>(std::stoi(partes[0]));
    out.cuerpo = partes[1];

    return true;
}