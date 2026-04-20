// =============================================================
// GestorFicheros.cpp
// Implementacion de lectura/escritura de ficheros.
// Usa excepciones en vez de cout+return false.
// =============================================================
#include "GestorFicheros.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

// --- RegistroVuelo ---
RegistroVuelo::RegistroVuelo() : kmRecorridos(0.0) {}

void RegistroVuelo::mostrar() const {
    cout << "  Dron: " << idDron << " (" << tipoDron << ")"
         << " | Paquete: " << idPaquete
         << " | Ruta: " << ruta
         << " | " << kmRecorridos << " km\n";
}

// =============================================================
// cargarFlota
// Lanza ExcepcionFichero si no puede abrir el fichero.
// Usa crearDron() (que hace 'new') para gestion dinamica.
// =============================================================
void cargarFlota(vector<Drone*>& drones, const string& nombreFichero) {
    ifstream archivo(nombreFichero);
    if (!archivo.is_open()) {
        throw ExcepcionFichero("No se puede abrir: " + nombreFichero);
    }

    string linea;
    int dronesLeidos = 0;

    while (getline(archivo, linea)) {
        if (linea.empty() || linea[0] == '#') continue;

        // Parsear campos separados por ';'
        string campos[4];
        int    campo  = 0;
        string actual = "";
        for (int i = 0; i < (int)linea.size() && campo < 4; i++) {
            if (linea[i] == ';') { campos[campo++] = actual; actual = ""; }
            else                   actual += linea[i];
        }
        campos[campo] = actual;

        if (campo < 3) {
            cerr << "[AVISO] Linea mal formateada, se omite: " << linea << "\n";
            continue;
        }

        string tipo    = campos[0];
        string id      = campos[1];
        string nombre  = campos[2];
        double bateria = campos[3].empty() ? -1.0 : stod(campos[3]);

        try {
            // crearDron() usa 'new' internamente (memoria dinamica)
            Drone* d = crearDron(tipo, id, nombre);
            if (bateria >= 0.0) d->setBateriaActual(bateria);
            drones.push_back(d);
            dronesLeidos++;
        } catch (const ExcepcionDronNoEncontrado& e) {
            cerr << "[AVISO] " << e.what() << " - linea omitida\n";
        }
    }

    archivo.close();
    cout << "[FICHERO] Flota cargada: " << dronesLeidos << " drones.\n";
}

// =============================================================
// exportarInforme
// Lanza ExcepcionFichero si no puede crear el fichero.
// =============================================================
void exportarInforme(const vector<RegistroVuelo>& vuelos,
                     const string& nombreFichero)
{
    ofstream archivo(nombreFichero);
    if (!archivo.is_open()) {
        throw ExcepcionFichero("No se puede crear: " + nombreFichero);
    }

    archivo << "======================================================\n";
    archivo << "   INFORME DE ENVIOS - SIGFD\n";
    archivo << "   Generado: " << obtenerFechaHoraActual() << "\n";
    archivo << "======================================================\n\n";

    if (vuelos.empty()) {
        archivo << "  No se realizaron envios hoy.\n";
    } else {
        archivo << "  Total de vuelos realizados: " << vuelos.size() << "\n\n";
        archivo << "------------------------------------------------------\n";
        for (int i = 0; i < (int)vuelos.size(); i++) {
            const RegistroVuelo& v = vuelos[i];
            archivo << "  Vuelo #" << (i + 1) << "\n";
            archivo << "    Fecha/Hora : " << v.fecha        << "\n";
            archivo << "    Dron       : " << v.idDron << " (" << v.tipoDron << ")\n";
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
// obtenerFechaHoraActual
// =============================================================
string obtenerFechaHoraActual() {
    time_t ahora = time(nullptr);
    tm     local{};
    localtime_s(&local, &ahora);
    char buf[30];
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &local);
    return string(buf);
}
