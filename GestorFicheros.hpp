// =============================================================
// GestorFicheros.hpp
// Lectura y escritura de archivos .txt del sistema SIGFD.
// Las funciones lanzan ExcepcionFichero si no pueden
// abrir/crear el archivo (en vez de cout + return false).
// =============================================================
#pragma once
#include <string>
#include <vector>
#include "Excepciones.hpp"
#include "Drone.hpp"

// ---- Registro de un vuelo completado ----
class RegistroVuelo {
public:
    std::string fecha;
    std::string idDron;
    std::string tipoDron;
    std::string idPaquete;
    std::string ruta;
    double      kmRecorridos;

    RegistroVuelo();
    void mostrar() const;
};

// ---- Funciones ----

// Lee flota.txt y llama a crearDron() (que usa 'new') por cada linea.
// Lanza ExcepcionFichero si no puede abrir el fichero.
// Formato: TIPO;ID;NOMBRE;BATERIA_ACTUAL
void cargarFlota(std::vector<Drone*>& drones,
                 const std::string& nombreFichero);

// Escribe el historial en un .txt.
// Lanza ExcepcionFichero si no puede crear el fichero.
void exportarInforme(const std::vector<RegistroVuelo>& vuelos,
                     const std::string& nombreFichero);

// Devuelve fecha/hora actual como string
std::string obtenerFechaHoraActual();
