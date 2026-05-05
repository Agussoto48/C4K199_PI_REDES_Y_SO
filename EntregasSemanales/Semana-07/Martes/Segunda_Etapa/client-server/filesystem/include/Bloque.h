#ifndef BLOQUE_H
#define BLOQUE_H

#include <fstream>
using namespace std;

#include "EstructurasFS.h"

class Bloque {
   public:
      Bloque();
      ~Bloque();

      void LeerBloque(fstream& archivo, int numeroBloque, char* buffer);
      void EscribirBloque(fstream& archivo, int numeroBloque, const char* buffer);

      void LeerBloqueGenerico(fstream& archivo, int numeroBloque, BloqueGenerico& bloque);
      void EscribirBloqueGenerico(fstream& archivo, int numeroBloque, const BloqueGenerico& bloque);

      void LeerBloqueDatos(fstream& archivo, int numeroBloque, BloqueDatos& bloque);
      void EscribirBloqueDatos(fstream& archivo, int numeroBloque, const BloqueDatos& bloque);

      void LeerBloqueDirectorio(fstream& archivo, int numeroBloque, BloqueDirectorio& bloque);
      void EscribirBloqueDirectorio(fstream& archivo, int numeroBloque, const BloqueDirectorio& bloque);

      long CalcularDesplazamiento(int numeroBloque);
};

#endif