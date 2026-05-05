#include "Figura.h"
#include <iostream>
#include <cstring>

using namespace std;

Figura::Figura() {
    idFigura = 0;
    mitad = 0;
    cantidadPiezas = 0;
    piezas = 0;
    nombreFigura[0] = '\0';
}

Figura::~Figura() {
    Liberar();
}

void Figura::Inicializar(int id, const char* nombre, int mitadFigura, int cantPiezas) {
    Liberar();

    idFigura = id;
    mitad = mitadFigura;
    cantidadPiezas = cantPiezas;

    strncpy(nombreFigura, nombre, TAM_NOMBRE_FIGURA - 1);
    nombreFigura[TAM_NOMBRE_FIGURA - 1] = '\0';

    if ( cantidadPiezas > 0 ) {
        piezas = new PiezaFigura[cantidadPiezas];

        for ( int i = 0; i < cantidadPiezas; i++ ) {
            piezas[i].nombre[0] = '\0';
            piezas[i].cantidad = 0;
        }
    }
}

void Figura::AgregarPieza(int indice, const char* nombrePieza, int cantidadPieza) {
    if ( piezas == 0 ) {
        return;
    }

    if ( indice < 0 || indice >= cantidadPiezas ) {
        return;
    }

    strncpy(piezas[indice].nombre, nombrePieza, TAM_NOMBRE_PIEZA - 1);
    piezas[indice].nombre[TAM_NOMBRE_PIEZA - 1] = '\0';
    piezas[indice].cantidad = cantidadPieza;
}

void Figura::Liberar() {
    if ( piezas != 0 ) {
        delete[] piezas;
        piezas = 0;
    }

    idFigura = 0;
    mitad = 0;
    cantidadPiezas = 0;
    nombreFigura[0] = '\0';
}

void Figura::Imprimir() {
    cout << "Figura: " << nombreFigura << endl;
    cout << "ID: " << idFigura << endl;
    cout << "Mitad: " << mitad << endl;
    cout << "Cantidad de piezas distintas: " << cantidadPiezas << endl;

    for ( int i = 0; i < cantidadPiezas; i++ ) {
        cout << "  Pieza " << i + 1 << ": "
             << piezas[i].nombre
             << " -> "
             << piezas[i].cantidad
             << endl;
    }
}