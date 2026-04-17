// =============================================================
// CentroLogistico.hpp
// Estructura principal del sistema: agrupa la flota, la cola
// de espera, el historial de vuelos y el grafo de rutas.
// Siguiendo el patron: contenedor + contadores.
// =============================================================

#ifndef CENTRO_LOGISTICO_HPP
#define CENTRO_LOGISTICO_HPP

#include "Drone.hpp"
#include "Paquete.hpp"
#include "GestorRutas.hpp"
#include "GestorFicheros.hpp"

#include <string>

// ---- Constantes de capacidad ----
const int MAX_DRONES    = 50;
const int MAX_COLA      = 100;
const int MAX_HISTORIAL = 200;

// =============================================================
// Estructura CentroLogistico
// Contiene todos los datos del sistema en arrays con contador.
// =============================================================
struct CentroLogistico {
    std::string nombre;

    // Flota de drones
    Drone drones[MAX_DRONES];
    int   numDrones;

    // Paquetes en espera (sin dron asignado)
    Paquete cola[MAX_COLA];
    int     numCola;

    // Historial de todos los vuelos del dia
    RegistroVuelo historial[MAX_HISTORIAL];
    int           numVuelos;

    // Total de envios realizados
    int totalEnvios;

    // Grafo de rutas para Dijkstra
    Grafo grafo;
};

// ---- Funciones de inicializacion ----
void inicializarCentro(CentroLogistico& c, std::string nombre);

// ---- Gestion de drones ----
// Agrega un dron a la flota (falla si el ID ya existe o esta llena)
bool agregarDron(CentroLogistico& c, Drone d);
// Elimina un dron por su ID
bool eliminarDron(CentroLogistico& c, std::string idDron);
// Muestra todos los drones de la flota
void mostrarFlota(const CentroLogistico& c);

// ---- Gestion de paquetes ----
// Intenta asignar un dron al paquete automaticamente
void registrarPaquete(CentroLogistico& c, Paquete p);
// Añade el paquete directamente a la cola de espera
void ponerEnCola(CentroLogistico& c, Paquete p);
// Muestra los paquetes en espera
void mostrarColaEspera(const CentroLogistico& c);

// ---- Rutas ----
// Calcula y muestra la ruta optima desde el centro al destino
void asignarRuta(const CentroLogistico& c, std::string idDron, std::string destino);

// ---- Ficheros ----
void cargarFlotaDesdeFichero(CentroLogistico& c, std::string fichero);
void exportarInformeDia(const CentroLogistico& c, std::string fichero);

// ---- Informes ----
void mostrarResumen(const CentroLogistico& c);

#endif // CENTRO_LOGISTICO_HPP
