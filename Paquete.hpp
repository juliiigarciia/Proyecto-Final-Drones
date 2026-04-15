// =============================================================
// Paquete.hpp
// Tipos y funciones relacionados con los paquetes del sistema.
// =============================================================

#ifndef PAQUETE_HPP
#define PAQUETE_HPP

#include <string>

// ---- Prioridad del envio ----
enum Prioridad {
    NORMAL,
    URGENTE
};

// ---- Estructura Paquete ----
struct Paquete {
    std::string id;               // identificador unico (ej. "PKG-001")
    double      peso;             // kg
    std::string destino;          // nodo destino del grafo
    Prioridad   prioridad;
    std::string idDronAsignado;   // vacio si aun no tiene dron
};

// ---- Funciones ----

// Convierte el enum Prioridad a texto
std::string prioridadATexto(Prioridad p);

// Muestra la informacion del paquete por pantalla
void mostrarPaquete(const Paquete& p);

#endif // PAQUETE_HPP
