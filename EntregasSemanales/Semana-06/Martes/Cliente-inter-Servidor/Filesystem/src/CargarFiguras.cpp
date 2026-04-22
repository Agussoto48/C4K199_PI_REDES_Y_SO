#include <iostream>
#include "SistemaArchivos.h"
#include "Figura.h"

using namespace std;

int main() {
    SistemaArchivos fs;
    Figura figura1;
    Figura figura2;
    Figura figura3;
    Figura figura4;
    int resultado = 0;

    cout << "===== CREANDO Y CARGANDO lego.dat =====" << endl;

    resultado = fs.CrearSistema("data/lego.dat", 100, 2);

    if ( resultado != 0 ) {
        cout << "Error al crear lego.dat" << endl;
        return 1;
    }

    resultado = fs.AbrirSistema("data/lego.dat");

    if ( resultado != 0 ) {
        cout << "Error al abrir lego.dat" << endl;
        return 1;
    }

    cout << "Sistema creado y abierto correctamente." << endl << endl;

    figura1.Inicializar(1, "pez", 1, 3);
    figura1.AgregarPieza(0, "brick1x1", 4);
    figura1.AgregarPieza(1, "brick2x2", 2);
    figura1.AgregarPieza(2, "plate1x4", 1);

    resultado = fs.InsertarFigura(figura1);

    if ( resultado == 0 ) {
        cout << "Insertada figura pez mitad 1" << endl;
    } else {
        cout << "Error insertando pez mitad 1" << endl;
    }

    figura2.Inicializar(2, "pez", 2, 3);
    figura2.AgregarPieza(0, "brick1x2", 5);
    figura2.AgregarPieza(1, "plate2x2", 2);
    figura2.AgregarPieza(2, "tile1x1", 1);

    resultado = fs.InsertarFigura(figura2);

    if ( resultado == 0 ) {
        cout << "Insertada figura pez mitad 2" << endl;
    } else {
        cout << "Error insertando pez mitad 2" << endl;
    }

    figura3.Inicializar(3, "caballo", 1, 4);
    figura3.AgregarPieza(0, "brick2x4", 3);
    figura3.AgregarPieza(1, "plate1x2", 2);
    figura3.AgregarPieza(2, "slope2x2", 1);
    figura3.AgregarPieza(3, "tile2x2", 2);

    resultado = fs.InsertarFigura(figura3);

    if ( resultado == 0 ) {
        cout << "Insertada figura caballo mitad 1" << endl;
    } else {
        cout << "Error insertando caballo mitad 1" << endl;
    }

    figura4.Inicializar(4, "dragon", 1, 10);
    figura4.AgregarPieza(0, "brick1x1", 1);
    figura4.AgregarPieza(1, "brick1x2", 2);
    figura4.AgregarPieza(2, "brick1x3", 3);
    figura4.AgregarPieza(3, "brick1x4", 4);
    figura4.AgregarPieza(4, "brick2x2", 5);
    figura4.AgregarPieza(5, "brick2x3", 6);
    figura4.AgregarPieza(6, "brick2x4", 7);
    figura4.AgregarPieza(7, "plate1x1", 8);
    figura4.AgregarPieza(8, "plate1x2", 9);
    figura4.AgregarPieza(9, "plate1x4", 10);

    resultado = fs.InsertarFigura(figura4);

    if ( resultado == 0 ) {
        cout << "Insertada figura dragon mitad 1" << endl;
    } else {
        cout << "Error insertando dragon mitad 1" << endl;
    }

    cout << endl;
    cout << "===== FIGURAS CARGADAS EN lego.dat =====" << endl;
    fs.ListarFiguras();

    fs.CerrarSistema();

    cout << endl;
    cout << "lego.dat fue cargado correctamente." << endl;

    return 0;
}