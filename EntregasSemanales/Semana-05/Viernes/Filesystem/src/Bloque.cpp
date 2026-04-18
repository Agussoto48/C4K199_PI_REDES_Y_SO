#include "Bloque.h"
#include <iostream>
#include <cstring>


Bloque::Bloque() {
}

Bloque::~Bloque() {
}

/**
 * Calcula el desplazamiento en bytes de un bloque dentro del archivo.
 *
 * Multiplica el numero de bloque por el tamano fijo de bloque.
 *
 * @param numeroBloque Numero del bloque dentro del archivo.
 * @return Desplazamiento en bytes desde el inicio del archivo.
 */
long Bloque::CalcularDesplazamiento(int numeroBloque) {
    long desplazamiento = 0;
    desplazamiento = (long) numeroBloque * TAM_BLOQUE;
    return desplazamiento;
}

/**
 * Lee un bloque completo del archivo en un buffer.
 *
 * Calcula la posicion del bloque, mueve el cursor de lectura
 * y copia TAM_BLOQUE bytes al buffer recibido.
 *
 * @param archivo Archivo del sistema de bloques.
 * @param numeroBloque Numero de bloque que se desea leer.
 * @param buffer Arreglo donde se almacenaran los bytes leidos.
 */
void Bloque::LeerBloque(fstream& archivo, int numeroBloque, char* buffer) {
    long desplazamiento = 0;

    // valida numero de bloque
    if ( numeroBloque < 0 ) {
        return;
    }

    desplazamiento = CalcularDesplazamiento(numeroBloque);

    // posiciona y leer bloque completo
    archivo.seekg(desplazamiento, ios::beg);
    archivo.read(buffer, TAM_BLOQUE);
}

/**
 * Escribe un bloque completo en el archivo desde un buffer.
 *
 * Calcula la posicion del bloque, mueve el cursor de escritura
 * y guarda TAM_BLOQUE bytes en el archivo.
 *
 * @param archivo Archivo del sistema de bloques.
 * @param numeroBloque Numero de bloque que se desea escribir.
 * @param buffer Arreglo con los bytes a guardar.
 */
void Bloque::EscribirBloque(fstream& archivo, int numeroBloque, const char* buffer) {
    long desplazamiento = 0;

    // valida numero de bloque
    if ( numeroBloque < 0 ) {
        return;
    }

    desplazamiento = CalcularDesplazamiento(numeroBloque);

    // posiciona y escribe bloque completo
    archivo.seekp(desplazamiento, ios::beg);
    archivo.write(buffer, TAM_BLOQUE);
    archivo.flush();
}

/**
 * Lee un bloque generico del archivo.
 *
 * Usa la rutina general de lectura para llenar la estructura BloqueGenerico.
 *
 * @param archivo Archivo del sistema de bloques.
 * @param numeroBloque Numero de bloque a leer.
 * @param bloque Estructura donde se almacenara el bloque leido.
 */
void Bloque::LeerBloqueGenerico(fstream& archivo, int numeroBloque, BloqueGenerico& bloque) {
    LeerBloque(archivo, numeroBloque, bloque.bytes);
}

/**
 * Escribe un bloque generico en el archivo.
 *
 * Usa la rutina general de escritura para guardar la estructura BloqueGenerico.
 *
 * @param archivo Archivo del sistema de bloques.
 * @param numeroBloque Numero de bloque a escribir.
 * @param bloque Estructura con el contenido del bloque.
 */
void Bloque::EscribirBloqueGenerico(fstream& archivo, int numeroBloque, const BloqueGenerico& bloque) {
    EscribirBloque(archivo, numeroBloque, bloque.bytes);
}

/**
 * Lee un bloque de datos del archivo.
 *
 * Usa la rutina general de lectura y convierte el bloque al tipo BloqueDatos.
 *
 * @param archivo Archivo del sistema de bloques.
 * @param numeroBloque Numero de bloque a leer.
 * @param bloque Estructura donde se almacenara el bloque de datos.
 */
void Bloque::LeerBloqueDatos(fstream& archivo, int numeroBloque, BloqueDatos& bloque) {
    LeerBloque(archivo, numeroBloque, (char*) &bloque);
}

/**
 * Escribe un bloque de datos en el archivo.
 *
 * Usa la rutina general de escritura para guardar la estructura BloqueDatos.
 *
 * @param archivo Archivo del sistema de bloques.
 * @param numeroBloque Numero de bloque a escribir.
 * @param bloque Estructura con el bloque de datos.
 */
void Bloque::EscribirBloqueDatos(fstream& archivo, int numeroBloque, const BloqueDatos& bloque) {
    EscribirBloque(archivo, numeroBloque, (const char*) &bloque);
}

/**
 * Lee un bloque de directorio del archivo.
 *
 * Usa la rutina general de lectura y convierte el bloque al tipo BloqueDirectorio.
 *
 * @param archivo Archivo del sistema de bloques.
 * @param numeroBloque Numero de bloque a leer.
 * @param bloque Estructura donde se almacenara el bloque de directorio.
 */
void Bloque::LeerBloqueDirectorio(fstream& archivo, int numeroBloque, BloqueDirectorio& bloque) {
    LeerBloque(archivo, numeroBloque, (char*) &bloque);
}

/**
 * Escribe un bloque de directorio en el archivo.
 *
 * Usa la rutina general de escritura para guardar la estructura BloqueDirectorio.
 *
 * @param archivo Archivo del sistema de bloques.
 * @param numeroBloque Numero de bloque a escribir.
 * @param bloque Estructura con el bloque de directorio.
 */
void Bloque::EscribirBloqueDirectorio(fstream& archivo, int numeroBloque, const BloqueDirectorio& bloque) {
    EscribirBloque(archivo, numeroBloque, (const char*) &bloque);
}