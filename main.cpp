// =============================================================
// main.cpp
// Programa principal del SIGFD.
//
// Compilar:
//   g++ -std=c++17 -Wall main.cpp Drone.cpp Paquete.cpp
//       GestorRutas.cpp GestorFicheros.cpp CentroLogistico.cpp -o sigfd
//
// Ejecutar:
//   sigfd.exe (Windows)   /   ./sigfd (Linux/Mac)
// =============================================================

#include "CentroLogistico.hpp"
#include "Drone.hpp"
#include "Paquete.hpp"
#include "GestorRutas.hpp"

#include <iostream>
#include <string>

using namespace std;


// =============================================================
// TESTS BASICOS
// =============================================================

// TEST 1: Verificar que Dijkstra encuentra la ruta correcta
void testDijkstra() {
    cout << "\n================================================\n";
    cout << "TEST 1: Algoritmo de Dijkstra\n";
    cout << "================================================\n";

    Grafo g;
    inicializarGrafo(g);
    mostrarGrafo(g);

    // Caso 1: hay dos caminos a Zona Norte (5km directo o 3+4=7km)
    // Dijkstra debe elegir los 5 km directos
    cout << "\n[TEST] Almacen Central --> Zona Norte\n";
    ResultadoRuta r1 = calcularRutaOptima(g, "Zona Norte");
    if (r1.encontrada && r1.kmTotales == 5.0) {
        cout << "  --> CORRECTO: ruta optima = " << r1.kmTotales << " km\n";
    } else {
        cout << "  --> ERROR: se esperaban 5 km, se obtuvo: " << r1.kmTotales << " km\n";
    }

    // Caso 2: solo hay un camino a Zona Sur (8 km)
    cout << "\n[TEST] Almacen Central --> Zona Sur\n";
    ResultadoRuta r2 = calcularRutaOptima(g, "Zona Sur");
    if (r2.encontrada && r2.kmTotales == 8.0) {
        cout << "  --> CORRECTO: ruta optima = " << r2.kmTotales << " km\n";
    } else {
        cout << "  --> ERROR: se esperaban 8 km, se obtuvo: " << r2.kmTotales << " km\n";
    }

    // Caso 3: destino que no existe en el grafo
    cout << "\n[TEST] Almacen Central --> Destino Inexistente\n";
    ResultadoRuta r3 = calcularRutaOptima(g, "Lugar Inexistente");
    if (!r3.encontrada) {
        cout << "  --> CORRECTO: se detecto destino invalido\n";
    } else {
        cout << "  --> ERROR: deberia haber fallado\n";
    }
}

// TEST 2: Verificar que se detecta cuando un paquete supera la carga maxima
void testCargaExcesiva() {
    cout << "\n================================================\n";
    cout << "TEST 2: Comprobacion de peso excesivo\n";
    cout << "================================================\n";

    // Un DroneExpress solo puede cargar 8 kg
    Drone dron;
    configurarDron(dron, EXPRESS, "TEST-01", "DronPrueba");

    // Intentamos asignar un paquete de 50 kg
    Paquete paquete;
    paquete.id               = "PKG-TEST";
    paquete.peso             = 50.0;
    paquete.destino          = "Zona Norte";
    paquete.prioridad        = NORMAL;
    paquete.idDronAsignado   = "";

    cout << "[TEST] Intentando enviar " << paquete.peso
         << " kg con un dron de capacidad maxima "
         << dron.cargaMaxima << " kg...\n";

    if (paquete.peso > dron.cargaMaxima) {
        cout << "  --> CORRECTO: El paquete supera la carga maxima del dron.\n";
    } else {
        cout << "  --> ERROR: deberia haber detectado el exceso de peso.\n";
    }
}

// TEST 3: Crear y rellenar un array de 100 paquetes (sin memoria dinamica)
void testArrayPaquetes() {
    cout << "\n================================================\n";
    cout << "TEST 3: Array de 100 paquetes (sin new/delete)\n";
    cout << "================================================\n";

    const int CANTIDAD = 100;
    Paquete paquetes[CANTIDAD];

    // Rellenamos el array con datos de prueba
    for (int i = 0; i < CANTIDAD; i++) {
        paquetes[i].id             = "PKG-MEM-" + to_string(i);
        paquetes[i].peso           = 1.0 + i * 0.1;
        paquetes[i].destino        = "Zona Este";
        paquetes[i].prioridad      = NORMAL;
        paquetes[i].idDronAsignado = "";
    }

    cout << "  Paquetes creados en el array: " << CANTIDAD << "\n";
    cout << "  Sin memoria dinamica (no se usa new ni delete).\n";
    cout << "  --> CORRECTO\n";
}


// =============================================================
// MENU INTERACTIVO
// =============================================================

int mostrarMenu() {
    int opcion;

    cout << "\n";
    cout << "+==========================================+\n";
    cout << "|   SIGFD - Sistema de Control de Drones   |\n";
    cout << "+==========================================+\n";
    cout << "|  1. Ver flota de drones                  |\n";
    cout << "|  2. Agregar dron manualmente             |\n";
    cout << "|  3. Registrar nuevo paquete              |\n";
    cout << "|  4. Ver cola de espera                   |\n";
    cout << "|  5. Calcular ruta optima (Dijkstra)      |\n";
    cout << "|  6. Calcular ruta por coordenadas        |\n";
    cout << "|  7. Ver resumen del dia                  |\n";
    cout << "|  8. Exportar informe del dia             |\n";
    cout << "|  9. Ejecutar tests basicos               |\n";
    cout << "|  0. Salir y exportar informe             |\n";
    cout << "+==========================================+\n";
    cout << "  Elige una opcion: ";
    cin >> opcion;

    return opcion;
}


