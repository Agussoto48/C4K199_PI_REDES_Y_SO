#include "syscall.h"

#define HOST "127.0.0.1"
#define PORT 2026
#define RESPONSE_SIZE 4096

char request[256];
char response[RESPONSE_SIZE];


//Declaracion de funcinoes
int length(const char *text);
void append(char *target, const char *extra);
void setText(char *target, const char *value);
void clear(char *target, int size);
int readLine(char *target, int limit);
char *getBody(char *text, int size);
int separateInput(char *input, char *figure, char *part);
void buildRequest(char *figure, char *part);
void buildListRequest();
void sendRequest();

int main()
{
    char option[4];
    char input[72];
    char figure[64];
    char part[4];

    Write("Cliente NachOS\n", 15, ConsoleOutput);
    Write("1. Ver figuras disponibles\n", 27, ConsoleOutput);
    Write("2. Solicitar figura y mitad\n", 27, ConsoleOutput);
    Write("\nSeleccione una opcion: ", 24, ConsoleOutput);

    readLine(option, 4);

    if (option[0] == '1')
    {
        buildListRequest();
        sendRequest();
    }
    else if (option[0] == '2')
    {
        Write("Ingrese figura y mitad: ", 24, ConsoleOutput);
        readLine(input, 72);

        if (!separateInput(input, figure, part))
        {
            Write("Entrada invalida\n", 17, ConsoleOutput);
            Exit(1);
        }

        if (part[0] == '0')
        {
            Write("Primera mitad:\n", 15, ConsoleOutput);
            buildRequest(figure, "1");
            sendRequest();

            Write("\nSegunda mitad:\n", 17, ConsoleOutput);
            buildRequest(figure, "2");
            sendRequest();
        }
        else
        {
            buildRequest(figure, part);
            sendRequest();
        }
    }
    else
    {
        Write("Opcion invalida\n", 16, ConsoleOutput);
        Exit(1);
    }

    Exit(0);
    return 0;
}

//Definiciones de funciones
int length(const char *text)
{
    int i = 0;
    while (text[i] != '\0')
        i++;
    return i;
}

void append(char *target, const char *extra)
{
    int i = length(target);
    int j = 0;

    while (extra[j] != '\0')
    {
        target[i] = extra[j];
        i++;
        j++;
    }

    target[i] = '\0';
}

void setText(char *target, const char *value)
{
    int i = 0;

    while (value[i] != '\0')
    {
        target[i] = value[i];
        i++;
    }

    target[i] = '\0';
}

void clear(char *target, int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        target[i] = '\0';
    }
}

int readLine(char *target, int limit)
{
    int i = 0;
    char c;

    while (i < limit - 1)
    {
        Read(&c, 1, ConsoleInput);

        if (c == '\n' || c == '\r')
        {
            break;
        }

        target[i] = c;
        i++;
    }

    target[i] = '\0';
    return i;
}

char *getBody(char *text, int size)
{
    int i;

    for (i = 0; i < size - 3; i++)
    {
        if (text[i] == '\r' && text[i + 1] == '\n' &&
            text[i + 2] == '\r' && text[i + 3] == '\n')
        {
            return text + i + 4;
        }
    }

    return text;
}

int separateInput(char *input, char *figure, char *part)
{
    int i = 0;
    int j = 0;

    while (input[i] != '\0' && input[i] != ' ')
    {
        figure[j] = input[i];
        i++;
        j++;
    }

    figure[j] = '\0';

    if (input[i] != ' ')
    {
        return 0;
    }

    i++;
    j = 0;

    while (input[i] != '\0')
    {
        part[j] = input[i];
        i++;
        j++;
    }

    part[j] = '\0';

    return j > 0;
}

void buildRequest(char *figure, char *part)
{
    clear(request, 256);

    append(request, "GET /figura/");
    append(request, figure);
    append(request, "/");
    append(request, part);
    append(request, " HTTP/1.0\r\nHost: ");
    append(request, HOST);
    append(request, "\r\n\r\n");
}

void buildListRequest()
{
    clear(request, 256);

    append(request, "GET /list HTTP/1.0\r\nHost: ");
    append(request, HOST);
    append(request, "\r\n\r\n");
}

void sendRequest()
{
    int socketId;
    int readBytes;
    char *body;

    socketId = Socket(AF_INET_NachOS, SOCK_STREAM_NachOS);

    if (socketId < 0)
    {
        Write("Error creando socket\n", 21, ConsoleOutput);
        Exit(1);
    }

    if (Connect(socketId, HOST, PORT) < 0)
    {
        Write("Error conectando al servidor\n", 29, ConsoleOutput);
        Close(socketId);
        Exit(1);
    }

    Write(request, length(request), socketId);

    clear(response, RESPONSE_SIZE);

    readBytes = Read(response, RESPONSE_SIZE - 1, socketId);
    Write("Read retorno: ", 14, ConsoleOutput);
    Write(readBytes, 0, ConsoleError);
    Close(socketId);

    if (readBytes <= 0)
    {
        Write("No se recibio respuesta\n", 24, ConsoleOutput);
        return;
    }

    response[readBytes] = '\0';

    /* Por ahora imprimimos la respuesta completa para validar que el socket funciona */
    Write(response, readBytes, ConsoleOutput);
}