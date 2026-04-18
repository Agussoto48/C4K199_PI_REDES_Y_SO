#include "SistemaArchivos.h"
#include <iostream>
#include <cstring>
#include <sstream>

using namespace std;

SistemaArchivos::SistemaArchivos() {
    nombreArchivo[0] = '\0';
}

SistemaArchivos::~SistemaArchivos() {
    CerrarSistema();
}

/**
 * Abre el archivo del sistema de archivos en modo lectura y escritura binaria.
 *
 * Si el archivo se abre correctamente, guarda su nombre internamente
 * y retorna 0. En caso contrario, retorna -1.
 *
 * @param ruta Ruta del archivo a abrir.
 * @return 0 si el archivo se abrio correctamente, -1 en caso de error.
 */
int SistemaArchivos::AbrirArchivo(const char* ruta) {
    int resultado = -1;

    archivo.open(ruta, ios::in | ios::out | ios::binary);

    if ( archivo.is_open() ) {
        strncpy(nombreArchivo, ruta, 127);
        nombreArchivo[127] = '\0';
        resultado = 0;
    }

    return resultado;
}

/**
 * Cierra el archivo del sistema de archivos si se encuentra abierto.
 *
 * Ademas, limpia el nombre del archivo almacenado internamente,
 * indicando que no hay ningún archivo activo.
 */
void SistemaArchivos::CerrarArchivo() {
    if ( archivo.is_open() ) {
        archivo.close();
    }

    nombreArchivo[0] = '\0';
}

/**
 * Lee el bloque de encabezado del sistema de archivos desde el archivo .dat.
 *
 * Limpia las estructuras auxiliares, obtiene el bloque 0 (header)
 * y copia su contenido al objeto HeaderFS proporcionado.
 *
 * @param header Referencia donde se almacenara la informacion del header.
 */
void SistemaArchivos::LeerHeader(HeaderFS& header) {
    BloqueGenerico bloque;

    LimpiarBloqueGenerico(bloque);
    LimpiarHeader(header);

    manejadorBloques.LeerBloqueGenerico(archivo, 0, bloque);
    memcpy(&header, bloque.bytes, sizeof(HeaderFS));
}

/**
 * Escribe el bloque de encabezado del sistema de archivos en el archivo .dat.
 *
 * Prepara un bloque generico, copia la informacion del header en el
 * y lo guarda en el bloque 0 del archivo.
 *
 * @param header Estructura que contiene la informacion del header a escribir.
 */
void SistemaArchivos::EscribirHeader(const HeaderFS& header) {
    BloqueGenerico bloque;

    LimpiarBloqueGenerico(bloque);
    memcpy(bloque.bytes, &header, sizeof(HeaderFS));
    manejadorBloques.EscribirBloqueGenerico(archivo, 0, bloque);
}

/**
 * Lee el bitmap del sistema de archivos desde el archivo .dat.
 *
 * Recorre los bloques donde se encuentra el bitmap segun el header
 * y copia su contenido en memoria continua en el arreglo bitmap.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @param bitmap Arreglo donde se almacenara el bitmap leido.
 */
void SistemaArchivos::LeerBitmap(const HeaderFS& header, char* bitmap) {
    BloqueGenerico bloque;
    int i = 0;

    for ( i = 0; i < header.cantidadBloquesBitmap; i++ ) {
        LimpiarBloqueGenerico(bloque);
        manejadorBloques.LeerBloqueGenerico(archivo, header.bloqueInicioBitmap + i, bloque);
        memcpy(bitmap + (i * TAM_BLOQUE), bloque.bytes, TAM_BLOQUE);
    }
}

/**
 * Escribe el bitmap del sistema de archivos en el archivo .dat.
 *
 * Recorre los bloques del bitmap segun el header, copia la informacion
 * desde memoria y la guarda en los bloques correspondientes del archivo.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @param bitmap Arreglo que contiene el bitmap a escribir.
 */
void SistemaArchivos::EscribirBitmap(const HeaderFS& header, const char* bitmap) {
    BloqueGenerico bloque;
    int i = 0;

    for ( i = 0; i < header.cantidadBloquesBitmap; i++ ) {
        LimpiarBloqueGenerico(bloque);
        memcpy(bloque.bytes, bitmap + (i * TAM_BLOQUE), TAM_BLOQUE);
        manejadorBloques.EscribirBloqueGenerico(archivo, header.bloqueInicioBitmap + i, bloque);
    }
}

/**
 * Obtiene el valor de un bit especifico dentro del bitmap.
 *
 * Calcula el byte y el bit correspondiente a la posicion dada
 * y retorna su valor (0 o 1).
 *
 * @param bitmap Arreglo que contiene el bitmap.
 * @param posicion Posicion del bit a consultar.
 * @return Valor del bit (0 o 1).
 */
int SistemaArchivos::ObtenerBit(const char* bitmap, int posicion) {
    int byteIndex = 0;
    int bitIndex = 0;
    unsigned char valor = 0;
    int resultado = 0;

    byteIndex = posicion / 8;
    bitIndex = posicion % 8;

    valor = (unsigned char) bitmap[byteIndex];
    resultado = (valor >> bitIndex) & 1;

    return resultado;
}
/**
 * Modifica el valor de un bit especifico dentro del bitmap.
 *
 * Calcula el byte y el bit correspondiente a la posicion dada
 * y actualiza su valor a 0 o 1 segun se indique.
 *
 * @param bitmap Arreglo que contiene el bitmap.
 * @param posicion Posicion del bit a modificar.
 * @param valor Nuevo valor del bit (0 o 1).
 */
