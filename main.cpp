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
#include <thread>
#include <chrono>

using namespace std;

// =============================================================
// ZONAS DISPONIBLES (array global - fuente unica de verdad)
// =============================================================

// Todos los destinos validos del grafo de Madrid.
// Al agregar una zona nueva en GestorRutas.cpp, añadela aqui tambien.
const string ZONAS[] = {
    "Madrid Centro",
    "Vallecas",
    "Alcorcon",
    "Getafe",
    "Las Rozas",
    "Alcobendas",
    "Pozuelo",
    "Mostoles",
    "Leganes",
    "Fuenlabrada",
    "Parla",
    "Pinto",
    "Majadahonda",
    "Boadilla",
    "Coslada",
    "Rivas",
    "San Fernando",
    "Torrejon",
    "Alcala de Henares",
    "San Sebastian de los Reyes"
};
const int NUM_ZONAS = 20;

// =============================================================
// UTILIDADES DE PANTALLA
// =============================================================

// Limpia la consola (Windows)
void limpiarPantalla() {
    system("cls");
}

// Muestra "Presiona Enter para continuar..." y espera
void pausar() {
    cout << "\n  Presiona Enter para volver al menu principal...";
    cin.get();
}

// Lee un entero de cin con blindaje anti-letras.
// Devuelve true si la lectura fue valida, false si el usuario escribio basura.
bool leerEntero(int& valor) {
    cin >> valor;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "\n  [ERROR] Introduce un numero valido.\n";
        return false;
    }
    cin.ignore(10000, '\n');
    return true;
}

