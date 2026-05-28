#pragma once
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include "estructuras.hpp"

class Uppercase
{
public:
    ID_TIPO tipo;
    std::string cuerpo;

    std::string unificar() const;
    static bool parse(const std::string &raw, Uppercase &out);
};