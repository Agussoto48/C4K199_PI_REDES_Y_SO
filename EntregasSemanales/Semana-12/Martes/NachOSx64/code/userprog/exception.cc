// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Copyright (c) -2025 Universidad de Costa Rica


#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "nachostabla.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>



#define SC_NachOS	015177

NachosOpenFilesTable * openFilesTable = nullptr;


//Auxiliares 
//leer user input
static void readUserString( int vaddr, char * buf, int maxLen ) {
   int val, i = 0;
   do {
      machine->ReadMem( vaddr + i, 1, &val );
      buf[i] = (char) val;
      i++;
   } while ( val != 0 && i < maxLen - 1 );
   buf[i] = '\0';
}

//auxiliar para iniciar la tabla

static void initTable() {
   if ( openFilesTable == nullptr ) {
      openFilesTable = new NachosOpenFilesTable();
   }
}

// avanza los registros del pc 
//evita que nachos haga la misam instruccion (consultado con companeros porque no servia)
static void AvanzarPC()
{
    // guarda la instruccion actual 
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));

    // avanza el pc
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));

    // siguiente PC avanzando 4 bytes.
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
}




/*
 *  System call interface: Halt()
 */
void NachOS_Halt() {		// System call 0

	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();

}


/*
 *  System call interface: void Exit( int )
 */
void NachOS_Exit() {		// System call 1
   DEBUG('u', "Exit system call\n");

   //liberar espacio de direcciones del proceso 
   if (currentThread->space != NULL)
   {
      delete currentThread->space;
      
      currentThread->space = NULL;
   }

   // finaliza el proceso
   currentThread->Finish();
}


/*
 *  System call interface: SpaceId Exec( char * )
 */
void NachOS_Exec() {		// System call 2
}


/*
 *  System call interface: int Join( SpaceId )
 */
void NachOS_Join() {		// System call 3
}


/*
 *  System call interface: void Create( char * )
 */
void NachOS_Create() {		// System call 4
}


/*
 *  System call interface: OpenFileId Open( char * )
 */
void NachOS_Open() {		// System call 5
   initTable();
   char name[ 128 ];
   readUserString( machine->ReadRegister( 4 ), name, 128 );
 
   int unixFd = open( name, O_RDWR );

   if ( unixFd < 0 ) {
      machine->WriteRegister( 2, -1 );
      AvanzarPC();
      return;
   }
 
   int arch = openFilesTable->Open( unixFd );
   machine->WriteRegister( 2, arch );
   AvanzarPC();


}


/*
 *  System call interface: OpenFileId Write( char *, int, OpenFileId )
 */
void NachOS_Write() {		// System call 6
   int addr = machine->ReadRegister(4); // Direccion virtual del texto

   int tam = machine->ReadRegister(5); // Cantidad de bytes a escribir

   int archivo = machine->ReadRegister(6); // Archivo destino

   int value;

   // Solo maneja salida estándar y salida de error.
   if (archivo == ConsoleOutput || archivo == ConsoleError)
   {
      for (int i = 0; i < tam; i++)
      {
         machine->ReadMem(addr + i, 1, &value);
         printf("%c", (char)value);
      }
   } else if ( openFilesTable != nullptr && openFilesTable->isOpened( archivo ) ) {
      // leer de memoria usuario y escribir en Unix
      char * buf = new char[ tam ];
      for ( int i = 0; i < tam; i++ ) {
         machine->ReadMem( addr + i, 1, &value );
         buf[i] = (char) value;
      }
      ::write( openFilesTable->getUnixHandle( archivo ), buf, tam );

      delete[] buf;
   }
 
   AvanzarPC();

}


/*
 *  System call interface: OpenFileId Read( char *, int, OpenFileId )
 */
void NachOS_Read() {		// System call 7
   int addr = machine->ReadRegister( 4 );
   int tam = machine->ReadRegister( 5 );
   int archivo   = machine->ReadRegister( 6 );
   int n = 0;
 
   if ( archivo == ConsoleInput ) {
      // Entrada estandar: leer carácter por carácter
      for ( int i = 0; i < tam; i++ ) {
         int c = getchar();
         if ( c == EOF ) break;
         machine->WriteMem( addr + i, 1, c );
         n++;
      }
   } else if ( openFilesTable != nullptr && openFilesTable->isOpened( archivo ) ) {
      // Archivo o socket: leer de Unix y escribir en memoria usuario
      char * buf = new char[ tam ];
      n = (int) ::read( openFilesTable->getUnixHandle(archivo), buf, tam );

      if (n < 0) {
         n = 0;
      }
      for ( int i = 0; i < n; i++ )
         machine->WriteMem( addr + i, 1, (int)(unsigned char) buf[ i ] );
      delete[] buf;
   }
 
   machine->WriteRegister( 2, n );
   AvanzarPC();



}


