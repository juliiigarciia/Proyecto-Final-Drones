#include <iostream>
#include <string>
#include "Drone.hpp"
#include "Paquete.hpp"
//#include "GestorRutas.hpp"

using namespace std;

int main() {
    int opcion = 0;
    do {
        cout << "\n=== SISTEMA DE DRONES ===\n";
        cout << "1. Registrar paquete\n";
        cout << "2. Ejecutar Tests\n";
        cout << "0. Salir\n";
        cout << "Opcion: ";
        cin >> opcion;
    } while (opcion != 0);
    return 0;
}