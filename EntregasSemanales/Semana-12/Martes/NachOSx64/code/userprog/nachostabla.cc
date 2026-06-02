/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2026-i
  *
  *  Tabla de archivos abiertos de NachOS
  *
 **/

#include <unistd.h>     // close()
#include "nachostabla.h"


/**
  *  Constructor
  *  Inicializa la tabla y el bitmap
  *  Las posiciones 0, 1 y 2 se reservan para stdin, stdout y stderr
 **/
NachosOpenFilesTable::NachosOpenFilesTable() {

   openFiles    = new int[ MAX_FILES ];
   openFilesMap = new BitMap( MAX_FILES );
   usage        = 0;

   // Reservar stdin(0), stdout(1), stderr(2)
   openFiles[ 0 ] = 0;   openFilesMap->Mark( 0 );
   openFiles[ 1 ] = 1;   openFilesMap->Mark( 1 );
   openFiles[ 2 ] = 2;   openFilesMap->Mark( 2 );

}


/**
  *  Destructor
  *  Libera el arreglo y el bitmap
 **/
NachosOpenFilesTable::~NachosOpenFilesTable() {

   delete[] openFiles;
   delete openFilesMap;

}


/**
  *  Open
  *  Registra un archivo ya abierto en Unix dentro de la tabla
  *  Retorna el handle de NachOS (posicion en en la tabla), o -1 si no hay espacio
 **/
int NachosOpenFilesTable::Open( int unixHandle ) {

   int nachosHandle = openFilesMap->Find();   // Encuentra libre


   if ( nachosHandle == -1 ) {
      return -1;   // Tabla llena
   
   }

   openFiles[ nachosHandle ] = unixHandle;    //relacion nachos a unix

   return nachosHandle;

}


/**
  *  Close
  *  Desregistra un archivo de la tabla
  *  Retorna 0 si ok, -1 si el handle no estaba abierto
 **/
int NachosOpenFilesTable::Close( int nachosHandle ) {

   if ( !isOpened( nachosHandle ) ) {
      return -1;
   }

   openFilesMap->Clear( nachosHandle );   // libera la posicion 

   return 0;

}


/**
  *  getUnixHandle
  *  con la handle de nachos retorna el handle Unix 
  *  Retorna -1 si no está abierto
 **/

int NachosOpenFilesTable::getUnixHandle( int nachosHandle ) {

   if ( !isOpened( nachosHandle ) ) {
      return -1;

   }

   return openFiles[ nachosHandle ];

}


/**
  *  isOpened
  *  Verifica si un handle de NachOS esta en uso
 **/
bool NachosOpenFilesTable::isOpened( int nachosHandle ) {

   if ( nachosHandle < 0 || nachosHandle >= MAX_FILES ) {
      return false;
   }

   return openFilesMap->Test( nachosHandle );

}


/**
  *  addThread
  *  Un hilo mas esta usando esta tabla
 **/
void NachosOpenFilesTable::addThread() {

   usage++;

}


/**
  *  delThread
  *  Un hilo deja de usar esta tabla
  *  Si es el ultimo hilo, cierra todos los archivos abiertos
 **/
void NachosOpenFilesTable::delThread() {

   usage--;

   if ( usage == 0 ) {
      // Cerrar todos los archivos abiertos excepto stdin(0), stdout(1), stderr(2)
      for ( int i = 3; i < MAX_FILES; i++ ) {
         if ( openFilesMap->Test( i ) ) {
            close( openFiles[ i ] );   // Cierra el archivo Unix
            openFilesMap->Clear( i );
         }
      }
   }

}