void SistemaArchivos::PonerBit(char* bitmap, int posicion, int valor) {
    int byteIndex = 0;
    int bitIndex = 0;
    unsigned char mascara = 0;
    unsigned char actual = 0;

    byteIndex = posicion / 8;
    bitIndex = posicion % 8;
    mascara = (unsigned char) (1 << bitIndex);
    actual = (unsigned char) bitmap[byteIndex];

    if ( valor == 0 ) {
        actual = (unsigned char) (actual & (~mascara));
    } else {
        actual = (unsigned char) (actual | mascara);
    }

    bitmap[byteIndex] = (char) actual;
}

/**
 * Busca el primer bloque libre en el sistema de archivos.
 *
 * Carga el bitmap en memoria y recorre los bloques de datos
 * hasta encontrar uno no ocupado. Retorna su numero o un
 * valor invalido si no hay disponibles.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @return Numero del bloque libre o BLOQUE_INVALIDO si no hay espacio.
 */
int SistemaArchivos::BuscarBloqueLibre(HeaderFS& header) {
    int resultado = BLOQUE_INVALIDO;
    int bytesBitmap = 0;
    char* bitmap = 0;
    int i = 0;

    bytesBitmap = header.cantidadBloquesBitmap * TAM_BLOQUE;
    bitmap = new char[bytesBitmap];
    memset(bitmap, 0, bytesBitmap);

    LeerBitmap(header, bitmap);

    for ( i = header.bloqueInicioDatos; i < header.totalBloques && resultado == BLOQUE_INVALIDO; i++ ) {
        if ( ObtenerBit(bitmap, i) == 0 ) {
            resultado = i;
        }
    }

    delete[] bitmap;

    return resultado;
}

/**
 * Marca un bloque como ocupado o libre en el bitmap.
 *
 * Carga el bitmap, modifica el bit correspondiente al bloque
 * indicado y luego guarda los cambios en el archivo.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @param numeroBloque Numero del bloque a modificar.
 * @param ocupado Valor del estado (0 libre, 1 ocupado).
 */
void SistemaArchivos::MarcarBloque(HeaderFS& header, int numeroBloque, int ocupado) {
    int bytesBitmap = 0;
    char* bitmap = 0;

    bytesBitmap = header.cantidadBloquesBitmap * TAM_BLOQUE;
    bitmap = new char[bytesBitmap];
    memset(bitmap, 0, bytesBitmap);

    LeerBitmap(header, bitmap);
    PonerBit(bitmap, numeroBloque, ocupado);
    EscribirBitmap(header, bitmap);

    delete[] bitmap;
}


/**
 * Calcula la cantidad de entradas de directorio que caben en un bloque.
 *
 * Divide el espacio disponible del bloque entre el tamaño de una entrada
 * de directorio para determinar cuantas se pueden almacenar.
 *
 * @return Cantidad de entradas por bloque de directorio.
 */
int SistemaArchivos::CantidadEntradasPorBloqueDirectorio() {
    int cantidad = 0;
    cantidad = 252 / (int) sizeof(EntradaDirectorio);
    return cantidad;
}

/**
 * Calcula la capacidad total del directorio en el sistema de archivos.
 *
 * Multiplica la cantidad de entradas por bloque por la cantidad de bloques
 * asignados al directorio.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @return Capacidad total de entradas en el directorio.
 */
int SistemaArchivos::CapacidadTotalDirectorio(const HeaderFS& header) {
    int capacidad = 0;
    capacidad = CantidadEntradasPorBloqueDirectorio() * header.cantidadBloquesDirectorio;
    return capacidad;
}

/**
 * Lee una entrada especifica del directorio desde el archivo .dat.
 *
 * Calcula en que bloque del directorio se encuentra la entrada segun
 * su indice, lee el bloque correspondiente y copia la entrada en la
 * estructura proporcionada.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @param indice Posicion de la entrada dentro del directorio.
 * @param entrada Referencia donde se almacenara la entrada leida.
 */
void SistemaArchivos::LeerEntradaDirectorio(const HeaderFS& header, int indice, EntradaDirectorio& entrada) {
    int entradasPorBloque = 0;
    int bloqueRelativo = 0;
    int desplazamientoEntrada = 0;
    int bloqueFisico = 0;
    BloqueDirectorio bloque;

    LimpiarEntradaDirectorio(entrada);

    entradasPorBloque = CantidadEntradasPorBloqueDirectorio();
    bloqueRelativo = indice / entradasPorBloque;
    desplazamientoEntrada = indice % entradasPorBloque;
    bloqueFisico = header.bloqueInicioDirectorio + bloqueRelativo;

    manejadorBloques.LeerBloqueDirectorio(archivo, bloqueFisico, bloque);

    memcpy(&entrada,bloque.datos + (desplazamientoEntrada * sizeof(EntradaDirectorio)),sizeof(EntradaDirectorio));
}

/**
 * Escribe una entrada especifica del directorio en el archivo .dat.
 *
 * Calcula en que bloque del directorio se encuentra la entrada segun
 * su indice, lee ese bloque, actualiza la posicion correspondiente
 * con la nueva entrada y vuelve a escribir el bloque en el archivo.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @param indice Posicion de la entrada dentro del directorio.
 * @param entrada Estructura con la entrada de directorio a escribir.
 */
