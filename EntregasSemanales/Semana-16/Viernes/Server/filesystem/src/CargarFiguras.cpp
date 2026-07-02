#include <iostream>
#include "SistemaArchivos.h"
#include "Figura.h"

using namespace std;

int main() {
    SistemaArchivos fs;
    Figura figura;
    int resultado = 0;

    cout << "===== CREANDO Y CARGANDO lego.dat =====" << endl;

    resultado = fs.CrearSistema("lego.dat", 300, 3);
    cout << "Cantidad de bloques del sistema: 300" << endl;

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

    // Maureen
    figura.Inicializar(1, "Maureen", 1, 3);
    figura.AgregarPieza(0, "quiz1x1 white", 26);
    figura.AgregarPieza(1, "laptop2x2 gray", 1);
    figura.AgregarPieza(2, "slides1x4 white", 1);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura Maureen mitad 1" : "Error insertando Maureen mitad 1") << endl;

    figura.Inicializar(2, "Maureen", 2, 3);
    figura.AgregarPieza(0, "hdmi1x20 black", 1);
    figura.AgregarPieza(1, "turing_machine1x1 white", 2);
    figura.AgregarPieza(2, "quiz1x1 white", 100);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura Maureen mitad 2" : "Error insertando Maureen mitad 2") << endl;

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

    // Sleyter
    figura.Inicializar(7, "Sleyter", 1, 4);
    figura.AgregarPieza(0, "MacbookPro 2x2 gray", 5);
    figura.AgregarPieza(1, "Iphone18promaxplus 1x1 blue", 10);
    figura.AgregarPieza(1, "Airtag 1x1 white", 10);
    figura.AgregarPieza(1, "Ipad 4x4 green", 3);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura Sleyter mitad 1" : "Error insertando Sleyter mitad 1") << endl;

    figura.Inicializar(8, "Sleyter", 2, 4);
    figura.AgregarPieza(0, "airpods 2x2 white", 2);
    figura.AgregarPieza(1, "applewatch 1x1 black", 2);
    figura.AgregarPieza(2, "Foto de Steve Jobs b&w", 1);
    figura.AgregarPieza(2, "Foto de Tim Cook color", 1);
    resultado = fs.InsertarFigura(figura);
    cout << (resultado == 0 ? "Insertada figura Sleyter mitad 2" : "Error insertando Sleyter mitad 2") << endl; 


    // este pedazo esta basado en lo que hizo brandon, pero no es lo mismo
    //furro
    const int CANT_PIEZAS_furro = 200;

    const char* nombres[] = {
        "brick1x1 green",
        "brick1x2 red",
        "brick1x2 pink",
        "plate1x3 black",
        "tile2x1 yellow",
        "brick2x3 gray",
        "slope1x2 red",
        "brick1x1 yellow"
    };

    figura.Inicializar(9, "furro", 1, CANT_PIEZAS_furro);

    for (int i = 0; i < CANT_PIEZAS_furro; i++) {
        figura.AgregarPieza(i, nombres[i % 8], (i % 5) + 1);
    }

    resultado = fs.InsertarFigura(figura);

    cout << (resultado == 0 ? 
        "Insertada figura furro mitad 1" : 
        "Error insertando furro mitad 1") << endl;
    
    
    figura.Inicializar(10, "furro", 2, CANT_PIEZAS_furro);

    for (int i = 0; i < CANT_PIEZAS_furro; i++) {
        figura.AgregarPieza(i, nombres[(i + 3) % 8], (i % 4) + 1);
    }

    resultado = fs.InsertarFigura(figura);

    cout << (resultado == 0 ? 
        "Insertada figura furro mitad 2" : 
        "Error insertando furro mitad 2") << endl;
    

    cout << endl;
    cout << "===== FIGURAS CARGADAS EN lego.dat =====" << endl;
    fs.ListarFiguras();

    fs.CerrarSistema();

    cout << endl;
    cout << "lego.dat fue cargado correctamente." << endl;

    return 0;
}
