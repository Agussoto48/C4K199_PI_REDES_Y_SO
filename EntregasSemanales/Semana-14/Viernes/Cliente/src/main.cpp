#include "Cliente.hpp"

int main() {
    Cliente client("127.0.0.1", 8080);
    client.run();

    return 0;
}