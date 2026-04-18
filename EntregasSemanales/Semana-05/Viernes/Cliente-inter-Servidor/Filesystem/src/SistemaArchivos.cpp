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

void SistemaArchivos::CerrarArchivo() {
    if ( archivo.is_open() ) {
        archivo.close();
    }

    nombreArchivo[0] = '\0';
}

void SistemaArchivos::LeerHeader(HeaderFS& header) {
    BloqueGenerico bloque;

    LimpiarBloqueGenerico(bloque);
    LimpiarHeader(header);

    manejadorBloques.LeerBloqueGenerico(archivo, 0, bloque);
    memcpy(&header, bloque.bytes, sizeof(HeaderFS));
}

void SistemaArchivos::EscribirHeader(const HeaderFS& header) {
    BloqueGenerico bloque;

    LimpiarBloqueGenerico(bloque);
    memcpy(bloque.bytes, &header, sizeof(HeaderFS));
    manejadorBloques.EscribirBloqueGenerico(archivo, 0, bloque);
}

void SistemaArchivos::LeerBitmap(const HeaderFS& header, char* bitmap) {
    BloqueGenerico bloque;
    int i = 0;

    for ( i = 0; i < header.cantidadBloquesBitmap; i++ ) {
        LimpiarBloqueGenerico(bloque);
        manejadorBloques.LeerBloqueGenerico(archivo, header.bloqueInicioBitmap + i, bloque);
        memcpy(bitmap + (i * TAM_BLOQUE), bloque.bytes, TAM_BLOQUE);
    }
}

void SistemaArchivos::EscribirBitmap(const HeaderFS& header, const char* bitmap) {
    BloqueGenerico bloque;
    int i = 0;

    for ( i = 0; i < header.cantidadBloquesBitmap; i++ ) {
        LimpiarBloqueGenerico(bloque);
        memcpy(bloque.bytes, bitmap + (i * TAM_BLOQUE), TAM_BLOQUE);
        manejadorBloques.EscribirBloqueGenerico(archivo, header.bloqueInicioBitmap + i, bloque);
    }
}

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

int SistemaArchivos::CantidadEntradasPorBloqueDirectorio() {
    int cantidad = 0;
    cantidad = 252 / (int) sizeof(EntradaDirectorio);
    return cantidad;
}

int SistemaArchivos::CapacidadTotalDirectorio(const HeaderFS& header) {
    int capacidad = 0;
    capacidad = CantidadEntradasPorBloqueDirectorio() * header.cantidadBloquesDirectorio;
    return capacidad;
}

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

    memcpy(&entrada,
           bloque.datos + (desplazamientoEntrada * sizeof(EntradaDirectorio)),
           sizeof(EntradaDirectorio));
}

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

    memcpy(bloque.datos + (desplazamientoEntrada * sizeof(EntradaDirectorio)),
           &entrada,
           sizeof(EntradaDirectorio));

    manejadorBloques.EscribirBloqueDirectorio(archivo, bloqueFisico, bloque);
}

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

int SistemaArchivos::CompararNombreFigura(const char* nombre1, const char* nombre2) {
    int resultado = 0;

    if ( strcmp(nombre1, nombre2) == 0 ) {
        resultado = 1;
    }

    return resultado;
}

