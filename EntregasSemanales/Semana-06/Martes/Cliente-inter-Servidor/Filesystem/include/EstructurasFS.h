#ifndef ESTRUCTURAS_FS_H
#define ESTRUCTURAS_FS_H

#include <cstring>

/*
 * Tamano fijo de cada bloque del archivo .dat
 */
#define TAM_BLOQUE 256

/*
 * Valores especiales y estados
 */
#define FIN_CADENA -1
#define BLOQUE_INVALIDO -1

#define ENTRADA_LIBRE 0
#define ENTRADA_ACTIVA 1

/*
 * Tipos logicos de bloque
 */
#define TIPO_HEADER 1
#define TIPO_BITMAP 2
#define TIPO_DIRECTORIO 3
#define TIPO_DATOS 4

/*
 * Tamaños maximos para nombres
 */
#define TAM_NOMBRE_FS 16
#define TAM_VERSION_FS 8
#define TAM_NOMBRE_FIGURA 32
#define TAM_NOMBRE_PIEZA 32

/*
 * Firma del sistema para validar el archivo
 */
#define FS_MAGIC "LEGOFS"

/*
 * Header del sistema de archivos.
 * Debe ocupar exactamente 256 bytes.
 *
 * Guarda la informacion general del sistema:
 * nombre, version, tamano de bloque, cantidad de bloques,
 * cantidad de figuras, y donde empiezan bitmap/directorio/datos.
 */
struct HeaderFS {
    char magic[8];
    char nombreFS[TAM_NOMBRE_FS];
    char version[TAM_VERSION_FS];

    int tamBloque;
    int totalBloques;
    int totalFigurasActivas;

    int bloqueInicioBitmap;
    int cantidadBloquesBitmap;

    int bloqueInicioDirectorio;
    int cantidadBloquesDirectorio;

    int bloqueInicioDatos;

    char relleno[192];
};

/*
 * Cada entrada del directorio representa una figura-mitad.
 * Esto sigue su propuesta:
 * - ID de figura
 * - nombre
 * - mitad
 * - bloque inicial
 * - cantidad de bloques usados
 * - tamano logico
 * - estado
 */
struct EntradaDirectorio {
    int idFigura;
    char nombreFigura[TAM_NOMBRE_FIGURA];
    int mitad;               // 1 o 2
    int bloqueInicial;
    int cantidadBloques;
    int tamanoLogico;        // bytes reales de la figura
    int estado;              // ENTRADA_LIBRE o ENTRADA_ACTIVA

    char relleno[8];
};

/*
 * Encabezado logico que va al inicio de la figura
 * dentro de los bloques de datos.
 */
struct EncabezadoFigura {
    int idFigura;
    char nombreFigura[TAM_NOMBRE_FIGURA];
    int mitad;
    int cantidadPiezas;
};

/*
 * Registro de una pieza de lego.
 */
struct RegistroPieza {
    char nombrePieza[TAM_NOMBRE_PIEZA];
    int cantidad;
};

/*
 * Bloque fisico de datos.
 * Los primeros 252 bytes guardan informacion y
 * los ultimos 4 bytes guardan el puntero al siguiente bloque.
 *
 * Asi cada bloque mide exactamente 256 bytes.
 */
struct BloqueDatos {
    char datos[252];
    int siguienteBloque;
};

/*
 * Bloque generico de 256 bytes.
 * Sirve para leer/escribir cualquier bloque del archivo.
 */
struct BloqueGenerico {
    char bytes[TAM_BLOQUE];
};

/*
 * Bloque de directorio.
 * Se reserva el ultimo entero como puntero a otro bloque
 * de directorio por si en el futuro quieren crecimiento
 * enlazado del directorio.
 *
 * 252 bytes / sizeof(EntradaDirectorio) = cantidad variable,
 * por eso aqui se deja como bytes crudos y luego se interpreta.
 */
struct BloqueDirectorio {
    char datos[252];
    int siguienteBloque;
};

/*
 * Funciones auxiliares opcionales para limpiar structs
 */
inline void LimpiarHeader(HeaderFS& header) {
    memset(&header, 0, sizeof(HeaderFS));
}

inline void LimpiarEntradaDirectorio(EntradaDirectorio& entrada) {
    memset(&entrada, 0, sizeof(EntradaDirectorio));
    entrada.bloqueInicial = BLOQUE_INVALIDO;
}

inline void LimpiarEncabezadoFigura(EncabezadoFigura& encabezado) {
    memset(&encabezado, 0, sizeof(EncabezadoFigura));
}

inline void LimpiarRegistroPieza(RegistroPieza& pieza) {
    memset(&pieza, 0, sizeof(RegistroPieza));
}

inline void LimpiarBloqueDatos(BloqueDatos& bloque) {
    memset(&bloque, 0, sizeof(BloqueDatos));
    bloque.siguienteBloque = FIN_CADENA;
}

inline void LimpiarBloqueGenerico(BloqueGenerico& bloque) {
    memset(&bloque, 0, sizeof(BloqueGenerico));
}



static_assert(sizeof(HeaderFS) == TAM_BLOQUE, "HeaderFS debe medir 256 bytes");
static_assert(sizeof(BloqueDatos) == TAM_BLOQUE, "BloqueDatos debe medir 256 bytes");
static_assert(sizeof(BloqueGenerico) == TAM_BLOQUE, "BloqueGenerico debe medir 256 bytes");
static_assert(sizeof(BloqueDirectorio) == TAM_BLOQUE, "BloqueDirectorio debe medir 256 bytes");



#endif