# README - Servidor SSL TCP IPv4 / IPv6

## Descripción general

Se implementó una clase `SSLSocket` que hereda de la clase `Socket`.\
Esta clase agrega soporte de comunicación segura utilizando OpenSSL
sobre conexiones TCP.

La clase `SSLSocket` permite:

-   Crear conexiones SSL tanto cliente como servidor\
-   Realizar handshake mediante `SSL_connect` y `SSL_accept`\
-   Enviar y recibir datos cifrados con `SSL_write` y `SSL_read`\
-   Mostrar certificados del peer\
-   Consultar el tipo de cifrado utilizado

------------------------------------------------------------------------

## Servidores implementados

### Servidor con hilos (PThreads)

-   Se utiliza `pthread_create()`\
-   Cada cliente es atendido en un hilo independiente\
-   Permite múltiples conexiones concurrentes\
-   Mejor rendimiento en aplicaciones ligeras

------------------------------------------------------------------------

### Servidor con procesos (fork)

-   Se utiliza `fork()`\
-   Cada cliente es atendido por un proceso hijo\
-   Mayor aislamiento entre conexiones\
-   Más pesado que hilos pero más seguro

------------------------------------------------------------------------

## Compilación

En la terminal:

    make clean
    make

------------------------------------------------------------------------

## Ejecución del programa

### Paso 1: Ejecutar el servidor

En una terminal:

#### Servidor con hilos

    ./ThreadSSLServer.out 4321

#### Servidor con procesos

    ./ForkSSLServer.out 4321

------------------------------------------------------------------------

### Paso 2: Ejecutar el cliente

En otra terminal:

#### IPv4

    ./SSLClient.out 127.0.0.1 4321

#### IPv6

    ./SSLClient.out ::1 4321

------------------------------------------------------------------------

### Paso 3: Ingresar credenciales

    UserName: piro
    Password: ci0123

------------------------------------------------------------------------

## Flujo de ejecución

    Terminal 1 -> Servidor (esperando conexiones)
    Terminal 2 -> Cliente 1
    Terminal 3 -> Cliente 2
    Terminal 4 -> Cliente 3

Cada cliente será atendido de forma concurrente (hilos o procesos).

------------------------------------------------------------------------

## Notas importantes

-   El servidor debe ejecutarse primero\
-   No cerrar el servidor mientras se prueban clientes\
-   No mezclar IPv4 con IPv6\
-   Los certificados deben estar en la carpeta `certs/`\
-   Se pueden abrir múltiples clientes simultáneamente

------------------------------------------------------------------------

## Verificación

Para ver procesos (fork):

    ps aux | grep ForkSSLServer

Para ver hilos:

    ps -eLf | grep ThreadSSLServer

------------------------------------------------------------------------

## Conclusión

El proyecto implementa:

-   Comunicación segura con SSL\
-   Soporte para IPv4 e IPv6\
-   Concurrencia mediante hilos y procesos\
-   Cliente y servidor funcionales
