# Modelo de hilos por conexión para reconocer las redes pública y privada del intermediario

- PI redes oper
- I 2026

## Descripción

La idea es que el servidor intermediario tenga una interfaz de red privada que es la isla del equipo y la interfaz de red pública. Entonces, cuando llega un cliente, se lanza un hilo que maneja esa conexión desde que inicia hasta que termina.

El hilo principal se encarga de aceptar conexiones y lanzar los hilos.

El cliente se conecta al intermediario y el hilo principal. Luego se delega la conexión a un hilo nuevo y se espera al siguiente cliente.

Ese hilo nuevo lee la solicitud del cliente, que viene por la interfaz privada. Despues el hilo abre una conexión hacia el servidor externo, que es el de nachos usando la interfaz pública, le reenvía la solicitud y espera la respuesta. Al recibir respiesta, se la manda al cliente y se cierra el hilo.

## Interfaces de red

- `ip link show` sirve para ver las interfaces de red
- `ip addr show` muestra cuales ip tienen asignadas
- `ip link show | grep "state UP"` muestra las que estan fisicamente conectadas
- 