int SistemaArchivos::BuscarEntradaDirectorio(const HeaderFS& header,
                                             const char* nombre,
                                             int mitad,
                                             EntradaDirectorio& entradaEncontrada,
                                             int& indiceEncontrado) {
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

int SistemaArchivos::CalcularTamanoLogicoFigura(Figura& figura) {
    int tamano = 0;

    tamano = sizeof(EncabezadoFigura) + (figura.cantidadPiezas * (int) sizeof(RegistroPieza));

    return tamano;
}

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

    figura.Inicializar(encabezado.idFigura,
                       encabezado.nombreFigura,
                       encabezado.mitad,
                       encabezado.cantidadPiezas);

    for ( i = 0; i < encabezado.cantidadPiezas; i++ ) {
        if ( offset + (int) sizeof(RegistroPieza) <= tamanoBuffer ) {
            LimpiarRegistroPieza(reg);
            memcpy(&reg, buffer + offset, sizeof(RegistroPieza));
            figura.AgregarPieza(i, reg.nombrePieza, reg.cantidad);
            offset += sizeof(RegistroPieza);
        }
    }
}

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

    creador.open(ruta, ios::out | ios::binary | ios::trunc);

    if ( creador.is_open() ) {
        LimpiarBloqueGenerico(bloqueVacio);

        for ( i = 0; i < totalBloques; i++ ) {
            creador.write(bloqueVacio.bytes, TAM_BLOQUE);
        }

        creador.close();

        if ( AbrirArchivo(ruta) == 0 ) {
            LimpiarHeader(header);

            strncpy(header.magic, FS_MAGIC, sizeof(header.magic) - 1);
            header.magic[sizeof(header.magic) - 1] = '\0';

            strncpy(header.nombreFS, "LegoFS", TAM_NOMBRE_FS - 1);
            header.nombreFS[TAM_NOMBRE_FS - 1] = '\0';

            strncpy(header.version, "1.0", TAM_VERSION_FS - 1);
            header.version[TAM_VERSION_FS - 1] = '\0';

            header.tamBloque = TAM_BLOQUE;
            header.totalBloques = totalBloques;
            header.totalFigurasActivas = 0;

            bytesBitmapNecesarios = totalBloques / 8;
            if ( (totalBloques % 8) != 0 ) {
                bytesBitmapNecesarios++;
            }

            bloquesBitmap = bytesBitmapNecesarios / TAM_BLOQUE;
            if ( (bytesBitmapNecesarios % TAM_BLOQUE) != 0 ) {
                bloquesBitmap++;
            }

            header.bloqueInicioBitmap = 1;
            header.cantidadBloquesBitmap = bloquesBitmap;
            header.bloqueInicioDirectorio = header.bloqueInicioBitmap + bloquesBitmap;
            header.cantidadBloquesDirectorio = bloquesDirectorio;
            header.bloqueInicioDatos = header.bloqueInicioDirectorio + bloquesDirectorio;

            EscribirHeader(header);

            bytesBitmap = header.cantidadBloquesBitmap * TAM_BLOQUE;
            bitmap = new char[bytesBitmap];
            memset(bitmap, 0, bytesBitmap);

            PonerBit(bitmap, 0, 1);

            for ( i = 0; i < header.cantidadBloquesBitmap; i++ ) {
                PonerBit(bitmap, header.bloqueInicioBitmap + i, 1);
            }

            for ( i = 0; i < header.cantidadBloquesDirectorio; i++ ) {
                PonerBit(bitmap, header.bloqueInicioDirectorio + i, 1);
            }

            EscribirBitmap(header, bitmap);
            delete[] bitmap;

            for ( i = 0; i < header.cantidadBloquesDirectorio; i++ ) {
                memset(&bloqueDir, 0, sizeof(BloqueDirectorio));

                if ( i < header.cantidadBloquesDirectorio - 1 ) {
                    bloqueDir.siguienteBloque = header.bloqueInicioDirectorio + i + 1;
                } else {
                    bloqueDir.siguienteBloque = FIN_CADENA;
                }

                manejadorBloques.EscribirBloqueDirectorio(archivo,
                                                          header.bloqueInicioDirectorio + i,
                                                          bloqueDir);
            }

            CerrarArchivo();
            resultado = 0;
        }
    }

    return resultado;
}

int SistemaArchivos::AbrirSistema(const char* ruta) {
    int resultado = -1;
    HeaderFS header;

    if ( AbrirArchivo(ruta) == 0 ) {
        LeerHeader(header);

        if ( strcmp(header.magic, FS_MAGIC) == 0 ) {
            resultado = 0;
        } else {
            CerrarArchivo();
        }
    }

    return resultado;
}

void SistemaArchivos::CerrarSistema() {
    CerrarArchivo();
}

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

    if ( !archivo.is_open() ) {
        return resultado;
    }

    LeerHeader(header);

    if ( BuscarEntradaDirectorio(header, figura.nombreFigura, figura.mitad, existente, indiceExistente) == 1 ) {
        return resultado;
    }

    indiceLibre = BuscarEntradaDirectorioLibre(header);

    if ( indiceLibre == -1 ) {
        return resultado;
    }

    tamanoLogico = CalcularTamanoLogicoFigura(figura);
    bloquesNecesarios = CalcularBloquesNecesarios(figura);

    buffer = new char[tamanoLogico];
    memset(buffer, 0, tamanoLogico);
    SerializarFigura(figura, buffer, tamanoLogico);

    bloques = new int[bloquesNecesarios];
    for ( i = 0; i < bloquesNecesarios; i++ ) {
        bloques[i] = BLOQUE_INVALIDO;
    }

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

        header.totalFigurasActivas++;
        EscribirHeader(header);

        resultado = 0;
    }

    delete[] buffer;
    delete[] bloques;

    return resultado;
}

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

    if ( !archivo.is_open() ) {
        return resultado;
    }

    LeerHeader(header);

    if ( BuscarEntradaDirectorio(header, nombre, mitad, entrada, indice) == 1 ) {
        buffer = new char[entrada.tamanoLogico];
        memset(buffer, 0, entrada.tamanoLogico);

        actual = entrada.bloqueInicial;
        offset = 0;

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

        DeserializarFigura(buffer, entrada.tamanoLogico, figura);
        delete[] buffer;

        resultado = 0;
    }

    return resultado;
}

