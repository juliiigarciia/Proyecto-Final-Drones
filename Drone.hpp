// =============================================================
// Drone.hpp
// Jerarquia de clases para los drones del sistema SIGFD.
//
// DISENO:
//   Drone (clase base abstracta)
//     |-- DroneStandard  (60 km/h | 15 kg | 80 km)
//     |-- DroneExpress   (120km/h |  8 kg | 60 km)
//     |-- DroneHeavyDuty ( 40km/h | 50 kg |120 km)
//
// POLIMORFISMO DINAMICO: calcularBateria(), getVelocidad(),
//   getCargaMaxima() y getTipo() son virtuales puros.
//   El compilador resuelve la llamada en tiempo de ejecucion
//   segun el tipo real del objeto apuntado.
//
// VARIABLE ESTATICA: totalDronesCreados cuenta todos los
//   objetos Drone vivos en memoria (incrementa en constructor,
//   decrementa en destructor).
// =============================================================
#pragma once
#include <string>
#include "Excepciones.hpp"

enum EstadoDron { DISPONIBLE, EN_VUELO, EN_MANTENIMIENTO };

// =============================================================
// CLASE BASE ABSTRACTA
// =============================================================
class Drone {
private:
    std::string id;
    std::string nombre;
    EstadoDron  estado;
    double      bateriaActual;
    double      autonomia;

    // Variable estatica: compartida por TODOS los objetos Drone
    static int totalDronesCreados;

public:
    // Constructor / destructor virtual (necesario con herencia)
    Drone(const std::string& id, const std::string& nombre, double autonomia);
    virtual ~Drone();

    // --- Getters ---
    const std::string& getId()            const;
    const std::string& getNombre()        const;
    EstadoDron         getEstado()        const;
    double             getBateriaActual() const;
    double             getAutonomia()     const;

    // --- Setters ---
    void setEstado(EstadoDron e);
    void setBateriaActual(double b);

    // --- Metodos virtuales puros (polimorfismo dinamico) ---
    // Cada clase hija DEBE implementarlos
    virtual double      getVelocidad()              const = 0;
    virtual double      getCargaMaxima()            const = 0;
    virtual double      calcularBateria(double peso) const = 0;
    virtual std::string getTipo()                   const = 0;

    // --- Metodos concretos ---
    void recargar();
    bool tieneBateriaSuficiente(double kmNecesarios) const;

    // Sobrecarga estatica de mostrar (mismo nombre, distinta firma)
    void mostrar()             const;   // resumen una linea
    void mostrar(bool detallado) const; // con velocidad y carga max

    // --- Metodo e informacion estatica ---
    static int         getTotalCreados();
    static std::string estadoATexto(EstadoDron e);
};

// =============================================================
// CLASE HIJA: DroneStandard
// =============================================================
class DroneStandard : public Drone {
public:
    DroneStandard(const std::string& id, const std::string& nombre);

    double      getVelocidad()              const override;
    double      getCargaMaxima()            const override;
    double      calcularBateria(double peso) const override;
    std::string getTipo()                   const override;
};

// =============================================================
// CLASE HIJA: DroneExpress
// =============================================================
class DroneExpress : public Drone {
public:
    DroneExpress(const std::string& id, const std::string& nombre);

    double      getVelocidad()              const override;
    double      getCargaMaxima()            const override;
    double      calcularBateria(double peso) const override;
    std::string getTipo()                   const override;
};

// =============================================================
// CLASE HIJA: DroneHeavyDuty
// =============================================================
class DroneHeavyDuty : public Drone {
public:
    DroneHeavyDuty(const std::string& id, const std::string& nombre);

    double      getVelocidad()              const override;
    double      getCargaMaxima()            const override;
    double      calcularBateria(double peso) const override;
    std::string getTipo()                   const override;
};

// =============================================================
// FUNCION FABRICA
// Crea el tipo correcto con 'new' segun el string de tipo.
// El llamador es responsable de llamar 'delete' (o usar vector).
// Lanza ExcepcionDronNoEncontrado si el tipo es desconocido.
// =============================================================
Drone* crearDron(const std::string& tipo,
                 const std::string& id,
                 const std::string& nombre);
