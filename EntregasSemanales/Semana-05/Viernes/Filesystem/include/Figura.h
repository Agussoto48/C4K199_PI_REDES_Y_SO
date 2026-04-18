#ifndef FIGURA_H
#define FIGURA_H

#include "EstructurasFS.h"

//Representa una pieza de una figura.

struct PiezaFigura {
    char nombre[TAM_NOMBRE_PIEZA];
    int cantidad;
};

//Representa una figura con sus piezas.

class Figura {
   public:

      Figura();

      ~Figura();

      int idFigura;
      char nombreFigura[TAM_NOMBRE_FIGURA];
      int mitad;

      int cantidadPiezas;
      PiezaFigura* piezas;

      
    //Inicializa la figura.
      void Inicializar(int, const char*, int, int);

    //Agrega una pieza
      void AgregarPieza(int, const char*, int);

    //Libera memoria.
      void Liberar();

    //Imprime la figura.
      void Imprimir();
};

#endif