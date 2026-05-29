#ifndef SISTEMA_ARCHIVOS_H
#define SISTEMA_ARCHIVOS_H

#include <fstream>
using namespace std;

#include "EstructurasFS.h"
#include "Bloque.h"
#include "Figura.h"

class SistemaArchivos {
   private:
      fstream archivo;
      char nombreArchivo[128];
      Bloque manejadorBloques;

      /*
       * Apertura interna del archivo
       */
      int AbrirArchivo(const char* ruta);
      void CerrarArchivo();

      /*
       * Header
       */
      void LeerHeader(HeaderFS& header);
      void EscribirHeader(const HeaderFS& header);

      /*
       * Bitmap
       */
      void LeerBitmap(const HeaderFS& header, char* bitmap);
      void EscribirBitmap(const HeaderFS& header, const char* bitmap);

      int ObtenerBit(const char* bitmap, int posicion);
      void PonerBit(char* bitmap, int posicion, int valor);

      int BuscarBloqueLibre(HeaderFS& header);
      void MarcarBloque(HeaderFS& header, int numeroBloque, int ocupado);

      /*
       * Directorio
       */
      int CantidadEntradasPorBloqueDirectorio();
      int CapacidadTotalDirectorio(const HeaderFS& header);

      void LeerEntradaDirectorio(const HeaderFS& header, int indice, EntradaDirectorio& entrada);
      void EscribirEntradaDirectorio(const HeaderFS& header, int indice, const EntradaDirectorio& entrada);

      int BuscarEntradaDirectorioLibre(const HeaderFS& header);
      int BuscarEntradaDirectorio(const HeaderFS& header, const char* nombre, int mitad,
                                 EntradaDirectorio& entradaEncontrada, int& indiceEncontrado);

      /*
       * Manejo de figuras
       */
      int CalcularTamanoLogicoFigura(Figura& figura);
      int CalcularBloquesNecesarios(Figura& figura);

      void SerializarFigura(Figura& figura, char* buffer, int tamanoBuffer);
      void DeserializarFigura(char* buffer, int tamanoBuffer, Figura& figura);

      int ReservarBloquesParaFigura(HeaderFS& header, int cantidadBloques, int* bloquesReservados);
      void LiberarCadenaBloques(HeaderFS& header, int bloqueInicial);

      /*
       * Utilidades
       */
      int CompararNombreFigura(const char* nombre1, const char* nombre2);

   public:
      SistemaArchivos();
      ~SistemaArchivos();

      /*
       * Operaciones generales del sistema
       */
      int CrearSistema(const char* ruta, int totalBloques, int bloquesDirectorio);
      int AbrirSistema(const char* ruta);
      void CerrarSistema();

      /*
       * Operaciones sobre figuras
       */
      int InsertarFigura(Figura& figura);
      int ObtenerFigura(const char* nombre, int mitad, Figura& figura);
      int EliminarFigura(const char* nombre, int mitad);
      void ListarFiguras();

      /*
       * Texto plano para respuestas
       */
      string ConstruirTextoListadoFiguras();
      string ConstruirTextoFigura(const char* nombre, int mitad);
};

#endif