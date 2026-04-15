// =============================================================
// GestorFicheros.cpp
// Lectura y escritura de archivos .txt del sistema.
// Usamos <fstream> para trabajar con ficheros.
// =============================================================

#include "GestorFicheros.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

using namespace std;

// =============================================================
// cargarFlota: lee flota.txt y rellena el array de drones
//
// Formato de cada linea:
//   TIPO;ID;NOMBRE;BATERIA_ACTUAL
// Ejemplo:
//   Standard;D001;Aguila-1;75.5
//   Express;D002;Falcon-3;60.0
//   HeavyDuty;D003;Titan-1;110.0
// =============================================================
bool cargarFlota(Drone drones[], int& numDrones, int maxDrones,
                 string nombreFichero)
{
    ifstream archivo(nombreFichero);

    // Si no se puede abrir, informamos y devolvemos false
    if (!archivo.is_open()) {
        cout << "[ERROR] No se puede abrir el fichero: " << nombreFichero << "\n";
        return false;
    }

    string linea;
    int dronesLeidos = 0;

    while (getline(archivo, linea)) {
        // Ignoramos lineas vacias y comentarios (empiezan por '#')
        if (linea.empty() || linea[0] == '#') continue;

        // Separamos la linea por ';' de forma manual
        string campos[4];
        int    campo  = 0;
        string actual = "";

        for (int i = 0; i < (int)linea.size() && campo < 4; i++) {
            if (linea[i] == ';') {
                campos[campo] = actual;
                actual = "";
                campo++;
            } else {
                actual += linea[i];
            }
        }
        campos[campo] = actual;

        // Necesitamos al menos 4 campos validos
        if (campo < 3) {
            cout << "[AVISO] Linea mal formateada, se omite: " << linea << "\n";
            continue;
        }

        string tipo   = campos[0];
        string id     = campos[1];
        string nombre = campos[2];
        double bateria = 0.0;

        if (!campos[3].empty()) {
            bateria = stod(campos[3]);
        }

        // Verificamos espacio en el array
        if (numDrones >= maxDrones) {
            cout << "[AVISO] Flota llena. Se omite dron: " << id << "\n";
            continue;
        }

        // Creamos el dron segun su tipo
        if (tipo == "Standard") {
            configurarDron(drones[numDrones], STANDARD, id, nombre);
        } else if (tipo == "Express") {
            configurarDron(drones[numDrones], EXPRESS, id, nombre);
        } else if (tipo == "HeavyDuty") {
            configurarDron(drones[numDrones], HEAVY_DUTY, id, nombre);
        } else {
            cout << "[AVISO] Tipo de dron desconocido: " << tipo << "\n";
            continue;
        }

        // Ajustamos la bateria actual (puede diferir de la autonomia maxima)
        drones[numDrones].bateriaActual = bateria;

        numDrones++;
        dronesLeidos++;
    }

    archivo.close();
    cout << "[FICHERO] Flota cargada: " << dronesLeidos << " drones.\n";

    if (dronesLeidos > 0) return true;
    return false;
}

// =============================================================
// exportarInforme: escribe el historial de vuelos en un .txt
// =============================================================
void exportarInforme(const RegistroVuelo vuelos[], int numVuelos,
                     string nombreFichero)
{
    ofstream archivo(nombreFichero);

    if (!archivo.is_open()) {
        cout << "[ERROR] No se puede crear el fichero: " << nombreFichero << "\n";
        return;
    }

    // Cabecera del informe
    archivo << "======================================================\n";
    archivo << "   INFORME DE ENVIOS - SIGFD\n";
    archivo << "   Generado: " << obtenerFechaHoraActual() << "\n";
    archivo << "======================================================\n\n";

    if (numVuelos == 0) {
        archivo << "  No se realizaron envios hoy.\n";
    } else {
        archivo << "  Total de vuelos realizados: " << numVuelos << "\n\n";
        archivo << "------------------------------------------------------\n";

        // Escribimos cada vuelo del historial
        for (int i = 0; i < numVuelos; i++) {
            const RegistroVuelo& v = vuelos[i];
            archivo << "  Vuelo #" << (i + 1) << "\n";
            archivo << "    Fecha/Hora : " << v.fecha        << "\n";
            archivo << "    Dron       : " << v.idDron
                    << " (" << v.tipoDron << ")\n";
            archivo << "    Paquete    : " << v.idPaquete    << "\n";
            archivo << "    Ruta       : " << v.ruta         << "\n";
            archivo << "    Km volados : " << v.kmRecorridos << " km\n";
            archivo << "------------------------------------------------------\n";
        }
    }

    archivo << "\n  Fin del informe.\n";
    archivo.close();

    cout << "[FICHERO] Informe exportado a: " << nombreFichero << "\n";
}

// =============================================================
// obtenerFechaHoraActual: devuelve la hora actual como string
// =============================================================
string obtenerFechaHoraActual() {
    time_t ahora = time(nullptr);
    tm     tiempoLocal{};
    localtime_s(&tiempoLocal, &ahora);
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &tiempoLocal);
    return string(buffer);
}
