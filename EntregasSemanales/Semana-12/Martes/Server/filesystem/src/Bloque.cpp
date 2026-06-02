#include "Bloque.h"
#include <iostream>
#include <cstring>

Bloque::Bloque() {
}

Bloque::~Bloque() {
}

long Bloque::CalcularDesplazamiento(int numeroBloque) {
    long desplazamiento = 0;
    desplazamiento = (long) numeroBloque * TAM_BLOQUE;
    return desplazamiento;
}

void Bloque::LeerBloque(fstream& archivo, int numeroBloque, char* buffer) {
    long desplazamiento = 0;

    if ( numeroBloque < 0 ) {
        return;
    }

    desplazamiento = CalcularDesplazamiento(numeroBloque);

    archivo.seekg(desplazamiento, ios::beg);
    archivo.read(buffer, TAM_BLOQUE);
}

void Bloque::EscribirBloque(fstream& archivo, int numeroBloque, const char* buffer) {
    long desplazamiento = 0;

    if ( numeroBloque < 0 ) {
        return;
    }

    desplazamiento = CalcularDesplazamiento(numeroBloque);

    archivo.seekp(desplazamiento, ios::beg);
    archivo.write(buffer, TAM_BLOQUE);
    archivo.flush();
}

void Bloque::LeerBloqueGenerico(fstream& archivo, int numeroBloque, BloqueGenerico& bloque) {
    LeerBloque(archivo, numeroBloque, bloque.bytes);
}

void Bloque::EscribirBloqueGenerico(fstream& archivo, int numeroBloque, const BloqueGenerico& bloque) {
    EscribirBloque(archivo, numeroBloque, bloque.bytes);
}

void Bloque::LeerBloqueDatos(fstream& archivo, int numeroBloque, BloqueDatos& bloque) {
    LeerBloque(archivo, numeroBloque, (char*) &bloque);
}

void Bloque::EscribirBloqueDatos(fstream& archivo, int numeroBloque, const BloqueDatos& bloque) {
    EscribirBloque(archivo, numeroBloque, (const char*) &bloque);
}

void Bloque::LeerBloqueDirectorio(fstream& archivo, int numeroBloque, BloqueDirectorio& bloque) {
    LeerBloque(archivo, numeroBloque, (char*) &bloque);
}

void Bloque::EscribirBloqueDirectorio(fstream& archivo, int numeroBloque, const BloqueDirectorio& bloque) {
    EscribirBloque(archivo, numeroBloque, (const char*) &bloque);
}