void SistemaArchivos::EscribirEntradaDirectorio(const HeaderFS& header, int indice, const EntradaDirectorio& entrada) {
    int entradasPorBloque = 0;
    int bloqueRelativo = 0;
    int desplazamientoEntrada = 0;
    int bloqueFisico = 0;
    BloqueDirectorio bloque;

    entradasPorBloque = CantidadEntradasPorBloqueDirectorio();
    bloqueRelativo = indice / entradasPorBloque;
    desplazamientoEntrada = indice % entradasPorBloque;
    bloqueFisico = header.bloqueInicioDirectorio + bloqueRelativo;

    manejadorBloques.LeerBloqueDirectorio(archivo, bloqueFisico, bloque);

    memcpy(bloque.datos + (desplazamientoEntrada * sizeof(EntradaDirectorio)),&entrada,sizeof(EntradaDirectorio));

    manejadorBloques.EscribirBloqueDirectorio(archivo, bloqueFisico, bloque);
}

/**
 * Busca una entrada libre dentro del directorio.
 *
 * Recorre todas las entradas del directorio hasta encontrar una
 * que este marcada como libre y retorna su indice. Si no existe,
 * retorna -1.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @return Indice de la entrada libre o -1 si no hay espacio disponible.
 */
int SistemaArchivos::BuscarEntradaDirectorioLibre(const HeaderFS& header) {
    int resultado = -1;
    int capacidad = 0;
    int i = 0;
    EntradaDirectorio entrada;

    capacidad = CapacidadTotalDirectorio(header);

    for ( i = 0; i < capacidad && resultado == -1; i++ ) {
        LeerEntradaDirectorio(header, i, entrada);
        if ( entrada.estado == ENTRADA_LIBRE ) {
            resultado = i;
        }
    }

    return resultado;
}

/**
 * Compara dos nombres de figura para verificar si son iguales.
 *
 * Utiliza strcmp para comparar ambas cadenas y retorna 1 si son
 * identicas o 0 en caso contrario.
 *
 * @param nombre1 Primer nombre a comparar.
 * @param nombre2 Segundo nombre a comparar.
 * @return 1 si los nombres son iguales, 0 si son diferentes.
 */
int SistemaArchivos::CompararNombreFigura(const char* nombre1, const char* nombre2) {
    int resultado = 0;

    if ( strcmp(nombre1, nombre2) == 0 ) {
        resultado = 1;
    }

    return resultado;
}

/**
 * Busca una entrada especifica en el directorio segun nombre y mitad.
 *
 * Recorre todas las entradas activas del directorio hasta encontrar
 * una que coincida con el nombre de la figura y la mitad indicada.
 * Si la encuentra, copia la entrada y su indice.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @param nombre Nombre de la figura a buscar.
 * @param mitad Mitad de la figura (1 o 2).
 * @param entradaEncontrada Referencia donde se almacenara la entrada encontrada.
 * @param indiceEncontrado Referencia donde se almacenara el indice encontrado.
 * @return 1 si se encontro la entrada, 0 en caso contrario.
 */
int SistemaArchivos::BuscarEntradaDirectorio(const HeaderFS& header,const char* nombre,int mitad,EntradaDirectorio& entradaEncontrada,int& indiceEncontrado) {
    int encontrado = 0;
    int capacidad = 0;
    int i = 0;
    EntradaDirectorio actual;

    LimpiarEntradaDirectorio(entradaEncontrada);
    indiceEncontrado = -1;
    capacidad = CapacidadTotalDirectorio(header);

    for ( i = 0; i < capacidad && encontrado == 0; i++ ) {
        LeerEntradaDirectorio(header, i, actual);

        if ( actual.estado == ENTRADA_ACTIVA ) {
            if ( CompararNombreFigura(actual.nombreFigura, nombre) == 1 && actual.mitad == mitad ) {
                entradaEncontrada = actual;
                indiceEncontrado = i;
                encontrado = 1;
            }
        }
    }

    return encontrado;
}

/**
 * Calcula el tamano logico de una figura en bytes.
 *
 * Suma el tamano del encabezado de la figura y el espacio requerido
 * para almacenar todos los registros de piezas.
 *
 * @param figura Estructura que contiene la informacion de la figura.
 * @return Tamano total en bytes de la figura.
 */
int SistemaArchivos::CalcularTamanoLogicoFigura(Figura& figura) {
    int tamano = 0;

    tamano = sizeof(EncabezadoFigura) + (figura.cantidadPiezas * (int) sizeof(RegistroPieza));

    return tamano;
}

/**
 * Calcula la cantidad de bloques necesarios para almacenar una figura.
 *
 * Divide el tamano logico de la figura entre la capacidad de datos por bloque
 * y redondea hacia arriba si hay residuo.
 *
 * @param figura Estructura que contiene la informacion de la figura.
 * @return Cantidad de bloques requeridos.
 */
int SistemaArchivos::CalcularBloquesNecesarios(Figura& figura) {
    int tamanoLogico = 0;
    int capacidadDatosPorBloque = 252;
    int bloques = 0;

    tamanoLogico = CalcularTamanoLogicoFigura(figura);
    bloques = tamanoLogico / capacidadDatosPorBloque;

    if ( (tamanoLogico % capacidadDatosPorBloque) != 0 ) {
        bloques++;
    }

    return bloques;
}