int SistemaArchivos::EliminarFigura(const char* nombre, int mitad) {
    int resultado = -1;
    HeaderFS header;
    EntradaDirectorio entrada;
    int indice = -1;

    if ( !archivo.is_open() ) {
        return resultado;
    }

    LeerHeader(header);

    if ( BuscarEntradaDirectorio(header, nombre, mitad, entrada, indice) == 1 ) {
        LiberarCadenaBloques(header, entrada.bloqueInicial);

        LimpiarEntradaDirectorio(entrada);
        entrada.estado = ENTRADA_LIBRE;
        EscribirEntradaDirectorio(header, indice, entrada);

        if ( header.totalFigurasActivas > 0 ) {
            header.totalFigurasActivas--;
        }

        EscribirHeader(header);
        resultado = 0;
    }

    return resultado;
}

void SistemaArchivos::ListarFiguras() {
    HeaderFS header;
    int capacidad = 0;
    int i = 0;
    EntradaDirectorio entrada;

    if ( !archivo.is_open() ) {
        return;
    }

    LeerHeader(header);
    capacidad = CapacidadTotalDirectorio(header);

    cout << "===== FIGURAS EN DIRECTORIO =====" << endl;

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

string SistemaArchivos::ConstruirTextoListadoFiguras() {
    HeaderFS header;
    int capacidad = 0;
    int i = 0;
    EntradaDirectorio entrada;
    ostringstream salida;

    if ( !archivo.is_open() ) {
        return "Error: sistema de archivos no abierto.\n";
    }

    LeerHeader(header);
    capacidad = CapacidadTotalDirectorio(header);

    for ( i = 0; i < capacidad; i++ ) {
        LeerEntradaDirectorio(header, i, entrada);

        if ( entrada.estado == ENTRADA_ACTIVA ) {
            salida << entrada.nombreFigura << "|" << entrada.mitad << "\n";
        }
    }

    return salida.str();
}

string SistemaArchivos::ConstruirTextoFigura(const char* nombre, int mitad) {
    Figura figura;
    int resultado = 0;
    int i = 0;
    ostringstream salida;

    resultado = ObtenerFigura(nombre, mitad, figura);

    if ( resultado != 0 ) {
        return "Error: figura no encontrada.\n";
    }

    salida << "figura=" << figura.nombreFigura << "\n";
    salida << "mitad=" << figura.mitad << "\n";

    for ( i = 0; i < figura.cantidadPiezas; i++ ) {
        salida << figura.piezas[i].nombre << "=" << figura.piezas[i].cantidad << "\n";
    }

    return salida.str();
}

string SistemaArchivos::ConstruirRespuestaHTTP200(const string& cuerpo) {
    ostringstream salida;

    salida << "HTTP/1.1 200 OK\r\n";
    salida << "Content-Type: text/plain\r\n";
    salida << "Content-Length: " << cuerpo.size() << "\r\n";
    salida << "Connection: close\r\n";
    salida << "\r\n";
    salida << cuerpo;

    return salida.str();
}

string SistemaArchivos::ConstruirRespuestaHTTP404(const string& cuerpo) {
    ostringstream salida;

    salida << "HTTP/1.1 404 Not Found\r\n";
    salida << "Content-Type: text/plain\r\n";
    salida << "Content-Length: " << cuerpo.size() << "\r\n";
    salida << "Connection: close\r\n";
    salida << "\r\n";
    salida << cuerpo;

    return salida.str();
}

string SistemaArchivos::ConstruirRespuestaHTTP400(const string& cuerpo) {
    ostringstream salida;

    salida << "HTTP/1.1 400 Bad Request\r\n";
    salida << "Content-Type: text/plain\r\n";
    salida << "Content-Length: " << cuerpo.size() << "\r\n";
    salida << "Connection: close\r\n";
    salida << "\r\n";
    salida << cuerpo;

    return salida.str();
}

string SistemaArchivos::ConstruirRespuestaHTTPListado() {
    string cuerpo;

    cuerpo = ConstruirTextoListadoFiguras();
    return ConstruirRespuestaHTTP200(cuerpo);
}


string SistemaArchivos::ConstruirRespuestaHTTPFigura(const char* nombre, int mitad) {
    string cuerpo;

    if ( mitad != 1 && mitad != 2 ) {
        return ConstruirRespuestaHTTP400("Error: mitad invalida.\n");
    }

    cuerpo = ConstruirTextoFigura(nombre, mitad);

    if ( cuerpo == "Error: figura no encontrada.\n" ) {
        return ConstruirRespuestaHTTP404(cuerpo);
    }

    return ConstruirRespuestaHTTP200(cuerpo);
}
