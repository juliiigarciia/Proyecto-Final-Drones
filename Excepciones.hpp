// =============================================================
// Excepciones.hpp
// Jerarquia de excepciones personalizadas del sistema SIGFD.
// Heredan de std::runtime_error para integrarse con el
// sistema estandar de excepciones de C++.
// =============================================================
#pragma once
#include <stdexcept>
#include <string>

// Lanzada cuando no se puede abrir/crear un fichero
class ExcepcionFichero : public std::runtime_error {
public:
    explicit ExcepcionFichero(const std::string& msg)
        : std::runtime_error("[FICHERO] " + msg) {}
};

// Lanzada cuando un dron no tiene bateria suficiente para el vuelo
class ExcepcionBateria : public std::runtime_error {
public:
    explicit ExcepcionBateria(const std::string& msg)
        : std::runtime_error("[BATERIA] " + msg) {}
};

// Lanzada cuando un paquete supera la carga maxima del dron asignado
class ExcepcionPesoExcesivo : public std::runtime_error {
public:
    explicit ExcepcionPesoExcesivo(const std::string& msg)
        : std::runtime_error("[PESO] " + msg) {}
};

// Lanzada cuando se busca un dron por ID y no existe
class ExcepcionDronNoEncontrado : public std::runtime_error {
public:
    explicit ExcepcionDronNoEncontrado(const std::string& msg)
        : std::runtime_error("[DRON] " + msg) {}
};

// Lanzada cuando el vector de la flota esta lleno (limite MAX_DRONES)
class ExcepcionFlotaLlena : public std::runtime_error {
public:
    explicit ExcepcionFlotaLlena(const std::string& msg)
        : std::runtime_error("[FLOTA] " + msg) {}
};

// Lanzada cuando Dijkstra no encuentra ruta al destino
class ExcepcionRutaNoEncontrada : public std::runtime_error {
public:
    explicit ExcepcionRutaNoEncontrada(const std::string& msg)
        : std::runtime_error("[RUTA] " + msg) {}
};