/*
 *  System call interface: void Close( OpenFileId )
 */
void NachOS_Close() {		// System call 8
   int archivo = machine->ReadRegister(4);

   if ( openFilesTable != nullptr && openFilesTable->isOpened(archivo) ) {

      ::close( openFilesTable->getUnixHandle(archivo) );
      openFilesTable->Close(archivo);
   }

   AvanzarPC();

}


/*
 *  System call interface: void Fork( void (*func)() )
 */
void NachOS_Fork() {		// System call 9
}


/*
 *  System call interface: void Yield()
 */
void NachOS_Yield() {		// System call 10
}


/*
 *  System call interface: Sem_t SemCreate( int )
 */
void NachOS_SemCreate() {		// System call 11
}


/*
 *  System call interface: int SemDestroy( Sem_t )
 */
void NachOS_SemDestroy() {		// System call 12
}


/*
 *  System call interface: int SemSignal( Sem_t )
 */
void NachOS_SemSignal() {		// System call 13
}


/*
 *  System call interface: int SemWait( Sem_t )
 */
void NachOS_SemWait() {		// System call 14
}


/*
 *  System call interface: Lock_t LockCreate( int )
 */
void NachOS_LockCreate() {		// System call 15
}


/*
 *  System call interface: int LockDestroy( Lock_t )
 */
void NachOS_LockDestroy() {		// System call 16
}


/*
 *  System call interface: int LockAcquire( Lock_t )
 */
void NachOS_LockAcquire() {		// System call 17
}


/*
 *  System call interface: int LockRelease( Lock_t )
 */
void NachOS_LockRelease() {		// System call 18
}


/*
 *  System call interface: Cond_t LockCreate( int )
 */
void NachOS_CondCreate() {		// System call 19
}


/*
 *  System call interface: int CondDestroy( Cond_t )
 */
void NachOS_CondDestroy() {		// System call 20
}


/*
 *  System call interface: int CondSignal( Cond_t )
 */
void NachOS_CondSignal() {		// System call 21
}


/*
 *  System call interface: int CondWait( Cond_t )
 */
void NachOS_CondWait() {		// System call 22
}


/*
 *  System call interface: int CondBroadcast( Cond_t )
 */
void NachOS_CondBroadcast() {		// System call 23
}


/*
 *  System call interface: Socket_t Socket( int, int )
 */
void NachOS_Socket() {			// System call 30
   initTable();
   int family = machine->ReadRegister( 4 );
   int type   = machine->ReadRegister( 5 );
 
   int af   = ( family == AF_INET_NachOS ) ? AF_INET : AF_INET6;
   int sock = ( type == SOCK_STREAM_NachOS ) ? SOCK_STREAM : SOCK_DGRAM;
 
   int unixFd = socket( af, sock, 0 );
   if ( unixFd < 0 ) {
      machine->WriteRegister( 2, -1 );
      AvanzarPC();
      return;
   }
 
   int fd = openFilesTable->Open( unixFd );
   if ( fd < 0 ) {
      ::close( unixFd );
      machine->WriteRegister( 2, -1 );
      AvanzarPC();
      return;
   }
 
   machine->WriteRegister( 2, fd );
   AvanzarPC();
}




/*
 *  System call interface: Socket_t Connect( char *, int )
 */
void NachOS_Connect() {		// System call 31
   int fd      = machine->ReadRegister( 4 );
   int ipVaddr = machine->ReadRegister( 5 );
   int port    = machine->ReadRegister( 6 );
 
   char ip[ 64 ];
   readUserString( ipVaddr, ip, 64 );
 
   if ( openFilesTable == nullptr || !openFilesTable->isOpened( fd ) ) {
      machine->WriteRegister( 2, -1 );
      AvanzarPC();
      return;
   }
 
   struct sockaddr_in server;
   memset( &server, 0, sizeof( server ) );
   server.sin_family = AF_INET;
   server.sin_port   = htons( (uint16_t) port );
   inet_pton( AF_INET, ip, &server.sin_addr );
 
   int result = connect( openFilesTable->getUnixHandle( fd ),
                         (struct sockaddr *) &server,
                         sizeof( server ) );
 
   machine->WriteRegister( 2, result );
   AvanzarPC();



}


