#include "Cliente.hpp"

std::string Cliente::buildRequest(const std::string &figure, const std::string &part)
{
    return "GET /figura/" + figure + "/" + part +
           " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
}

std::string Cliente::buildListRequest()
{
    return "GET /list HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
}

bool Cliente::separateInput(const std::string &input, std::string &figure, std::string &part)
{
    size_t space = input.find(' ');

    if (space == std::string::npos)
    {
        return false;
    }

    figure = input.substr(0, space);
    part = input.substr(space + 1);

    return !figure.empty() && !part.empty();
}

void Cliente::sendRequest(const std::string &request)
{
    int socketId = socket(AF_INET, SOCK_STREAM, 0);

    if (socketId < 0)
    {
        std::cout << "Error creando socket\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0)
    {
        std::cout << "Direccion IP invalida\n";
        close(socketId);
        return;
    }

    if (connect(socketId, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cout << "Error conectando al servidor\n";
        close(socketId);
        return;
    }

    send(socketId, request.c_str(), request.size(), 0);

    char response[4096];
    int readBytes = recv(socketId, response, sizeof(response) - 1, 0);

    close(socketId);

    if (readBytes <= 0)
    {
        std::cout << "No se recibio respuesta\n";
        return;
    }

    response[readBytes] = '\0';
    std::cout << response << std::endl;
}

Cliente::Client(const std::string &host, int port)
{
    this->host = host;
    this->port = port;
}

void Cliente::run()
{
    std::string option;

    std::cout << "1. Ver figuras disponibles\n";
    std::cout << "2. Solicitar figura y mitad\n";
    std::cout << "\nSeleccione una opcion: ";

    std::getline(std::cin, option);

    if (option == "1")
    {
        sendRequest(buildListRequest());
    }
    else if (option == "2")
    {
        std::string input;
        std::string figure;
        std::string part;

        std::cout << "Ingrese figura y mitad: ";
        std::getline(std::cin, input);

        if (!separateInput(input, figure, part))
        {
            std::cout << "Entrada invalida\n";
            return;
        }

        if (part == "0")
        {
            std::cout << "Primera mitad:\n";
            sendRequest(buildRequest(figure, "1"));

            std::cout << "\nSegunda mitad:\n";
            sendRequest(buildRequest(figure, "2"));
        }
        else
        {
            sendRequest(buildRequest(figure, part));
        }
    }
    else
    {
        std::cout << "Opcion invalida\n";
    }
}