/**
 * Convierte una figura a formato binario y la guarda en un buffer.
 *
 * Serializa la informacion de la figura escribiendo primero el encabezado
 * y luego cada una de sus piezas en memoria continua dentro del buffer.
 *
 * @param figura Estructura que contiene la informacion de la figura.
 * @param buffer Arreglo donde se almacenara la representacion binaria.
 * @param tamanoBuffer Tamano total del buffer.
 */
void SistemaArchivos::SerializarFigura(Figura& figura, char* buffer, int tamanoBuffer) {
    EncabezadoFigura encabezado;
    RegistroPieza reg;
    int offset = 0;
    int i = 0;

    memset(buffer, 0, tamanoBuffer);
    LimpiarEncabezadoFigura(encabezado);

    encabezado.idFigura = figura.idFigura;
    strncpy(encabezado.nombreFigura, figura.nombreFigura, TAM_NOMBRE_FIGURA - 1);
    encabezado.nombreFigura[TAM_NOMBRE_FIGURA - 1] = '\0';
    encabezado.mitad = figura.mitad;
    encabezado.cantidadPiezas = figura.cantidadPiezas;

    memcpy(buffer + offset, &encabezado, sizeof(EncabezadoFigura));
    offset += sizeof(EncabezadoFigura);

    for ( i = 0; i < figura.cantidadPiezas; i++ ) {
        LimpiarRegistroPieza(reg);
        strncpy(reg.nombrePieza, figura.piezas[i].nombre, TAM_NOMBRE_PIEZA - 1);
        reg.nombrePieza[TAM_NOMBRE_PIEZA - 1] = '\0';
        reg.cantidad = figura.piezas[i].cantidad;

        memcpy(buffer + offset, &reg, sizeof(RegistroPieza));
        offset += sizeof(RegistroPieza);
    }
}

/**
 * Reconstruye una figura a partir de su representacion binaria en un buffer.
 *
 * Lee primero el encabezado de la figura y luego sus registros de piezas
 * para volver a llenar la estructura en memoria. Si el buffer no contiene
 * suficiente informacion para el encabezado, no realiza cambios.
 *
 * @param buffer Arreglo que contiene la representacion binaria de la figura.
 * @param tamanoBuffer Tamano total del buffer.
 * @param figura Estructura donde se almacenara la figura reconstruida.
 */
void SistemaArchivos::DeserializarFigura(char* buffer, int tamanoBuffer, Figura& figura) {
    EncabezadoFigura encabezado;
    RegistroPieza reg;
    int offset = 0;
    int i = 0;

    figura.Liberar();
    LimpiarEncabezadoFigura(encabezado);

    if ( tamanoBuffer < (int) sizeof(EncabezadoFigura) ) {
        return;
    }

    memcpy(&encabezado, buffer + offset, sizeof(EncabezadoFigura));
    offset += sizeof(EncabezadoFigura);

    figura.Inicializar(encabezado.idFigura,encabezado.nombreFigura,encabezado.mitad,encabezado.cantidadPiezas);

    for ( i = 0; i < encabezado.cantidadPiezas; i++ ) {
        if ( offset + (int) sizeof(RegistroPieza) <= tamanoBuffer ) {
            LimpiarRegistroPieza(reg);
            memcpy(&reg, buffer + offset, sizeof(RegistroPieza));
            figura.AgregarPieza(i, reg.nombrePieza, reg.cantidad);
            offset += sizeof(RegistroPieza);
        }
    }
}

/**
 * Reserva la cantidad indicada de bloques para almacenar una figura.
 *
 * Busca bloques libres uno por uno, los registra en el arreglo recibido
 * y los marca como ocupados en el bitmap. Si en algun momento no hay
 * suficientes bloques disponibles, libera los que ya habia reservado
 * y retorna error.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @param cantidadBloques Cantidad de bloques que se necesitan reservar.
 * @param bloquesReservados Arreglo donde se guardaran los numeros de bloques reservados.
 * @return 0 si la reserva fue exitosa, -1 si no fue posible completarla.
 */
int SistemaArchivos::ReservarBloquesParaFigura(HeaderFS& header, int cantidadBloques, int* bloquesReservados) {
    int resultado = 0;
    int i = 0;
    int bloqueLibre = BLOQUE_INVALIDO;

    for ( i = 0; i < cantidadBloques && resultado == 0; i++ ) {
        bloqueLibre = BuscarBloqueLibre(header);

        if ( bloqueLibre == BLOQUE_INVALIDO ) {
            resultado = -1;
        } else {
            bloquesReservados[i] = bloqueLibre;
            MarcarBloque(header, bloqueLibre, 1);
        }
    }

    if ( resultado != 0 ) {
        for ( i = 0; i < cantidadBloques; i++ ) {
            if ( bloquesReservados[i] != BLOQUE_INVALIDO ) {
                MarcarBloque(header, bloquesReservados[i], 0);
            }
        }
    }

    return resultado;
}

/**
 * Libera todos los bloques de una cadena enlazada de datos.
 *
 * Recorre la cadena de bloques a partir del bloque inicial, obtiene
 * el siguiente bloque de cada uno y marca el bloque actual como libre
 * en el bitmap hasta llegar al final de la cadena.
 *
 * @param header Estructura con la informacion del sistema de archivos.
 * @param bloqueInicial Primer bloque de la cadena que se desea liberar.
 */
