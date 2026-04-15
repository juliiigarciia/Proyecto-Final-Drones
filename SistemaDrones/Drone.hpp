// =============================================================
// Drone.hpp
// Tipos y funciones relacionados con los drones del sistema.
// En vez de clases con herencia, usamos un enum TipoDron
// para diferenciar el comportamiento de cada tipo.
// =============================================================

#ifndef DRONE_HPP
#define DRONE_HPP

#include <string>

// ---- Tipo de dron (sustituye a las clases hijas) ----
enum TipoDron {
    STANDARD,    // uso general: 60 km/h, 15 kg, 80 km
    EXPRESS,     // rapido: 120 km/h, 8 kg, 60 km
    HEAVY_DUTY   // pesado: 40 km/h, 50 kg, 120 km
};

// ---- Estado actual del dron ----
enum EstadoDron {
    DISPONIBLE,
    EN_VUELO,
    EN_MANTENIMIENTO
};

// ---- Estructura Drone ----
struct Drone {
    std::string id;
    std::string nombre;
    TipoDron    tipo;
    EstadoDron  estado;
    double      velocidad;      // km/h
    double      cargaMaxima;    // kg maximos que puede cargar
    double      autonomia;      // km maximos de bateria
    double      bateriaActual;  // km de bateria restantes
};

// ---- Funciones ----

// Inicializa un dron con los valores por defecto segun su tipo
void configurarDron(Drone& d, TipoDron tipo, std::string id, std::string nombre);

// Conversiones a texto legible
std::string tipoDronATexto(TipoDron t);
std::string estadoDronATexto(EstadoDron e);

// Calcula el consumo de bateria (km/km) segun tipo y peso transportado
double calcularBateria(const Drone& d, double peso);

// Muestra toda la informacion del dron por pantalla
void mostrarDron(const Drone& d);

#endif // DRONE_HPP