/*
 *  System call interface: int Bind( Socket_t, int )
 */
void NachOS_Bind() {		// System call 32
}


/*
 *  System call interface: int Listen( Socket_t, int )
 */
void NachOS_Listen() {		// System call 33
}


/*
 *  System call interface: int Accept( Socket_t )
 */
void NachOS_Accept() {		// System call 34
}


/*
 *  System call interface: int Shutdown( Socket_t, int )
 */
void NachOS_Shutdown() {	// System call 25
}




//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2) - SC_Base;

    switch ( which ) {

       case SyscallException:
       //AvanzarPC();
          switch ( type ) {
             case SC_Halt:		// System call # 0
                NachOS_Halt();
                break;
             case SC_Exit:		// System call # 1
                NachOS_Exit();
                break;
             case SC_Exec:		// System call # 2
                NachOS_Exec();
                break;
             case SC_Join:		// System call # 3
                NachOS_Join();
                break;

             case SC_Create:		// System call # 4
                NachOS_Create();
                break;
             case SC_Open:		// System call # 5
                NachOS_Open();
                break;
             case SC_Read:		// System call # 6
                NachOS_Read();
                break;
             case SC_Write:		// System call # 7
                NachOS_Write();
                //AvanzarPC();
                break;
             case SC_Close:		// System call # 8
                NachOS_Close();
                break;

             case SC_Fork:		// System call # 9
                NachOS_Fork();
                break;
             case SC_Yield:		// System call # 10
                NachOS_Yield();
                break;

             case SC_SemCreate:         // System call # 11
                NachOS_SemCreate();
                break;
             case SC_SemDestroy:        // System call # 12
                NachOS_SemDestroy();
                break;
             case SC_SemSignal:         // System call # 13
                NachOS_SemSignal();
                break;
             case SC_SemWait:           // System call # 14
                NachOS_SemWait();
                break;

             case SC_LckCreate:         // System call # 15
                NachOS_LockCreate();
                break;
             case SC_LckDestroy:        // System call # 16
                NachOS_LockDestroy();
                break;
             case SC_LckAcquire:         // System call # 17
                NachOS_LockAcquire();
                break;
             case SC_LckRelease:           // System call # 18
                NachOS_LockRelease();
                break;

             case SC_CondCreate:         // System call # 19
                NachOS_CondCreate();
                break;
             case SC_CondDestroy:        // System call # 20
                NachOS_CondDestroy();
                break;
             case SC_CondSignal:         // System call # 21
                NachOS_CondSignal();
                break;
             case SC_CondWait:           // System call # 22
                NachOS_CondWait();
                break;
             case SC_CondBroadcast:           // System call # 23
                NachOS_CondBroadcast();
                break;

             case SC_Socket:	// System call # 30
		NachOS_Socket();
               break;
             case SC_Connect:	// System call # 31
		NachOS_Connect();
               break;
             case SC_Bind:	// System call # 32
		NachOS_Bind();
               break;
             case SC_Listen:	// System call # 33
		NachOS_Listen();
               break;
             case SC_Accept:	// System call # 32
		NachOS_Accept();
               break;
             case SC_Shutdown:	// System call # 33
		NachOS_Shutdown();
               break;

             default:
                printf("NachOS version: %d-%d\n", (SC_Base + SC_NachOS)/10, (SC_Base + SC_NachOS)%10 );
                printf("Unexpected syscall exception %d\n", type );
                ASSERT( false );
                break;
          }
          break;

       case PageFaultException: {
          break;
       }

       case ReadOnlyException:
          printf( "Read Only exception (%d)\n", which );
          ASSERT( false );
          break;

       case BusErrorException:
          printf( "Bus error exception (%d)\n", which );
          ASSERT( false );
          break;

       case AddressErrorException:
          printf( "Address error exception (%d)\n", which );
          ASSERT( false );
          break;

       case OverflowException:
          printf( "Overflow exception (%d)\n", which );
          ASSERT( false );
          break;

       case IllegalInstrException:
          printf( "Ilegal instruction exception (%d)\n", which );
          ASSERT( false );
          break;

       default:
          printf( "Unexpected exception %d\n", which );
          ASSERT( false );
          break;
    }

}
