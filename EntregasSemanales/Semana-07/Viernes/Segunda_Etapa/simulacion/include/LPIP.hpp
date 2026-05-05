#pragma once
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include "estructuras.hpp"

class LPIP
{
public:
    ID_TIPO tipo;
    std::string cuerpo;

    std::string unificar() const;
    static bool parse(const std::string &raw, LPIP &out);
};