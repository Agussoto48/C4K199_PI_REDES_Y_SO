# Cómo reiniciar un 2960 a especificaciones de fábrica

- Hay que tener instalado `minicom`.
- hay que identificar el puerto donde tenemos el switch: `ls /dev/tty*`. Es `/dev/ttyUSB0`.
- Sabiendo ya que ese es el puerto, hay que abrir `minicom` y configurar con ese puerto: `sudo minicom -s`
- Nos va a salir una pantalla azul. Hay que seleccionar "Serial Port Setup". El serial device debe ser `/dev/ttyUSB0`. el `Bps/Par/Bits` debe ser 9600, que es el baud rate. Hardware Flow Control y Software Flow Control deben estar en "No".
- Luego de haber puesto todo eso, le damos a `esc` para salir al menú principal
- Le damos a "Save setup as DFL"
- Le damos "exit"
- Hay que reconectar el switch y tener presionado el  boton de mode
- 
Habiendo seguido esas instrucciones, va a aparecernos la cli del switch.

Hay que meter:

- `flash_init`
- `del flash:config.text`
- `del flash:vlan.dat`
- `boot`

Con esto, va a haberse reiniciado el switch

## Setup basico del switch

Despues de resetearlo, nos va a mostrar un dialog de configuracion inicial.

- El host name puede ser lo que queramos. Como por ejemplo "Industria".
- El enable secret password es `12345678`
- el enable password es `password`
- El virtual terminal password es `12345678`
- SNMP network manager queda en "NO"
Enter interface name used to connect to the                                     
management network from the above interface summary: vlan1

Se sigue hasta completar todo y con eso queda funcional el switch.

## Configurar el switch

Hay que hacer enable y modo configuración:

```bash
enable
configure terminal
```

Hay que ponerle la vlan 230, que es la de mi isla:

```bash
vlan 230
name isla3
exit
```

Hay que configurar los puertos access:

```bash
interface range fa0/1 - 2
switchport mode access
switchport access vlan 230
exit
```

Esto es para comunicarse con el 3560 (L3):

```bash
interface fa0/3
switchport mode trunk
exit
```

Ip de administracion:

```bash
interface vlan 230
ip address 172.16.123.50 255.255.255.240
no shutdown
exit
```

Lueog se configura dhcp:

```bash

ip dhcp excluded-address 172.16.123.49
ip dhcp excluded-address 172.16.123.50
ip dhcp pool isla3
network 172.16.123.48 255.255.255.240
default-router 172.16.123.49
exit

```

Se hace `end` y luego se guarda la configuracion con `write memory`

Para ver esa configuración, se hace `show running-config`

## Conectar dos PC al switch

Las conecto en los primeros dos puertos. Luego, en cada computadora, hago:

- `ip link show` esto me da que el nombre de la conexion es `enp0s31f6`.

- Luego hago `sudo nmcli device connect enp0s31f6`
- Luego, con `ip addr show enp0s31f6` vamos a poder verificar que se asignó correctamente una ip a la computadora. En estas dos compus, se asignaron las IP `172.16.123.53` y la `172.16.123.54`.

En el switch, aparece como:

```bash
Industria#show interfaces status                                                    
                                                                                    
Port      Name               Status       Vlan       Duplex  Speed Type             
Fa0/1                        connected    230        a-full  a-100 10/100BaseTX     
Fa0/2                        connected    230        a-full  a-100 10/100BaseTX 
```

## Configurar otra isla

Hay que hacer todo esto mismo, pero con otro 2960.
Al otro switch le puse de nombre jaguar y vlan220

```bash
interface vlan 220
ip address 172.16.123.34 255.255.255.240
no shutdown
exit
```

DHCP:

```bash
ip dhcp excluded-address 172.16.123.33
ip dhcp excluded-address 172.16.123.34
ip dhcp pool isla2
network 172.16.123.32 255.255.255.240
default-router 172.16.123.33
exit
```

## Configurar isla 6 (tercer 2960)

Mismo proceso que las otras islas. VLAN 260:

```bash
vlan 260
name isla6
exit
```

Puertos access:

```bash
interface range fa0/1 - 2
switchport mode access
switchport access vlan 260
exit
```

Trunk hacia el 3560:

```bash
interface fa0/3
switchport mode trunk
exit
```

IP de administración:

```bash
interface vlan 260
ip address 172.16.123.98 255.255.255.240
no shutdown
exit
```

DHCP:

```bash
ip dhcp excluded-address 172.16.123.97
ip dhcp excluded-address 172.16.123.98
ip dhcp pool isla6
network 172.16.123.96 255.255.255.240
default-router 172.16.123.97
exit
```

```bash
end
write memory
```

## Configurar el 3560

Conectarse por minicom

```bash
enable
configure terminal
```

Crear las vlan:

```bash
vlan 220
name isla2
exit
vlan 230
name isla3
exit
vlan 260
name isla6
exit
```
Configurar puertos trunk:

```bash
interface fa0/1
switchport trunk encapsulation dot1q
switchport mode trunk
exit
interface fa0/2
switchport trunk encapsulation dot1q
switchport mode trunk
exit
interface fa0/3
switchport trunk encapsulation dot1q
switchport mode trunk
exit
```

IPs:

```bash
interface vlan 220
ip address 172.16.123.33 255.255.255.240
no shutdown
exit
interface vlan 230
ip address 172.16.123.49 255.255.255.240
no shutdown
exit
interface vlan 260
ip address 172.16.123.97 255.255.255.240
no shutdown
exit
```

habilitar routing entre VLANs:

```bash
ip routing
```

guardar:

```bash
end
write memory
```

## Conectar los dos L2 al L3

El primer l2 tiene una conexion que sale del puerto 3 y llega al puerto 1 del L3.

El segundo l2 tiene una conexion que sale del puerto 3 y llega al puerto 2 del L3.

## Comprobar conectividad

Es posible hacer ping entre computadoras de la misma isla y entre computadoras de islas distintas.
