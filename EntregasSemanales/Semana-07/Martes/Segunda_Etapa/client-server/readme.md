# Sistema cliente-servidor

- Enrique Ramírez (C16272)
- PI redes oper
- I 2026, grupo 02

## Descripción

Se adapta el cliente de la primera entrega grupal del proyecto, de tal manera que se comunique con un servidor de piezas propio por medio de HTTP.

El servidor contiene los datos de las figuras dentro del filesystem, que sigue una estructura de tipo linked allocation, como se propuso en entregas anteriores.

Las respuestas que da el servidor son las listas de piezas para las figuras que almacena.

El servidor usa hilos, de tal manera que pueda manejar más de una solicitud a la vez.

El servidor tiene un log que guarda registro de sus interacciones con los clientes.

## Instrucciones de uso

- `make`
- Para llenar de datos el servidor: `./bin/cargar_figuras.out`. Esto solo hay que hacerlo una vez.
- en un dispositivo: `./bin/servidor.out` para correr el servidor
- en otro dispositivo: `./bin/cliente.out <animal> <mitad>`, o `./bin/cliente.out --list`
- Se puede interactuar con el servidor por medio de `curl`. Por ejemplo: `curl http://192.168.0.105:2026/figura/pez/1` (servidor personal de Enrique).
- Se puede interactuar con el servidor por medio de `wget`, por ejemplo `wget -qO- http://192.168.0.105:2026/list`
- Se puede acceder desde el navegador, tanto en ordenador como en celular, por ejemplo `http://192.168.0.105:2026/list`