void SistemaArchivos::LiberarCadenaBloques(HeaderFS& header, int bloqueInicial) {
    int actual = 0;
    int siguiente = 0;
    BloqueDatos bloque;

    actual = bloqueInicial;

    while ( actual != FIN_CADENA && actual != BLOQUE_INVALIDO ) {
        LimpiarBloqueDatos(bloque);
        manejadorBloques.LeerBloqueDatos(archivo, actual, bloque);
        siguiente = bloque.siguienteBloque;
        MarcarBloque(header, actual, 0);
        actual = siguiente;
    }
}

/**
 * Crea e inicializa un nuevo sistema de archivos en un archivo .dat.
 *
 * Genera el archivo con la cantidad total de bloques indicada, escribe
 * el header del sistema, inicializa el bitmap marcando como ocupados
 * los bloques de metadatos y directorio, y prepara los bloques del
 * directorio enlazados entre si.
 *
 * @param ruta Ruta del archivo .dat que se va a crear.
 * @param totalBloques Cantidad total de bloques del sistema de archivos.
 * @param bloquesDirectorio Cantidad de bloques reservados para el directorio.
 * @return 0 si el sistema fue creado correctamente, -1 en caso de error.
 */
int SistemaArchivos::CrearSistema(const char* ruta, int totalBloques, int bloquesDirectorio) {
    int resultado = -1;
    ofstream creador;
    BloqueGenerico bloqueVacio;
    HeaderFS header;
    int bytesBitmapNecesarios = 0;
    int bloquesBitmap = 0;
    int i = 0;
    int bytesBitmap = 0;
    char* bitmap = 0;
    BloqueDirectorio bloqueDir;

    // crea el archivo .dat con bloques vacios
    creador.open(ruta, ios::out | ios::binary | ios::trunc);

    if ( creador.is_open() ) {
        LimpiarBloqueGenerico(bloqueVacio);

        for ( i = 0; i < totalBloques; i++ ) {
            creador.write(bloqueVacio.bytes, TAM_BLOQUE);
        }

        creador.close();

        // abre archivo para trabajar
        if ( AbrirArchivo(ruta) == 0 ) {
            LimpiarHeader(header);

            // inicializa metadata del FS
            strncpy(header.magic, FS_MAGIC, sizeof(header.magic) - 1);
            header.magic[sizeof(header.magic) - 1] = '\0';

            strncpy(header.nombreFS, "LegoFS", TAM_NOMBRE_FS - 1);
            header.nombreFS[TAM_NOMBRE_FS - 1] = '\0';

            strncpy(header.version, "1.0", TAM_VERSION_FS - 1);
            header.version[TAM_VERSION_FS - 1] = '\0';

            header.tamBloque = TAM_BLOQUE;
            header.totalBloques = totalBloques;
            header.totalFigurasActivas = 0;

            // calculaa tamaño del bitmap
            bytesBitmapNecesarios = totalBloques / 8;
            if ( (totalBloques % 8) != 0 ) {
                bytesBitmapNecesarios++;
            }

            bloquesBitmap = bytesBitmapNecesarios / TAM_BLOQUE;
            if ( (bytesBitmapNecesarios % TAM_BLOQUE) != 0 ) {
                bloquesBitmap++;
            }

            // define layout del sistema
            header.bloqueInicioBitmap = 1;
            header.cantidadBloquesBitmap = bloquesBitmap;
            header.bloqueInicioDirectorio = header.bloqueInicioBitmap + bloquesBitmap;
            header.cantidadBloquesDirectorio = bloquesDirectorio;
            header.bloqueInicioDatos = header.bloqueInicioDirectorio + bloquesDirectorio;

            // guarda header
            EscribirHeader(header);

            // inicializa bitmap en memoria
            bytesBitmap = header.cantidadBloquesBitmap * TAM_BLOQUE;
            bitmap = new char[bytesBitmap];
            memset(bitmap, 0, bytesBitmap);

            // marca bloques ocupados (header, bitmap, directorio)
            PonerBit(bitmap, 0, 1);

            for ( i = 0; i < header.cantidadBloquesBitmap; i++ ) {
                PonerBit(bitmap, header.bloqueInicioBitmap + i, 1);
            }

            for ( i = 0; i < header.cantidadBloquesDirectorio; i++ ) {
                PonerBit(bitmap, header.bloqueInicioDirectorio + i, 1);
            }

            // guarda bitmap
            EscribirBitmap(header, bitmap);
            delete[] bitmap;

            // inicializa bloques del directorio enlazados
            for ( i = 0; i < header.cantidadBloquesDirectorio; i++ ) {
                memset(&bloqueDir, 0, sizeof(BloqueDirectorio));

                if ( i < header.cantidadBloquesDirectorio - 1 ) {
                    bloqueDir.siguienteBloque = header.bloqueInicioDirectorio + i + 1;
                } else {
                    bloqueDir.siguienteBloque = FIN_CADENA;
                }

                manejadorBloques.EscribirBloqueDirectorio(archivo,header.bloqueInicioDirectorio + i,bloqueDir);
            }

            CerrarArchivo();
            resultado = 0;
        }
    }

    return resultado;
}
/**
 * Abre un sistema de archivos existente y valida su encabezado.
 *
 * Intenta abrir el archivo .dat, lee el header y verifica que el
 * identificador magic corresponda al sistema esperado. Si no es
 * valido, cierra el archivo y retorna error.
 *
 * @param ruta Ruta del archivo .dat a abrir.
 * @return 0 si el sistema se abrio correctamente, -1 en caso de error.
 */
