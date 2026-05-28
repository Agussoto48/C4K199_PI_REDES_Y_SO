/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2026-i
  *
  *  Tabla de archivos abiertos de Nachos
  *
 **/

#ifndef NACHOSTABLA_H
#define NACHOSTABLA_H

#include "bitmap.h"

#define MAX_FILES 128   // Máximo de archivos abiertos simultáneamente

class NachosOpenFilesTable {
   public:
      NachosOpenFilesTable();           // Constructor: inicializa la tabla
      ~NachosOpenFilesTable();          // Destructor: libera memoria

      int Open( int unixHandle );       // Registra un archivo abierto, retorna handle de NachOS
      int Close( int nachosHandle );    // Desregistra un archivo, retorna 0 si ok
      int getUnixHandle( int nachosHandle ); // Retorna el handle Unix dado un handle NachOS
      bool isOpened( int nachosHandle );     // Verifica si un handle está abierto

      void addThread();                 // Incrementa contador de hilos usando esta tabla
      void delThread();                 // Decrementa contador; si es el último cierra todo

   private:
      int * openFiles;                  // Arreglo: índice = handle NachOS, valor = handle Unix
      BitMap * openFilesMap;            // Bitmap para saber qué posiciones están ocupadas
      int usage;                        // Cuántos hilos están usando esta tabla
};

#endif // NACHOSTABLA_H
