// =============================================================
// CentroLogistico.hpp
// Clase principal del sistema: agrupa la flota (vector<Drone*>),
// la cola de espera (vector<Paquete>) y el historial de vuelos.
//
// MEMORIA DINAMICA: la flota usa vector<Drone*>.
//   Cada Drone* apunta a un objeto creado con 'new'.
//   El destructor de CentroLogistico llama 'delete' sobre cada
//   puntero para liberar la memoria correctamente.
// =============================================================
#pragma once
#include <string>
#include <vector>
#include "Drone.hpp"
#include "Paquete.hpp"
#include "GestorRutas.hpp"
#include "GestorFicheros.hpp"

// Limite logico (no de memoria, sino de negocio)
const int MAX_DRONES_FLOTA = 50;

class CentroLogistico {
private:
    std::string nombre;

    // Flota: punteros a Drone creados con 'new' (memoria dinamica)
    std::vector<Drone*>        drones;
    // Cola FIFO de paquetes pendientes
    std::vector<Paquete>       cola;
    // Historial de vuelos completados
    std::vector<RegistroVuelo> historial;

    int totalEnvios;

    // Grafo de rutas (Dijkstra)
    Grafo grafo;

    // Variable estatica: total de envios en toda la sesion (todas las instancias)
    static int totalEnviosSesion;

    // --- Metodos privados de apoyo ---
    Drone*  buscarDronPorId(const std::string& id) const;
    Drone*  buscarDronPorTipo(double peso, const std::string& tipo) const;
    Drone*  buscarDronCualquiera(double peso) const;
    bool    existePaquete(const std::string& id)   const;
    void    realizarEnvio(Drone& dron, Paquete& paquete);

public:
    CentroLogistico(const std::string& nombre);
    // Destructor: libera memoria de todos los Drone* del vector
    ~CentroLogistico();

    // --- Gestion de drones ---
    void agregarDron(Drone* d);               // toma ownership del puntero
    bool eliminarDron(const std::string& id); // llama delete internamente
    void mostrarFlota()  const;
    void mostrarFlota(bool detallado) const;  // sobrecarga estatica

    // --- Gestion de paquetes ---
    // Sobrecarga estatica: registrar desde objeto Paquete
    void registrarPaquete(const Paquete& p);
    // Sobrecarga estatica: registrar desde parametros sueltos
    void registrarPaquete(const std::string& id, double peso,
                          const std::string& destino, Prioridad prioridad);

    void ponerEnCola(const Paquete& p);
    void mostrarColaEspera() const;
    void procesarCola();

    // --- Vuelo con Dijkstra + bateria (lanza excepciones) ---
    void ejecutarVuelo(const std::string& idDron);

    // --- Mantenimiento ---
    void recargarBaterias();

    // --- Ficheros ---
    void cargarFlotaDesdeFichero(const std::string& fichero);
    void exportarInformeDia(const std::string& fichero) const;

    // --- Informes ---
    void mostrarResumen() const;

    // Getter para variable estatica
    static int getTotalEnviosSesion();
};
