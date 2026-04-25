# Sistema cliente-intermediario-servidor

- Enrique Ramírez (C16272)
- PI redes oper
- I 2026, grupo 02

## Descripción

Se adapta el cliente de la primera entrega grupal del proyecto, de tal manera que se comunique con el servidor por medio de un intermediario.

El intermediario recibe solicitudes HTTP del ciente y las transforma al formato del protocolo propuesto, de tal forma que el servidor de piezas pueda recibir y entender la solicitud. Recibe una respuesta y se la manda como HTTP al cliente de vuelta.

El servidor contiene los datos de las figuras dentro del filesystem, que sigue una estructura de tipo linked allocation, como se propuso en entregas anteriores.

Las respuestas que da el servidor son las listas de piezas para las figuras que almacena.

El servidor usa hilos, de tal manera que pueda manejar más de una solicitud a la vez.

## Instrucciones de uso

- `make`
- Para llenar de datos el servidor: `./bin/cargar_figuras.out`. Esto solo hay que hacerlo una vez.
- en una terminal: `./bin/servidor.out` para inicial el servidor
- en otra terminal: `./bin/intermediario.out`
- en una tercera terminal: `./bin/cliente.out <animal> <mitad>`

## Ejemplo de uso

```bash
./bin/cliente.out pez 0
```

Salida:

```bash
Figura: pez
Primera mitad:
t = s, IPv6 = 0, dominio = 2, tipoSocket = 1
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 52
Connection: close

figura=pez
mitad=1
brick1x1=4
brick2x2=2
plate1x4=1
Segunda mitad:
t = s, IPv6 = 0, dominio = 2, tipoSocket = 1
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 51
Connection: close

figura=pez
mitad=2
brick1x2=5
plate2x2=2
tile1x1=1
```

## Notas

Para el trabajo en clase 7, se solicitaba en uno de los puntos "Entregar un documento con las elecciones realizadas por el equipo de trabajo". Por ello, se intuye que la implementación del sistema cliente-intermediario-servidor aquí presentes, corresponde a un esfuerzo de naturaleza grupal de parte de los cuatro integrantes del grupo. En ese sentido, este código es una adaptación del presentado por los otros compañeros de equipo. La implementación se hizo basándonos en la propuesta de protocolo acordada y el sistema de archivos discutido con los compañeros durante clases.
