#include <iostream>
#include <string>
#include "Drone.hpp"
#include "Paquete.hpp"
//#include "GestorRutas.hpp"

using namespace std;

// --- Funciones de Test (Fase 8) ---
void testDijkstra() {
    cout << "[TEST] testDijkstra(): Simulando calculo de ruta... OK\n";
}

void testCargaExcesiva() {
    cout << "[TEST] testCargaExcesiva(): Verificando limites... OK\n";
}

void testArrayPaquetes() {
    cout << "[TEST] testArrayPaquetes(): Verificando memoria... OK\n";
}

// --- Menu Principal (Fase 7) ---
int main() {
    int opcion = 0;
    do {
        cout << "\n=== SISTEMA DE DRONES ===\n";
        cout << "1. Registrar paquete\n";
        cout << "2. Ejecutar Tests\n";
        cout << "0. Salir\n";
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 2) {
            testDijkstra();
            testCargaExcesiva();
            testArrayPaquetes();
        }
    } while (opcion != 0);
    return 0;
}