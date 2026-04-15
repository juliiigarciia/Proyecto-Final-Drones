// =============================================================
// GestorFicheros.hpp
// Lectura y escritura de archivos .txt del sistema.
// =============================================================

#ifndef GESTOR_FICHEROS_HPP
#define GESTOR_FICHEROS_HPP

#include "Drone.hpp"
#include <string>

// ---- Registro de un vuelo completado ----
struct RegistroVuelo {
    std::string fecha;        // fecha y hora del envio
    std::string idDron;       // ID del dron que realizo el vuelo
    std::string tipoDron;     // tipo como texto (Standard, Express...)
    std::string idPaquete;    // ID del paquete transportado
    std::string ruta;         // ruta como texto (ej. "A -> B -> C")
    double      kmRecorridos; // distancia total del vuelo
};

// ---- Funciones ----

// Lee flota.txt y rellena el array de drones.
// Formato de cada linea: TIPO;ID;NOMBRE;BATERIA
// Devuelve true si se leyo al menos un dron.
// Lanza mensaje de error si no puede abrir el fichero.
bool cargarFlota(Drone drones[], int& numDrones, int maxDrones,
                 std::string nombreFichero);

// Escribe el historial de vuelos en un fichero de texto
void exportarInforme(const RegistroVuelo vuelos[], int numVuelos,
                     std::string nombreFichero);

// Devuelve la fecha y hora actual como string
std::string obtenerFechaHoraActual();

#endif // GESTOR_FICHEROS_HPP
