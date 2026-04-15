// =============================================================
// Paquete.cpp
// Implementacion de las funciones para trabajar con paquetes.
// =============================================================

#include "Paquete.hpp"
#include <iostream>
#include <string>

using namespace std;

// =============================================================
// Conversiones a texto
// =============================================================

string prioridadATexto(Prioridad p) {
    if (p == URGENTE) return "URGENTE";
    return "Normal";
}

// =============================================================
// Visualizacion
// =============================================================

// Muestra la informacion del paquete.
// Si tiene dron asignado lo indica; si no, muestra que esta en cola.
void mostrarPaquete(const Paquete& p) {
    cout << "  Paquete [" << p.id << "]"
        << "  Peso: " << p.peso << " kg"
        << "  Destino: " << p.destino
        << "  Prioridad: " << prioridadATexto(p.prioridad);

    if (!p.idDronAsignado.empty()) {
        cout << "  --> Asignado a dron: " << p.idDronAsignado;
    }
    else {
        cout << "  --> EN COLA DE ESPERA";
    }
    cout << "\n";
}
