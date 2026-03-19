# Semana 1 - PI Redes y Sistemas Operativos

## Descripción

En esta semana se desarrollan varios programas enfocados en la comunicación mediante sockets utilizando protocolos de envío como TCP y UDP en IPv4 e IPv6, así como una simulación de como debería funcionar el modelo Cliente-Intermediario-Servidor.

Los componentes principales del proyecto son:

- **IPv4**: Implementación de comunicación mediante sockets TCP en IPv4.
- **IPv6**: Implementación de comunicación mediante sockets TCP en IPv6.
- **Simulación**: Implementación de un protocolo simulado.
- **Cliente/Servidor IPv4**: Intercambio de datagramas sin conexión utilizando UDP.
- **Cliente/Servidor IPv6**: Intercambio de datagramas sin conexión utilizando UDP.


## Uso del Makefile

El `Makefile` permite compilar, ejecutar y limpiar el proyecto de manera automatizada.

### Compilación

- `make`  
  Compila todos los ejecutables del proyecto.

- `make build_IPv4`  
  Compila el programa principal de IPv4.

- `make build_IPv6`  
  Compila el programa principal de IPv6.

- `make build_Sim`  
  Compila la simulación.

- `make build_Cliente_IPV4`  
  Compila el cliente UDP en IPv4.

- `make build_Server_IPV4`  
  Compila el servidor UDP en IPv4.

- `make build_Cliente_IPV6`  
  Compila el cliente UDP en IPv6.

- `make build_Server_IPV6`  
  Compila el servidor UDP en IPv6.

### Ejecución

- `make run_IPv4`  
  Ejecuta el programa IPv4.

- `make run_IPv6`  
  Ejecuta el programa IPv6.

- `make run_Sim`  
  Ejecuta la simulación.

- `make run_Cliente_IPV4`  
  Ejecuta el cliente IPv4.

- `make run_Server_IPV4`  
  Ejecuta el servidor IPv4.

- `make run_Cliente_IPV6`  
  Ejecuta el cliente IPv6.

- `make run_Server_IPV6`  
  Ejecuta el servidor IPv6.


## Ejecución Cliente-Servidor

Para probar la comunicación UDP:

1. Ejecutar el servidor
2. En otra terminal ejecutar el cliente

Esto aplica para ambos IPv4 como IPv6

## Limpieza
- `make clean`  
Elimina el directorio `bin` junto con todos los ejecutables generados.


## Autor
Agustín Soto | C4K199