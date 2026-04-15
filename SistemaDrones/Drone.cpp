// =============================================================
// Drone.cpp
// Implementacion de las funciones para trabajar con drones.
// Sin clases ni herencia: usamos el enum TipoDron para
// diferenciar el comportamiento de cada tipo de dron.
// =============================================================

#include "Drone.hpp"
#include <iostream>
#include <string>

using namespace std;

// =============================================================
// Inicializacion
// =============================================================

// Asigna el ID, nombre y los valores por defecto segun el tipo.
// La bateria empieza siempre al 100% (igual a la autonomia).
void configurarDron(Drone& d, TipoDron tipo, string id, string nombre) {
    d.id = id;
    d.nombre = nombre;
    d.tipo = tipo;
    d.estado = DISPONIBLE;

    // Cada tipo tiene velocidad, carga y autonomia distintas
    if (tipo == STANDARD) {
        d.velocidad = 60.0;
        d.cargaMaxima = 15.0;
        d.autonomia = 80.0;
    }
    else if (tipo == EXPRESS) {
        d.velocidad = 120.0;
        d.cargaMaxima = 8.0;
        d.autonomia = 60.0;
    }
    else if (tipo == HEAVY_DUTY) {
        d.velocidad = 40.0;
        d.cargaMaxima = 50.0;
        d.autonomia = 120.0;
    }

    // Bateria al 100% al crear el dron
    d.bateriaActual = d.autonomia;
}

// =============================================================
// Conversiones a texto
// =============================================================

string tipoDronATexto(TipoDron t) {
    if (t == STANDARD)   return "Standard";
    if (t == EXPRESS)    return "Express";
    if (t == HEAVY_DUTY) return "HeavyDuty";
    return "Desconocido";
}

string estadoDronATexto(EstadoDron e) {
    if (e == DISPONIBLE)       return "Disponible";
    if (e == EN_VUELO)         return "En vuelo";
    if (e == EN_MANTENIMIENTO) return "En mantenimiento";
    return "Desconocido";
}

// =============================================================
// Calculo de bateria
// =============================================================

// Devuelve cuantos km de bateria consume el dron por cada km volado,
// segun su tipo y el peso del paquete que transporta.
//
// Formulas:
//   Standard:   1.0 + peso * 0.05  (ej: 10 kg --> 1.5 km/km)
//   Express:    1.5 + peso * 0.08  (ej:  5 kg --> 1.9 km/km)
//   HeavyDuty:  0.8 + peso * 0.02  (ej: 30 kg --> 1.4 km/km)
double calcularBateria(const Drone& d, double peso) {
    if (d.tipo == STANDARD)   return 1.0 + (peso * 0.05);
    if (d.tipo == EXPRESS)    return 1.5 + (peso * 0.08);
    if (d.tipo == HEAVY_DUTY) return 0.8 + (peso * 0.02);
    return 1.0;
}

// =============================================================
// Visualizacion
// =============================================================

// Muestra toda la informacion del dron en una sola linea
void mostrarDron(const Drone& d) {
    cout << "  [" << tipoDronATexto(d.tipo) << "] "
        << d.id << " - " << d.nombre << "\n"
        << "    Velocidad: " << d.velocidad << " km/h"
        << "  |  Carga max: " << d.cargaMaxima << " kg"
        << "  |  Autonomia: " << d.autonomia << " km"
        << "  |  Bateria: " << d.bateriaActual << " km"
        << "  |  Estado: " << estadoDronATexto(d.estado) << "\n";
}
