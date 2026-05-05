#include <iostream>
#include "SistemaArchivos.h"
#include "Figura.h"

using namespace std;

int main() {
    SistemaArchivos fs;
    Figura figura;
    int resultado = 0;

    cout << "===== CREANDO Y CARGANDO lego.dat =====" << endl;

    resultado = fs.CrearSistema("lego.dat", 100, 3);

    if ( resultado != 0 ) {
        cout << "Error al crear lego.dat" << endl;
        return 1;
    }

    resultado = fs.AbrirSistema("lego.dat");

    if ( resultado != 0 ) {
        cout << "Error al abrir lego.dat" << endl;
        return 1;
    }

    cout << "Sistema creado y abierto correctamente." << endl << endl;

    // pez
    figura.Inicializar(1, "pez", 1, 3);
    figura.AgregarPieza(0, "brick1x1 blue", 4);
    figura.AgregarPieza(1, "brick2x2 blue", 2);
    figura.AgregarPieza(2, "plate1x4 yellow", 1);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura pez mitad 1" : "Error insertando pez mitad 1") << endl;

    figura.Inicializar(2, "pez", 2, 3);
    figura.AgregarPieza(0, "brick1x2 blue", 5);
    figura.AgregarPieza(1, "plate2x2 blue", 2);
    figura.AgregarPieza(2, "tile1x1 gray", 1);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura pez mitad 2" : "Error insertando pez mitad 2") << endl;

    // caballo
    figura.Inicializar(3, "caballo", 1, 4);
    figura.AgregarPieza(0, "brick2x3 yellow", 1);
    figura.AgregarPieza(1, "brick1x1 eye yellow", 2);
    figura.AgregarPieza(2, "brick2x1 yellow", 2);
    figura.AgregarPieza(3, "brick2x2 white", 1);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura caballo mitad 1" : "Error insertando caballo mitad 1") << endl;

    figura.Inicializar(4, "caballo", 2, 4);
    figura.AgregarPieza(0, "brick2x2 yellow", 5);
    figura.AgregarPieza(1, "brick2x4 white", 4);
    figura.AgregarPieza(2, "brick2x4 yellow", 2);
    figura.AgregarPieza(3, "brick2x6 yellow", 1);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura caballo mitad 2" : "Error insertando caballo mitad 2") << endl;

    // tortuga
    figura.Inicializar(5, "tortuga", 1, 4);
    figura.AgregarPieza(0, "plate2x4 black", 2);
    figura.AgregarPieza(1, "brick2x2 yellow", 1);
    figura.AgregarPieza(2, "brick1x2 yellow", 2);
    figura.AgregarPieza(3, "brick1x2 slope yellow", 2);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura tortuga mitad 1" : "Error insertando tortuga mitad 1") << endl;

    figura.Inicializar(6, "tortuga", 2, 3);
    figura.AgregarPieza(0, "brick2x2 slope inv yellow", 1);
    figura.AgregarPieza(1, "brick2x2 trans blue", 2);
    figura.AgregarPieza(2, "brick1x1 round eye yellow", 2);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura tortuga mitad 2" : "Error insertando tortuga mitad 2") << endl;

    // serpiente
    figura.Inicializar(7, "serpiente", 1, 2);
    figura.AgregarPieza(0, "brick2x6 green", 1);
    figura.AgregarPieza(1, "brick2x2 black", 1);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura serpiente mitad 1" : "Error insertando serpiente mitad 1") << endl;

    figura.Inicializar(8, "serpiente", 2, 3);
    figura.AgregarPieza(0, "brick2x2 green", 2);
    figura.AgregarPieza(1, "brick1x1 black", 2);
    figura.AgregarPieza(2, "brick1x1 green", 2);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura serpiente mitad 2" : "Error insertando serpiente mitad 2") << endl;

    cout << endl;
    cout << "===== FIGURAS CARGADAS EN lego.dat =====" << endl;
    fs.ListarFiguras();

    fs.CerrarSistema();

    cout << endl;
    cout << "lego.dat fue cargado correctamente." << endl;

    return 0;
}