int SistemaArchivos::AbrirSistema(const char* ruta) {
    int resultado = -1;
    HeaderFS header;

    if ( AbrirArchivo(ruta) == 0 ) {
        // lee header del sistema
        LeerHeader(header);

        // valida firma del sistema de archivos
        if ( strcmp(header.magic, FS_MAGIC) == 0 ) {
            resultado = 0;
        } else {
            CerrarArchivo();
        }
    }

    return resultado;
}

/**
 * Cierra el sistema de archivos activo.
 *
 * Libera el archivo asociado al sistema de archivos.
 */
void SistemaArchivos::CerrarSistema() {
    CerrarArchivo();
}

/**
 * Inserta una figura nueva en el sistema de archivos.
 *
 * Verifica que el sistema este abierto, que la figura no exista ya
 * en el directorio y que haya una entrada libre. Luego serializa la
 * figura, reserva los bloques necesarios, escribe su contenido en la
 * zona de datos y registra la figura en el directorio.
 *
 * @param figura Estructura que contiene la informacion de la figura a insertar.
 * @return 0 si la figura se inserto correctamente, -1 en caso de error.
 */
int SistemaArchivos::InsertarFigura(Figura& figura) {
    int resultado = -1;
    HeaderFS header;
    EntradaDirectorio entrada;
    int indiceLibre = -1;
    int indiceExistente = -1;
    EntradaDirectorio existente;
    int tamanoLogico = 0;
    int bloquesNecesarios = 0;
    char* buffer = 0;
    int* bloques = 0;
    int i = 0;
    int bytesCopiados = 0;
    int bytesRestantes = 0;
    int bytesEsteBloque = 0;
    BloqueDatos bloque;

    // valida que el archivo del sistema este abierto
    if ( !archivo.is_open() ) {
        return resultado;
    }

    LeerHeader(header);

    // evita insertar una figura repetida
    if ( BuscarEntradaDirectorio(header, figura.nombreFigura, figura.mitad, existente, indiceExistente) == 1 ) {
        return resultado;
    }

    // busca una entrada libre en el directorio
    indiceLibre = BuscarEntradaDirectorioLibre(header);

    if ( indiceLibre == -1 ) {
        return resultado;
    }

    // calcula espacio necesario para almacenar la figura
    tamanoLogico = CalcularTamanoLogicoFigura(figura);
    bloquesNecesarios = CalcularBloquesNecesarios(figura);

    // serializa la figura en memoria continua
    buffer = new char[tamanoLogico];
    memset(buffer, 0, tamanoLogico);
    SerializarFigura(figura, buffer, tamanoLogico);

    // prepara arreglo de bloques reservados
    bloques = new int[bloquesNecesarios];
    for ( i = 0; i < bloquesNecesarios; i++ ) {
        bloques[i] = BLOQUE_INVALIDO;
    }

    // reserva bloques y escribir la cadena de datos
    if ( ReservarBloquesParaFigura(header, bloquesNecesarios, bloques) == 0 ) {
        bytesCopiados = 0;

        for ( i = 0; i < bloquesNecesarios; i++ ) {
            LimpiarBloqueDatos(bloque);

            bytesRestantes = tamanoLogico - bytesCopiados;
            bytesEsteBloque = 252;

            if ( bytesRestantes < bytesEsteBloque ) {
                bytesEsteBloque = bytesRestantes;
            }

            memcpy(bloque.datos, buffer + bytesCopiados, bytesEsteBloque);
            bytesCopiados += bytesEsteBloque;

            if ( i < bloquesNecesarios - 1 ) {
                bloque.siguienteBloque = bloques[i + 1];
            } else {
                bloque.siguienteBloque = FIN_CADENA;
            }

            manejadorBloques.EscribirBloqueDatos(archivo, bloques[i], bloque);
        }

        // crea la entrada de directorio para la figura
        LimpiarEntradaDirectorio(entrada);
        entrada.idFigura = figura.idFigura;
        strncpy(entrada.nombreFigura, figura.nombreFigura, TAM_NOMBRE_FIGURA - 1);
        entrada.nombreFigura[TAM_NOMBRE_FIGURA - 1] = '\0';
        entrada.mitad = figura.mitad;
        entrada.bloqueInicial = bloques[0];
        entrada.cantidadBloques = bloquesNecesarios;
        entrada.tamanoLogico = tamanoLogico;
        entrada.estado = ENTRADA_ACTIVA;

        EscribirEntradaDirectorio(header, indiceLibre, entrada);

        // actualiza header con la nueva figura activa
        header.totalFigurasActivas++;
        EscribirHeader(header);

        resultado = 0;
    }

    delete[] buffer;
    delete[] bloques;

    return resultado;
}
/**
 * Obtiene una figura almacenada en el sistema de archivos.
 *
 * Busca la entrada correspondiente en el directorio, recorre la cadena
 * de bloques de datos asociada, reconstruye el contenido completo en
 * memoria y luego deserializa la figura para devolverla.
 *
 * @param nombre Nombre de la figura a buscar.
 * @param mitad Mitad de la figura que se desea obtener.
 * @param figura Estructura donde se almacenara la figura recuperada.
 * @return 0 si la figura se obtuvo correctamente, -1 en caso de error.
 */
