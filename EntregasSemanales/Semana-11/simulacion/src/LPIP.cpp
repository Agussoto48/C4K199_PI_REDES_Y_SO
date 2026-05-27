#include "LPIP.hpp"

std::string LPIP::unificar() const
{
    return "P/" + std::string(1, static_cast<char>(tipo)) + "/" + cuerpo;
}

bool LPIP::parse(const std::string &raw, LPIP &out)
{
    if (raw.size() < 4)
        return false;

    if (raw[0] != 'P' || raw[1] != '/' || raw[3] != '/')
        return false;

    out.tipo = static_cast<ID_TIPO>(raw[2]);
    out.cuerpo = raw.substr(4);

    return true;
}