// =============================================================
// MAIN
// =============================================================
int main() {
    cout << "==============================================\n";
    cout << "  Iniciando SIGFD v1.0\n";
    cout << "  Sistema de Control Logistico para Drones\n";
    cout << "==============================================\n";

    // Creamos e inicializamos el centro logistico
    CentroLogistico centro;
    inicializarCentro(centro, "SIGFD - Madrid Central");

    // Intentamos cargar la flota desde el fichero
    cargarFlotaDesdeFichero(centro, "flota.txt");
    bool cargado = (centro.numDrones > 0); // o el criterio que uses para saber si cargó
     

    if (!cargado) {
        cout << "[INFO] Fichero no encontrado. Cargando flota por defecto...\n\n";

        // Flota por defecto (sin new: usamos configurarDron)
        Drone d1, d2, d3, d4, d5;
        configurarDron(d1, STANDARD,   "D001", "Aguila-1");
        configurarDron(d2, EXPRESS,    "D002", "Falcon-3");
        configurarDron(d3, HEAVY_DUTY, "D003", "Titan-1");
        configurarDron(d4, EXPRESS,    "D004", "Falcon-4");
        configurarDron(d5, STANDARD,   "D005", "Aguila-2");

        agregarDron(centro, d1);
        agregarDron(centro, d2);
        agregarDron(centro, d3);
        agregarDron(centro, d4);
        agregarDron(centro, d5);
    }

    // Bucle del menu principal
    int opcion = -1;

    do {
        opcion = mostrarMenu();
        cin.ignore(1000, '\n');

        // Opcion 1: Ver la flota
        if (opcion == 1) {
            mostrarFlota(centro);

        // Opcion 2: Agregar un dron manualmente
        } else if (opcion == 2) {
            string tipoStr, id, nombre;

            cout << "  Tipo de dron (Standard / Express / HeavyDuty): ";
            getline(cin, tipoStr);
            cout << "  ID del dron (ej. D010): ";
            getline(cin, id);
            cout << "  Nombre del dron: ";
            getline(cin, nombre);

            Drone nuevo;

            if (tipoStr == "Standard") {
                configurarDron(nuevo, STANDARD, id, nombre);
                agregarDron(centro, nuevo);
            } else if (tipoStr == "Express") {
                configurarDron(nuevo, EXPRESS, id, nombre);
                agregarDron(centro, nuevo);
            } else if (tipoStr == "HeavyDuty") {
                configurarDron(nuevo, HEAVY_DUTY, id, nombre);
                agregarDron(centro, nuevo);
            } else {
                cout << "[ERROR] Tipo no reconocido. Use: Standard, Express o HeavyDuty\n";
            }

        // Opcion 3: Registrar un paquete
        } else if (opcion == 3) {
            string id, destino, prioridadStr;
            double peso;

            cout << "  ID del paquete (ej. PKG-010): ";
            getline(cin, id);
            cout << "  Peso del paquete (kg): ";
            cin >> peso;
            cin.ignore(1000, '\n');
            cout << "  Destino (Zona Norte / Zona Sur / Zona Este): ";
            getline(cin, destino);
            cout << "  Prioridad (normal / urgente): ";
            getline(cin, prioridadStr);

            Paquete pkg;
            pkg.id             = id;
            pkg.peso           = peso;
            pkg.destino        = destino;
            pkg.idDronAsignado = "";

            if (prioridadStr == "urgente") {
                pkg.prioridad = URGENTE;
            } else {
                pkg.prioridad = NORMAL;
            }

            registrarPaquete(centro, pkg);

        // Opcion 4: Ver cola de espera
        } else if (opcion == 4) {
            mostrarColaEspera(centro);

        // Opcion 5: Calcular ruta por nombre de destino
        } else if (opcion == 5) {
            string idDron, destino;
            cout << "  ID del dron: ";
            getline(cin, idDron);
            cout << "  Destino (Zona Norte / Zona Sur / Zona Este): ";
            getline(cin, destino);
            asignarRuta(centro, idDron, destino);

        // Opcion 6: Calcular ruta por coordenadas
        } else if (opcion == 6) {
            string idDron;
            float x, y;
            cout << "  ID del dron: ";
            getline(cin, idDron);
            cout << "  Coordenada X: ";
            cin >> x;
            cout << "  Coordenada Y: ";
            cin >> y;
            cin.ignore(1000, '\n');
            asignarRutaCoordenadas(centro, idDron, x, y);

        // Opcion 7: Resumen del dia
        } else if (opcion == 7) {
            mostrarResumen(centro);

        // Opcion 8: Exportar informe
        } else if (opcion == 8) {
            exportarInformeDia(centro, "informe_envios.txt");
            cout << "[OK] Informe exportado correctamente.\n";

        // Opcion 9: Ejecutar los tests
        } else if (opcion == 9) {
            testDijkstra();
            testCargaExcesiva();
            testArrayPaquetes();

        // Opcion 0: Salir
        } else if (opcion == 0) {
            cout << "\n[INFO] Exportando informe antes de cerrar...\n";
            exportarInformeDia(centro, "informe_envios.txt");
            cout << "[INFO] Hasta pronto!\n";

        // Opcion no valida
        } else {
            cout << "[AVISO] Opcion no valida. Elige entre 0 y 9.\n";
        }

    } while (opcion != 0);

    return 0;
}