// Muestra el menu de zonas y devuelve la zona elegida (o "" si es invalida).
string elegirZona() {
    cout << "\n  --- DESTINOS DISPONIBLES ---\n";
    for (int i = 0; i < NUM_ZONAS; i++) {
        cout << "    " << (i + 1) << ") " << ZONAS[i] << "\n";
    }
    cout << "  Selecciona el numero de destino: ";

    int opcion;
    if (!leerEntero(opcion)) return "";

    if (opcion >= 1 && opcion <= NUM_ZONAS) {
        return ZONAS[opcion - 1];
    }

    cout << "  [ERROR] Numero de zona no valido.\n";
    return "";
}

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

    // Caso 1: ruta directa Madrid Centro -> Vallecas (8 km)
    cout << "\n[TEST] Madrid Centro --> Vallecas\n";
    ResultadoRuta r1 = calcularRutaOptima(g, "Vallecas");
    if (r1.encontrada && r1.kmTotales == 8.0) {
        cout << "  --> CORRECTO: ruta optima = " << r1.kmTotales << " km\n";
    }
    else {
        cout << "  --> ERROR: se esperaban 8 km, se obtuvo: " << r1.kmTotales << " km\n";
    }

    // Caso 2: ruta directa Madrid Centro -> Pozuelo (10 km)
    cout << "\n[TEST] Madrid Centro --> Pozuelo\n";
    ResultadoRuta r2 = calcularRutaOptima(g, "Pozuelo");
    if (r2.encontrada && r2.kmTotales == 10.0) {
        cout << "  --> CORRECTO: ruta optima = " << r2.kmTotales << " km\n";
    }
    else {
        cout << "  --> ERROR: se esperaban 10 km, se obtuvo: " << r2.kmTotales << " km\n";
    }

    // Caso 3: destino que no existe en el grafo
    cout << "\n[TEST] Madrid Centro --> Destino Inexistente\n";
    ResultadoRuta r3 = calcularRutaOptima(g, "Lugar Inexistente");
    if (!r3.encontrada) {
        cout << "  --> CORRECTO: se detecto destino invalido\n";
    }
    else {
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
    paquete.id = "PKG-TEST";
    paquete.peso = 50.0;
    paquete.destino = "Vallecas";
    paquete.prioridad = NORMAL;
    paquete.idDronAsignado = "";

    cout << "[TEST] Intentando enviar " << paquete.peso
        << " kg con un dron de capacidad maxima "
        << dron.cargaMaxima << " kg...\n";

    if (paquete.peso > dron.cargaMaxima) {
        cout << "  --> CORRECTO: El paquete supera la carga maxima del dron.\n";
    }
    else {
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
        paquetes[i].id = "PKG-MEM-" + to_string(i);
        paquetes[i].peso = 1.0 + i * 0.1;
        paquetes[i].destino = "Vallecas";
        paquetes[i].prioridad = NORMAL;
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
    limpiarPantalla();

    cout << "\n";
    cout << "+==========================================+\n";
    cout << "|   SIGFD - Sistema de Control de Drones   |\n";
    cout << "+==========================================+\n";
    cout << "|  1. Ver flota de drones                  |\n";
    cout << "|  2. Agregar dron manualmente             |\n";
    cout << "|  3. Registrar nuevo paquete              |\n";
    cout << "|  4. Ver cola de espera                   |\n";
    cout << "|  5. Calcular ruta optima (Dijkstra)      |\n";
    cout << "|  6. Guia de uso y funcionamiento         |\n";
    cout << "|  7. Ver resumen del dia                  |\n";
    cout << "|  8. Exportar informe del dia             |\n";
    cout << "|  9. Ejecutar tests basicos               |\n";
    cout << "|  0. Salir y exportar informe             |\n";
    cout << "+==========================================+\n";
    cout << "  Elige una opcion: ";

    int opcion = -1;
    leerEntero(opcion);   // blindado contra letras
    return opcion;
}

// =============================================================
// OPCIONES DEL MENU (funciones independientes)
// =============================================================

void opcionVerFlota(CentroLogistico& centro) {
    mostrarFlota(centro);
    pausar();
}

void opcionAgregarDron(CentroLogistico& centro) {
    // Menu de tipo de dron por numero
    cout << "\n  --- TIPO DE DRON ---\n";
    cout << "    1) Standard\n";
    cout << "    2) Express\n";
    cout << "    3) HeavyDuty\n";
    cout << "  Selecciona el tipo: ";

    int tipoNum = 0;
    if (!leerEntero(tipoNum) || tipoNum < 1 || tipoNum > 3) {
        cout << "  [ERROR] Tipo no valido.\n";
        pausar();
        return;
    }

    string id, nombre;
    cout << "  ID del dron (ej. D010): ";
    getline(cin, id);
    cout << "  Nombre del dron: ";
    getline(cin, nombre);

    Drone nuevo;
    if (tipoNum == 1) configurarDron(nuevo, STANDARD, id, nombre);
    else if (tipoNum == 2) configurarDron(nuevo, EXPRESS, id, nombre);
    else                   configurarDron(nuevo, HEAVY_DUTY, id, nombre);

    agregarDron(centro, nuevo);
    cout << "  [OK] Dron '" << nombre << "' agregado.\n";
    pausar();
}

void opcionGuiaUso() {
    limpiarPantalla();
    cout << "\n===========================================================\n";
    cout << "           GUIA DE USO Y FUNCIONAMIENTO - SIGFD            \n";
    cout << "===========================================================\n";
    cout << "  El Sistema de Gestion de Flota de Drones (SIGFD) permite \n";
    cout << "  coordinar envios de paquetes usando una flota variada.   \n\n";
    cout << "  1. Flota de Drones:\n";
    cout << "     - Standard: Uso general (carga media, velocidad media).\n";
    cout << "     - Express: Entregas rapidas (poca carga, muy rapidos).\n";
    cout << "     - HeavyDuty: Cargas pesadas (mucha carga, mas lentos).\n\n";
    cout << "  2. Registro de Paquetes:\n";
    cout << "     Al registrar un paquete, puedes 'Enviar ahora' o mandarlo\n";
    cout << "     a la 'Cola de espera'. Si envias ahora, el sistema busca\n";
    cout << "     el dron mas adecuado segun el peso del paquete. Si no hay\n";
    cout << "     drones disponibles o sin bateria, ira a la cola de espera.\n\n";
    cout << "  3. Rutas (Algoritmo de Dijkstra):\n";
    cout << "     El sistema conoce un mapa de la Comunidad de Madrid. Usa el\n";
    cout << "     algoritmo de Dijkstra para encontrar el camino mas corto\n";
    cout << "     (menos kilometros) entre el centro y el destino.\n\n";
    cout << "  4. Consumo de Bateria:\n";
    cout << "     Cada dron gasta mas bateria cuanto mas peso lleva. Si un\n";
    cout << "     dron no tiene bateria suficiente para el viaje, no podra\n";
    cout << "     realizar el envio.\n";
    cout << "===========================================================\n";
    pausar();
}

void opcionRegistrarPaquete(CentroLogistico& centro) {
    string id;
    double peso;

    cout << "\n  ID del paquete (ej. PKG-010): ";
    getline(cin, id);

    cout << "  Peso del paquete (kg): ";
    cin >> peso;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "  [ERROR] Peso no valido. Introduce un numero (ej: 3.5)\n";
        pausar();
        return;
    }
    cin.ignore(10000, '\n');

    // Destino por numero
    string destino = elegirZona();
    if (destino.empty()) { pausar(); return; }

    // Prioridad por numero
    cout << "\n  --- PRIORIDAD ---\n";
    cout << "    1) Normal\n";
    cout << "    2) Urgente\n";
    cout << "  Selecciona: ";

    int priNum = 0;
    if (!leerEntero(priNum) || priNum < 1 || priNum > 2) {
        cout << "  [ERROR] Prioridad no valida.\n";
        pausar();
        return;
    }

    Paquete pkg;
    pkg.id = id;
    pkg.peso = peso;
    pkg.destino = destino;
    pkg.idDronAsignado = "";
    pkg.prioridad = (priNum == 2) ? URGENTE : NORMAL;

    cout << "\n  --- ACCION ---\n";
    cout << "    1) Enviar ahora (Asignacion automatica)\n";
    cout << "    2) Guardar en cola de espera\n";
    cout << "  Selecciona: ";
    
    int accion = 0;
    if (!leerEntero(accion) || (accion != 1 && accion != 2)) {
        cout << "  [ERROR] Accion no valida. Cancelando registro.\n";
        pausar();
        return;
    }

    cout << "\n";
    if (accion == 1) {
        registrarPaquete(centro, pkg);
        cout << "  [OK] Proceso de envio ejecutado para '" << id << "'.\n";
    } else {
        ponerEnCola(centro, pkg);
    }
    
    pausar();
}

