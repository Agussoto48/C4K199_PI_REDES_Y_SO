#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class Cliente {
private:
    std::string host;
    int port;

    std::string buildRequest(const std::string& figure, const std::string& part);
    std::string buildListRequest();
    bool separateInput(const std::string& input, std::string& figure, std::string& part);
    void sendRequest(const std::string& request);

public:
    Cliente(const std::string& host, int port);
    void run();
};
