#include <iostream>
#include "SistemaArchivos.h"

using namespace std;

int main() {
    SistemaArchivos fs;

    if ( fs.AbrirSistema("lego.dat") != 0 ) {
        cout << "Error al abrir lego.dat" << endl;
        return 1;
    }

    cout << "TEXTO LISTADO" << endl;
    cout << fs.ConstruirTextoListadoFiguras() << endl;

    cout << "TEXTO FIGURA pez mitad 1" << endl;
    cout << fs.ConstruirTextoFigura("pez", 1) << endl;

    cout << "TEXTO FIGURA dragon mitad 1" << endl;
    cout << fs.ConstruirTextoFigura("dragon", 1) << endl;

    fs.CerrarSistema();

    return 0;
}