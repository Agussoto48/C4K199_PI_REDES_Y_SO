#ifndef BLOQUE_H
#define BLOQUE_H

#include <fstream>
using namespace std;

#include "EstructurasFS.h"

// Maneja lectura y escritura de bloques en el archivo
class Bloque {
   public:
      Bloque();

      ~Bloque();

      // Lee un bloque del archivo
      void LeerBloque(fstream&, int, char*);

      // Escribe un bloque en el archivo
      void EscribirBloque(fstream&, int, const char*);

      // Lee bloque generico
      void LeerBloqueGenerico(fstream&, int, BloqueGenerico&);

      // Escribe bloque generico
      void EscribirBloqueGenerico(fstream&, int, const BloqueGenerico&);

      // Lee bloque de datos
      void LeerBloqueDatos(fstream&, int, BloqueDatos&);

      // Escribe bloque de datos
      void EscribirBloqueDatos(fstream&, int, const BloqueDatos&);

      // Lee bloque de directorio
      void LeerBloqueDirectorio(fstream&, int, BloqueDirectorio&);

      // Escribe bloque de directorio
      void EscribirBloqueDirectorio(fstream&, int, const BloqueDirectorio&);

      // Calcula desplazamiento de un bloque
      long CalcularDesplazamiento(int);
};

#endif