int SistemaArchivos::ObtenerFigura(const char* nombre, int mitad, Figura& figura) {
    int resultado = -1;
    HeaderFS header;
    EntradaDirectorio entrada;
    int indice = -1;
    char* buffer = 0;
    int offset = 0;
    int actual = 0;
    int bytesRestantes = 0;
    int bytesCopiar = 0;
    BloqueDatos bloque;

    // valida que el archivo del sistema este abierto
    if ( !archivo.is_open() ) {
        return resultado;
    }

    LeerHeader(header);

    // busca la figura en el directorio
    if ( BuscarEntradaDirectorio(header, nombre, mitad, entrada, indice) == 1 ) {
        buffer = new char[entrada.tamanoLogico];
        memset(buffer, 0, entrada.tamanoLogico);

        actual = entrada.bloqueInicial;
        offset = 0;

        // reconstruye el contenido logico leyendo la cadena de bloques
        while ( actual != FIN_CADENA && actual != BLOQUE_INVALIDO && offset < entrada.tamanoLogico ) {
            LimpiarBloqueDatos(bloque);
            manejadorBloques.LeerBloqueDatos(archivo, actual, bloque);

            bytesRestantes = entrada.tamanoLogico - offset;
            bytesCopiar = 252;

            if ( bytesRestantes < bytesCopiar ) {
                bytesCopiar = bytesRestantes;
            }

            memcpy(buffer + offset, bloque.datos, bytesCopiar);
            offset += bytesCopiar;
            actual = bloque.siguienteBloque;
        }

        // convierte los bytes leidos nuevamente en una figura
        DeserializarFigura(buffer, entrada.tamanoLogico, figura);
        delete[] buffer;

        resultado = 0;
    }

    return resultado;
}
/**
 * Elimina una figura del sistema de archivos.
 *
 * Busca la entrada correspondiente en el directorio, libera la cadena
 * de bloques de datos asociada, marca la entrada como libre y actualiza
 * la cantidad de figuras activas en el header.
 *
 * @param nombre Nombre de la figura a eliminar.
 * @param mitad Mitad de la figura que se desea eliminar.
 * @return 0 si la figura se elimino correctamente, -1 en caso de error.
 */
int SistemaArchivos::EliminarFigura(const char* nombre, int mitad) {
    int resultado = -1;
    HeaderFS header;
    EntradaDirectorio entrada;
    int indice = -1;

    // valida que el archivo del sistema este abierto
    if ( !archivo.is_open() ) {
        return resultado;
    }

    LeerHeader(header);

    // busca la figura en el directorio
    if ( BuscarEntradaDirectorio(header, nombre, mitad, entrada, indice) == 1 ) {
        // libera bloques de datos ocupados por la figura
        LiberarCadenaBloques(header, entrada.bloqueInicial);

        // marca la entrada del directorio como libre
        LimpiarEntradaDirectorio(entrada);
        entrada.estado = ENTRADA_LIBRE;
        EscribirEntradaDirectorio(header, indice, entrada);

        // actualiza contador de figuras activas
        if ( header.totalFigurasActivas > 0 ) {
            header.totalFigurasActivas--;
        }

        EscribirHeader(header);
        resultado = 0;
    }

    return resultado;
}

/**
 * Muestra en pantalla las figuras activas registradas en el directorio.
 *
 * Recorre todas las entradas del directorio y despliega la informacion
 * de aquellas que se encuentren activas.
 */
void SistemaArchivos::ListarFiguras() {
    HeaderFS header;
    int capacidad = 0;
    int i = 0;
    EntradaDirectorio entrada;

    // valida que el archivo del sistema este abierto
    if ( !archivo.is_open() ) {
        return;
    }

    LeerHeader(header);
    capacidad = CapacidadTotalDirectorio(header);

    cout << "Figuras en el directorio:" << endl;

    // recorre entradas y mostrar solo las activas
    for ( i = 0; i < capacidad; i++ ) {
        LeerEntradaDirectorio(header, i, entrada);

        if ( entrada.estado == ENTRADA_ACTIVA ) {
            cout << "Indice: " << i << endl;
            cout << "  ID: " << entrada.idFigura << endl;
            cout << "  Nombre: " << entrada.nombreFigura << endl;
            cout << "  Mitad: " << entrada.mitad << endl;
            cout << "  Bloque inicial: " << entrada.bloqueInicial << endl;
            cout << "  Cantidad de bloques: " << entrada.cantidadBloques << endl;
            cout << "  Tamano logico: " << entrada.tamanoLogico << endl;
            cout << endl;
        }
    }
}

/**
 * Construye un texto con el listado de figuras activas del directorio.
 *
 * Recorre todas las entradas del directorio y genera una cadena
 * con el formato "nombre|mitad" por cada figura activa.
 *
 * @return Cadena con el listado de figuras o mensaje de error.
 */
