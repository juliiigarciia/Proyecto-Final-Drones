// =============================================================
// Paquete.cpp
// Implementacion de la clase Paquete.
// =============================================================
#include "Paquete.hpp"
#include <iostream>
using namespace std;

// Constructor: inicializa todos los campos con lista de inicializacion
Paquete::Paquete(const string& id, double peso,
                 const string& destino, Prioridad prioridad)
    : id(id), peso(peso), destino(destino),
      prioridad(prioridad), idDronAsignado("") {}

// --- Getters ---
const string& Paquete::getId()             const { return id; }
double        Paquete::getPeso()           const { return peso; }
const string& Paquete::getDestino()        const { return destino; }
Prioridad     Paquete::getPrioridad()      const { return prioridad; }
const string& Paquete::getIdDronAsignado() const { return idDronAsignado; }

// --- Setter ---
void Paquete::setIdDronAsignado(const string& idDron) {
    idDronAsignado = idDron;
}

// --- Sobrecarga estatica de mostrar ---

// Version 1: resumen en una linea
void Paquete::mostrar() const {
    cout << "  Paquete [" << id << "]"
         << "  Peso: "      << peso    << " kg"
         << "  Destino: "   << destino
         << "  Prioridad: " << prioridadATexto(prioridad);
    if (!idDronAsignado.empty())
        cout << "  --> Dron: " << idDronAsignado;
    else
        cout << "  --> EN COLA";
    cout << "\n";
}

// Version 2: con parametro verbose (sobrecarga estatica)
void Paquete::mostrar(bool verbose) const {
    mostrar();  // llama a la version basica
    if (verbose) {
        cout << "    ID interno: " << id
             << " | Prioridad enum: " << (int)prioridad << "\n";
    }
}

string Paquete::prioridadATexto(Prioridad p) {
    return (p == URGENTE) ? "URGENTE" : "Normal";
}
