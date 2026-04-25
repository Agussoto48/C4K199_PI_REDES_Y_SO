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

      //Apertura interna del archivo
      int AbrirArchivo(const char*);
      void CerrarArchivo();

      //Header
      void LeerHeader(HeaderFS&);
      void EscribirHeader(const HeaderFS&);

      //Bitmap
      void LeerBitmap(const HeaderFS&, char*);
      void EscribirBitmap(const HeaderFS&, const char*);

      int ObtenerBit(const char*, int);
      void PonerBit(char*, int, int);

      int BuscarBloqueLibre(HeaderFS&);
      void MarcarBloque(HeaderFS&, int, int);

      //Directorio
      int CantidadEntradasPorBloqueDirectorio();
      int CapacidadTotalDirectorio(const HeaderFS&);

      void LeerEntradaDirectorio(const HeaderFS&, int, EntradaDirectorio&);
      void EscribirEntradaDirectorio(const HeaderFS&, int, const EntradaDirectorio&);

      int BuscarEntradaDirectorioLibre(const HeaderFS&);
      int BuscarEntradaDirectorio(const HeaderFS&, const char*, int,
                                 EntradaDirectorio&, int&);

      //Manejo de figuras
      int CalcularTamanoLogicoFigura(Figura&);
      int CalcularBloquesNecesarios(Figura&);

      void SerializarFigura(Figura&, char*, int);
      void DeserializarFigura(char*, int, Figura&);

      int ReservarBloquesParaFigura(HeaderFS&, int, int*);
      void LiberarCadenaBloques(HeaderFS&, int);

      //Utilidades
      int CompararNombreFigura(const char*, const char*);

   public:
      SistemaArchivos();
      ~SistemaArchivos();

      //Opciones generales del sistema
      int CrearSistema(const char*, int, int);
      int AbrirSistema(const char*);
      void CerrarSistema();

      //Operaciones sobre figuras
      int InsertarFigura(Figura&);
      int ObtenerFigura(const char*, int, Figura&);
      int EliminarFigura(const char*, int);
      void ListarFiguras();

      //Funciones para HTTP
      string ConstruirTextoListadoFiguras();
      string ConstruirTextoFigura(const char*, int);

      string ConstruirRespuestaHTTP200(const string&);
      string ConstruirRespuestaHTTP404(const string&);
      string ConstruirRespuestaHTTP400(const string&);

      string ConstruirRespuestaHTTPListado();
      string ConstruirRespuestaHTTPFigura(const char*, int);
};

#endif