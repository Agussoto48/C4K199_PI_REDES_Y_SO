/**
 * Cliente de NachOS de la entrega grupal. el resto de nachos es de Enrique
 * 
 *   nachos -x ../test/cliente
 *
 */

#include "syscall.h"

#define SERVIDOR "172.16.123.51"
#define PUERTO   2026
#define BUFSIZE  4096 // agrandar si resulta necesario

char req[256]; // buffer para el request que se le hace al server
char buf[BUFSIZE]; // buffer para la respuesta completa de http


//funciones de manejo de strings

// esto es como strlen
static int tamString(const char *string) {
    int n = 0;
    while (string[n]) n++;
    return n;
}

// como strcpy
static void copiarstring(char *destino, const char *origen) {
    int i = 0;
    while ((destino[i] = origen[i]) != '\0') {
        i++;
    }
}

// como strcat
static void concatenar(char *destino, const char *src) {
    int i = 0, j = 0;
    while (destino[i]) {
        i++;
    }
    while ((destino[i++] = src[j++]) != '\0');
}

// guarda lo leido en bufer y retorna la cant. de caracteres
static int readline(char *buf, int maxlen) {
    int i = 0;
    char c;
    while (i < maxlen - 1) {
        Read(&c, 1, 0);
        if (c == '\n' || c == '\r') {
            break;
        }
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

// devuelve el cuerpo http
static char *find_body(char *resp, int n) {
    int i;
    for (i = 0; i < n - 3; i++) {
        if (resp[i]=='\r' && resp[i+1]=='\n'
            && resp[i+2]=='\r' && resp[i+3]=='\n') {
            return resp + i + 4;
        }
    }
    return resp;
}

// Esto es para dividir la figura y la mitad en dos tokens cuando el cliente da input en la terminal
static int split_space(const char *src, char *first, char *second) {
    int i = 0, j = 0;
    while (src[i] && src[i] != ' ') {
        first[j++] = src[i++];
    }

    first[j] = '\0';

    if (src[i] == ' ') {
        i++;
    }

    j = 0;

    while (src[i]) {
        second[j++] = src[i++];
    }

    second[j] = '\0';

    return j > 0;
}

// se hace conexión, se manda solicitud y se muestra la rspuesta
static void hacer_request(char *request) {
    int id, n, i;
    char *body;

    id = Socket(AF_INET_NachOS, SOCK_STREAM_NachOS);

    if (Connect(id, SERVIDOR, PUERTO) < 0) {
        Write("Error: no se pudo conectar al servidor\n", 39, 1);
        Exit(1);
    }

    Write(request, tamString(request), id);

    // limpiar el bufer (para cuando se piden las dos mitades de un solo)
    for (i = 0; i < BUFSIZE; i++) { 
        buf[i] = '\0';
    }

    n = Read(buf, BUFSIZE - 1, id);
    Close(id);

    // encontrar la info que se quiere imprimir
    body = find_body(buf, n);
    // imprimir lo que quiere el cliente
    Write(body, tamString(body), 1);
}

int main() {
    // estos son los bufers que guardan la entrada de usuario
    char opcion[4];
    char figura[64];
    char mitad[4];
    char entrada[72];

    Write("Servidor de legos\n", 18, 1);
    Write("1: Lista de figuras\n", 20, 1);
    Write("2: Pedir pieza (ej: pez 1)\n", 27, 1);
    Write("Opcion: ", 8, 1);
    readline(opcion, 4);

    // manejo de i/o. La salida la maneja hacer_request()
    if (opcion[0] == '1') {

        copiarstring(req, "GET /list HTTP/1.0\r\nHost: ");
        concatenar(req, SERVIDOR);
        concatenar(req, "\r\n\r\n");
        hacer_request(req);

    } else if (opcion[0] == '2') {

        Write("Figura y mitad: ", 16, 1);
        readline(entrada, 72);

        if (!split_space(entrada, figura, mitad)) {
            Write("Formato invalido\n", 17, 1);
            Exit(1);
        }

        if (mitad[0] == '0') {

            Write("Primera mitad:\n", 15, 1);
            copiarstring(req, "GET /figura/");
            concatenar(req, figura);
            concatenar(req, "/1 HTTP/1.0\r\nHost: ");
            concatenar(req, SERVIDOR);
            concatenar(req, "\r\n\r\n");
            hacer_request(req);

            Write("\nSegunda Mitad:\n", 16, 1);
            copiarstring(req, "GET /figura/");
            concatenar(req, figura);
            concatenar(req, "/2 HTTP/1.0\r\nHost: ");
            concatenar(req, SERVIDOR);
            concatenar(req, "\r\n\r\n");
            hacer_request(req);

        } else {

            copiarstring(req, "GET /figura/");
            concatenar(req, figura);
            concatenar(req, "/");
            concatenar(req, mitad);
            concatenar(req, " HTTP/1.0\r\nHost: ");
            concatenar(req, SERVIDOR);
            concatenar(req, "\r\n\r\n");
            hacer_request(req);

        }

    } else {
        Write("Opcion invalida\n", 16, 1);
        Exit(1);
    }

    Exit(0);
    return 0;
}