void opcionVerCola(CentroLogistico& centro) {
    mostrarColaEspera(centro);
    pausar();
}

void opcionCalcularRuta(CentroLogistico& centro) {
    string idDron;
    cout << "\n  ID del dron: ";
    getline(cin, idDron);

    // Elegir destino por numero (a prueba de errores tipograficos)
    string destino = elegirZona();
    if (destino.empty()) { pausar(); return; }

    // Efecto "pensando..." para Dijkstra
    cout << "\n  Calculando ruta optima";
    for (int i = 0; i < 3; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        cout << "." << flush;
    }
    cout << " iRuta encontrada!\n\n";

    asignarRuta(centro, idDron, destino);
    pausar();
}

void opcionCalcularCoordenadas(CentroLogistico& centro) {

}

void opcionResumen(CentroLogistico& centro) {
    mostrarResumen(centro);
    pausar();
}

void opcionExportarInforme(CentroLogistico& centro) {
    exportarInformeDia(centro, "informe_envios.txt");
    cout << "  [OK] Informe exportado correctamente en 'informe_envios.txt'.\n";
    pausar();
}

void opcionTests() {
    testDijkstra();
    testCargaExcesiva();
    testArrayPaquetes();
    pausar();
}

// =============================================================
// MAIN
// =============================================================

int main() {
    limpiarPantalla();
    cout << "==============================================\n";
    cout << "  Iniciando SIGFD v1.0\n";
    cout << "  Sistema de Control Logistico para Drones\n";
    cout << "==============================================\n";

    // Creamos e inicializamos el centro logistico
    CentroLogistico centro;
    inicializarCentro(centro, "SIGFD - Madrid Central");

    // Intentamos cargar la flota desde el fichero
    cargarFlotaDesdeFichero(centro, "flota.txt");
    bool cargado = (centro.numDrones > 0);

    if (!cargado) {
        cout << "[INFO] Fichero no encontrado. Cargando flota por defecto...\n\n";

        Drone d1, d2, d3, d4, d5;
        configurarDron(d1, STANDARD, "D001", "Aguila-1");
        configurarDron(d2, EXPRESS, "D002", "Falcon-3");
        configurarDron(d3, HEAVY_DUTY, "D003", "Titan-1");
        configurarDron(d4, EXPRESS, "D004", "Falcon-4");
        configurarDron(d5, STANDARD, "D005", "Aguila-2");

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

        if (opcion == 1) opcionVerFlota(centro);
        else if (opcion == 2) opcionAgregarDron(centro);
        else if (opcion == 3) opcionRegistrarPaquete(centro);
        else if (opcion == 4) opcionVerCola(centro);
        else if (opcion == 5) opcionCalcularRuta(centro);
        else if (opcion == 6) opcionGuiaUso();
        else if (opcion == 7) opcionResumen(centro);
        else if (opcion == 8) opcionExportarInforme(centro);
        else if (opcion == 9) opcionTests();
        else if (opcion == 0) {
            cout << "\n[INFO] Exportando informe antes de cerrar...\n";
            exportarInformeDia(centro, "informe_envios.txt");
            cout << "[INFO] Hasta pronto!\n";
        }
        else {
            cout << "\n  [AVISO] Opcion no valida. Elige entre 0 y 9.\n";
            pausar();
        }

    } while (opcion != 0);

    return 0;
}
