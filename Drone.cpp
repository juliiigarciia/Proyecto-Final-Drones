// =============================================================
// Drone.cpp
// Implementacion de la jerarquia de clases Drone.
// =============================================================
#include "Drone.hpp"
#include <iostream>
using namespace std;

// =============================================================
// Definicion de la variable estatica (OBLIGATORIO fuera de clase)
// Sin esto el linker da "undefined reference".
// =============================================================
int Drone::totalDronesCreados = 0;

// =============================================================
// Clase base Drone
// =============================================================

Drone::Drone(const string& id, const string& nombre, double autonomia)
    : id(id), nombre(nombre), autonomia(autonomia),
      estado(DISPONIBLE), bateriaActual(autonomia)
{
    totalDronesCreados++;   // cada new DroneXxx incrementa el contador
}

Drone::~Drone() {
    totalDronesCreados--;   // cada delete decrementa el contador
}

// --- Getters ---
const string& Drone::getId()            const { return id; }
const string& Drone::getNombre()        const { return nombre; }
EstadoDron    Drone::getEstado()        const { return estado; }
double        Drone::getBateriaActual() const { return bateriaActual; }
double        Drone::getAutonomia()     const { return autonomia; }

// --- Setters ---
void Drone::setEstado(EstadoDron e)     { estado = e; }
void Drone::setBateriaActual(double b)  { bateriaActual = b; }

void Drone::recargar() {
    bateriaActual = autonomia;  // 100%
}

bool Drone::tieneBateriaSuficiente(double kmNecesarios) const {
    return bateriaActual >= kmNecesarios;
}

// --- Sobrecarga estatica: mostrar() sin parametros ---
void Drone::mostrar() const {
    cout << "  [" << getTipo() << "] " << id << " - " << nombre
         << " | Bateria: " << bateriaActual << "/" << autonomia << " km"
         << " | " << estadoATexto(estado) << "\n";
}

// --- Sobrecarga estatica: mostrar(detallado) con bool ---
void Drone::mostrar(bool detallado) const {
    mostrar();
    if (detallado) {
        cout << "    Velocidad: " << getVelocidad() << " km/h"
             << " | Carga max: " << getCargaMaxima() << " kg"
             << " | Autonomia: " << autonomia << " km\n";
    }
}

int    Drone::getTotalCreados() { return totalDronesCreados; }

string Drone::estadoATexto(EstadoDron e) {
    if (e == DISPONIBLE)       return "Disponible";
    if (e == EN_VUELO)         return "En vuelo";
    if (e == EN_MANTENIMIENTO) return "En mantenimiento";
    return "Desconocido";
}

// =============================================================
// DroneStandard: 60 km/h | 15 kg | 80 km autonomia
// =============================================================
DroneStandard::DroneStandard(const string& id, const string& nombre)
    : Drone(id, nombre, 80.0) {}

double DroneStandard::getVelocidad()              const { return 60.0; }
double DroneStandard::getCargaMaxima()            const { return 15.0; }
string DroneStandard::getTipo()                   const { return "Standard"; }
// Polimorfismo dinamico: formula especifica de este tipo
double DroneStandard::calcularBateria(double peso) const {
    return 1.0 + (peso * 0.05);
}

// =============================================================
// DroneExpress: 120 km/h | 8 kg | 60 km autonomia
// =============================================================
DroneExpress::DroneExpress(const string& id, const string& nombre)
    : Drone(id, nombre, 60.0) {}

double DroneExpress::getVelocidad()              const { return 120.0; }
double DroneExpress::getCargaMaxima()            const { return 8.0; }
string DroneExpress::getTipo()                   const { return "Express"; }
double DroneExpress::calcularBateria(double peso) const {
    return 1.5 + (peso * 0.08);
}

// =============================================================
// DroneHeavyDuty: 40 km/h | 50 kg | 120 km autonomia
// =============================================================
DroneHeavyDuty::DroneHeavyDuty(const string& id, const string& nombre)
    : Drone(id, nombre, 120.0) {}

double DroneHeavyDuty::getVelocidad()              const { return 40.0; }
double DroneHeavyDuty::getCargaMaxima()            const { return 50.0; }
string DroneHeavyDuty::getTipo()                   const { return "HeavyDuty"; }
double DroneHeavyDuty::calcularBateria(double peso) const {
    return 0.8 + (peso * 0.02);
}

// =============================================================
// Funcion fabrica: crea el objeto correcto con 'new'
// Lanza ExcepcionDronNoEncontrado si el tipo no es valido.
// =============================================================
Drone* crearDron(const string& tipo, const string& id, const string& nombre) {
    if (tipo == "Standard")  return new DroneStandard(id, nombre);
    if (tipo == "Express")   return new DroneExpress(id, nombre);
    if (tipo == "HeavyDuty") return new DroneHeavyDuty(id, nombre);
    throw ExcepcionDronNoEncontrado("Tipo de dron desconocido: '" + tipo + "'");
}
