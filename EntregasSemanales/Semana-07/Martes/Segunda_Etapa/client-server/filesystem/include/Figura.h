#ifndef FIGURA_H
#define FIGURA_H

#include "EstructurasFS.h"

struct PiezaFigura {
    char nombre[TAM_NOMBRE_PIEZA];
    int cantidad;
};

class Figura {
   public:
      Figura();
      ~Figura();

      int idFigura;
      char nombreFigura[TAM_NOMBRE_FIGURA];
      int mitad;

      int cantidadPiezas;
      PiezaFigura* piezas;

      void Inicializar(int id, const char* nombre, int mitadFigura, int cantPiezas);
      void AgregarPieza(int indice, const char* nombrePieza, int cantidadPieza);
      void Liberar();
      void Imprimir();
};

#endif