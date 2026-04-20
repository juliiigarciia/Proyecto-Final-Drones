// =============================================================
// Paquete.hpp
// Clase Paquete con atributos privados y metodos publicos.
// Aplica encapsulacion: nadie puede modificar id/peso/destino
// directamente, solo a traves de los metodos publicos.
// =============================================================
#pragma once
#include <string>

enum Prioridad { NORMAL, URGENTE };

class Paquete {
private:
    std::string id;
    double      peso;
    std::string destino;
    Prioridad   prioridad;
    std::string idDronAsignado;   // "" si aun no tiene dron

public:
    // Constructor: inicializa todos los campos obligatorios
    Paquete(const std::string& id, double peso,
            const std::string& destino, Prioridad prioridad);

    // --- Getters (lectura) ---
    const std::string& getId()             const;
    double             getPeso()           const;
    const std::string& getDestino()        const;
    Prioridad          getPrioridad()      const;
    const std::string& getIdDronAsignado() const;

    // --- Setter (unica modificacion permitida) ---
    void setIdDronAsignado(const std::string& idDron);

    // Sobrecarga estatica: dos versiones de mostrar()
    void mostrar()             const;   // resumen en una linea
    void mostrar(bool verbose) const;   // detallado (verbose=true)

    static std::string prioridadATexto(Prioridad p);
};
