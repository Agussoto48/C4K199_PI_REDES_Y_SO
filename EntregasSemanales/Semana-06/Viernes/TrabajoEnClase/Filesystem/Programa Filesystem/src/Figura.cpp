#include "Figura.h"
#include <iostream>
#include <cstring>

using namespace std;

/**
 * Constructor por defecto de la figura.
 *
 * Inicializa la figura con valores vacios y sin piezas asociadas.
 */
Figura::Figura() {
    idFigura = 0;
    mitad = 0;
    cantidadPiezas = 0;
    piezas = 0;
    nombreFigura[0] = '\0';
}

/**
 * Destructor de la figura.
 *
 * Libera la memoria asociada a las piezas almacenadas.
 */
Figura::~Figura() {
    Liberar();
}

/**
 * Inicializa la figura con sus datos basicos y reserva espacio
 * para almacenar sus piezas.
 *
 * Primero libera cualquier contenido previo, luego asigna el id,
 * nombre, mitad y cantidad de piezas. Si hay piezas, reserva memoria
 * dinamica para almacenarlas.
 *
 * @param id Identificador de la figura.
 * @param nombre Nombre de la figura.
 * @param mitadFigura Mitad de la figura.
 * @param cantPiezas Cantidad de piezas distintas de la figura.
 */
void Figura::Inicializar(int id, const char* nombre, int mitadFigura, int cantPiezas) {
    Liberar();

    idFigura = id;
    mitad = mitadFigura;
    cantidadPiezas = cantPiezas;

    // copia nombre de la figura
    strncpy(nombreFigura, nombre, TAM_NOMBRE_FIGURA - 1);
    nombreFigura[TAM_NOMBRE_FIGURA - 1] = '\0';

    // reserva arreglo de piezas
    if ( cantidadPiezas > 0 ) {
        piezas = new PiezaFigura[cantidadPiezas];

        for ( int i = 0; i < cantidadPiezas; i++ ) {
            piezas[i].nombre[0] = '\0';
            piezas[i].cantidad = 0;
        }
    }
}

/**
 * Agrega o actualiza una pieza en una posicion especifica de la figura.
 *
 * Verifica que el arreglo de piezas exista y que el indice sea valido
 * antes de copiar el nombre y la cantidad de la pieza.
 *
 * @param indice Posicion donde se almacenara la pieza.
 * @param nombrePieza Nombre de la pieza.
 * @param cantidadPieza Cantidad de esa pieza.
 */
void Figura::AgregarPieza(int indice, const char* nombrePieza, int cantidadPieza) {
    if ( piezas == 0 ) {
        return;
    }

    if ( indice < 0 || indice >= cantidadPiezas ) {
        return;
    }

    // guarda datos de la pieza en la posicion indicada
    strncpy(piezas[indice].nombre, nombrePieza, TAM_NOMBRE_PIEZA - 1);
    piezas[indice].nombre[TAM_NOMBRE_PIEZA - 1] = '\0';
    piezas[indice].cantidad = cantidadPieza;
}

/**
 * Libera la memoria dinamica asociada a las piezas de la figura.
 *
 * Ademas reinicia los datos basicos de la figura a valores vacios.
 */
void Figura::Liberar() {
    if ( piezas != 0 ) {
        delete[] piezas;
        piezas = 0;
    }

    // reinicia atributos de la figura
    idFigura = 0;
    mitad = 0;
    cantidadPiezas = 0;
    nombreFigura[0] = '\0';
}

/**
 * Muestra en pantalla la informacion de la figura y sus piezas.
 *
 * Imprime el nombre, id, mitad y el detalle de cada pieza almacenada.
 */
void Figura::Imprimir() {
    cout << "Figura: " << nombreFigura << endl;
    cout << "ID: " << idFigura << endl;
    cout << "Mitad: " << mitad << endl;
    cout << "Cantidad de piezas distintas: " << cantidadPiezas << endl;

    // imprime detalle de piezas
    for ( int i = 0; i < cantidadPiezas; i++ ) {
        cout << "  Pieza " << i + 1 << ": "<< piezas[i].nombre << " -> "<< piezas[i].cantidad << endl;
    }
}