string SistemaArchivos::ConstruirTextoListadoFiguras() {
    HeaderFS header;
    int capacidad = 0;
    int i = 0;
    EntradaDirectorio entrada;
    ostringstream salida;

    // valida que el sistema este abierto
    if ( !archivo.is_open() ) {
        return "Error: sistema de archivos no abierto.\n";
    }

    LeerHeader(header);
    capacidad = CapacidadTotalDirectorio(header);

    // recorre entradas y construir texto
    for ( i = 0; i < capacidad; i++ ) {
        LeerEntradaDirectorio(header, i, entrada);

        if ( entrada.estado == ENTRADA_ACTIVA ) {
            salida << entrada.nombreFigura << "|" << entrada.mitad << "\n";
        }
    }

    return salida.str();
}

/**
 * Construye el texto plano con la informacion de una figura.
 *
 * Obtiene la figura desde el sistema de archivos y genera una
 * cadena con su nombre, mitad y listado de piezas.
 *
 * @param nombre Nombre de la figura solicitada.
 * @param mitad Mitad de la figura solicitada.
 * @return Texto con la informacion de la figura o mensaje de error.
 */
string SistemaArchivos::ConstruirTextoFigura(const char* nombre, int mitad) {
    Figura figura;
    int resultado = 0;
    int i = 0;
    ostringstream salida;

    // obtener figura desde el sistema
    resultado = ObtenerFigura(nombre, mitad, figura);

    if ( resultado != 0 ) {
        return "Error: figura no encontrada.\n";
    }

    // construir cuerpo en texto plano
    salida << "figura=" << figura.nombreFigura << "\n";
    salida << "mitad=" << figura.mitad << "\n";

    for ( i = 0; i < figura.cantidadPiezas; i++ ) {
        salida << figura.piezas[i].nombre << "=" << figura.piezas[i].cantidad << "\n";
    }

    return salida.str();
}

/**
 * Construye una respuesta HTTP 200 OK con cuerpo en texto plano.
 *
 * Genera los encabezados HTTP basicos y agrega el cuerpo recibido.
 *
 * @param cuerpo Contenido de la respuesta.
 * @return Respuesta HTTP completa con codigo 200.
 */
string SistemaArchivos::ConstruirRespuestaHTTP200(const string& cuerpo) {
    ostringstream salida;

    // encabezados de respuesta exitosa
    salida << "HTTP/1.1 200 OK\r\n";
    salida << "Content-Type: text/plain\r\n";
    salida << "Content-Length: " << cuerpo.size() << "\r\n";
    salida << "Connection: close\r\n";
    salida << "\r\n";
    salida << cuerpo;

    return salida.str();
}

/**
 * Construye una respuesta HTTP 404 Not Found con cuerpo en texto plano.
 *
 * Genera los encabezados HTTP para indicar que el recurso no fue encontrado.
 *
 * @param cuerpo Contenido de la respuesta.
 * @return Respuesta HTTP completa con codigo 404.
 */
string SistemaArchivos::ConstruirRespuestaHTTP404(const string& cuerpo) {
    ostringstream salida;

    // encabezados de recurso no encontrado
    salida << "HTTP/1.1 404 Not Found\r\n";
    salida << "Content-Type: text/plain\r\n";
    salida << "Content-Length: " << cuerpo.size() << "\r\n";
    salida << "Connection: close\r\n";
    salida << "\r\n";
    salida << cuerpo;

    return salida.str();
}

/**
 * Construye una respuesta HTTP 400 Bad Request con cuerpo en texto plano.
 *
 * Genera los encabezados HTTP para indicar que la solicitud es invalida.
 *
 * @param cuerpo Contenido de la respuesta.
 * @return Respuesta HTTP completa con codigo 400.
 */
string SistemaArchivos::ConstruirRespuestaHTTP400(const string& cuerpo) {
    ostringstream salida;

    // encabezados de solicitud invalida
    salida << "HTTP/1.1 400 Bad Request\r\n";
    salida << "Content-Type: text/plain\r\n";
    salida << "Content-Length: " << cuerpo.size() << "\r\n";
    salida << "Connection: close\r\n";
    salida << "\r\n";
    salida << cuerpo;

    return salida.str();
}

/**
 * Construye la respuesta HTTP para el listado de figuras.
 *
 * Genera el cuerpo con las figuras activas y lo envuelve en una
 * respuesta HTTP exitosa.
 *
 * @return Respuesta HTTP con el listado de figuras.
 */
string SistemaArchivos::ConstruirRespuestaHTTPListado() {
    string cuerpo;

    // construye listado en texto plano
    cuerpo = ConstruirTextoListadoFiguras();
    return ConstruirRespuestaHTTP200(cuerpo);
}

/**
 * Construye la respuesta HTTP para la consulta de una figura.
 *
 * Valida la mitad solicitada, genera el texto de la figura y retorna
 * una respuesta HTTP 200, 400 o 404 segun corresponda.
 *
 * @param nombre Nombre de la figura solicitada.
 * @param mitad Mitad de la figura solicitada.
 * @return Respuesta HTTP correspondiente al resultado de la consulta.
 */
string SistemaArchivos::ConstruirRespuestaHTTPFigura(const char* nombre, int mitad) {
    string cuerpo;

    // valida mitad permitida
    if ( mitad != 1 && mitad != 2 ) {
        return ConstruirRespuestaHTTP400("Error: mitad invalida.\n");
    }

    // construye cuerpo de la figura solicitada
    cuerpo = ConstruirTextoFigura(nombre, mitad);

    if ( cuerpo == "Error: figura no encontrada.\n" ) {
        return ConstruirRespuestaHTTP404(cuerpo);
    }

    return ConstruirRespuestaHTTP200(cuerpo);
}