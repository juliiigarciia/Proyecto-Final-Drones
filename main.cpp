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
    cout << "|   SIGFD - Centro de Control Logistico    |\n";
    cout << "+==========================================+\n";
    cout << "| --- GESTION DE FLOTA -------------------|\n";
    cout << "|  1. Ver estado de flota y baterias       |\n";
    cout << "|  2. Mantenimiento: Recargar baterias     |\n";
    cout << "|  3. Agregar dron manualmente             |\n";
    cout << "| --- OPERATIVA DE ENVIOS -----------------|\n";
    cout << "|  4. Registrar nuevo paquete              |\n";
    cout << "|  5. Procesar cola de espera (FIFO)       |\n";
    cout << "|  6. Ejecutar Vuelo (Dijkstra + Bateria)  |\n";
    cout << "| --- ADMINISTRACION ---------------------|\n";
    cout << "|  7. Ver resumen del dia                  |\n";
    cout << "|  8. Guia de uso y funcionamiento         |\n";
    cout << "|  9. Ejecutar tests basicos (QA)          |\n";
    cout << "|  0. Salir y exportar informe             |\n";
    cout << "+==========================================+\n";
    cout << "  Elige una opcion: ";

    int opcion = -1;
    leerEntero(opcion);
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
    cout << "  FLUJO DE TRABAJO RECOMENDADO:\n";
    cout << "  1) Revisar flota     -> Opcion 1 (ver baterias y estados)\n";
    cout << "  2) Registrar paquete -> Opcion 4 (envia ahora o a la cola)\n";
    cout << "  3) Ejecutar vuelo    -> Opcion 6 (Dijkstra + bateria real)\n";
    cout << "  4) Si sin bateria    -> Opcion 2 (recargar y volver al 3)\n";
    cout << "  5) Cola pendiente    -> Opcion 5 (procesar paquetes FIFO)\n\n";
    cout << "  TIPOS DE DRON:\n";
    cout << "    Standard  : 60 km/h | max 15 kg | autonomia  80 km\n";
    cout << "    Express   : 120km/h | max  8 kg | autonomia  60 km\n";
    cout << "    HeavyDuty : 40 km/h | max 50 kg | autonomia 120 km\n\n";
    cout << "  REGLA DE BATERIA:\n";
    cout << "    consumoPorKm = base_tipo + (peso_kg * factor_tipo)\n";
    cout << "    kmGastados   = kmRuta * consumoPorKm\n";
    cout << "    Si bateriaActual < kmGastados --> vuelo CANCELADO\n\n";
    cout << "  COLA DE ESPERA (FIFO):\n";
    cout << "    Los paquetes se guardan en un array. Al procesar,\n";
    cout << "    se saca el primero (indice 0) y el resto se\n";
    cout << "    desplaza una posicion (equivale a std::queue).\n\n";
    cout << "  ALGORITMO DE DIJKSTRA:\n";
    cout << "    Recorre el grafo de Madrid (nodos = zonas, aristas = km).\n";
    cout << "    Siempre elige el nodo mas cercano no visitado.\n";
    cout << "    Resultado: ruta optima en km desde Madrid Centro.\n";
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
    // Opcion 1: el sistema busca dron, lo pre-asigna en cola, y usas Opcion 6 para volar.
    // Opcion 2: va directo a la cola sin asignar dron (para procesar con Opcion 5).
    // Opcion 3: asignacion y vuelo completo automatico (comportamiento clasico).
    cout << "    1) Pre-asignar dron (luego vuela con Opcion 6)\n";
    cout << "    2) Guardar en cola de espera (procesar con Opcion 5)\n";
    cout << "    3) Envio completo automatico (asignar + volar ahora)\n";
    cout << "  Selecciona: ";

    int accion = 0;
    if (!leerEntero(accion) || accion < 1 || accion > 3) {
        cout << "  [ERROR] Accion no valida. Cancelando registro.\n";
        pausar();
        return;
    }

    cout << "\n";
    if (accion == 1) {
        // Pre-asignamos un dron buscando en el array de drones del centro.
        // Buscamos el primero DISPONIBLE que soporte el peso del paquete.
        // El campo idDronAsignado guarda el ID para que opcion 6 lo encuentre.
        int idxDron = -1;
        for (int i = 0; i < centro.numDrones && idxDron == -1; i++) {
            if (centro.drones[i].estado == DISPONIBLE &&
                centro.drones[i].cargaMaxima >= peso) {
                idxDron = i;
            }
        }

        if (idxDron == -1) {
            cout << "  [AVISO] No hay dron disponible ahora. Paquete a la cola sin asignar.\n";
            ponerEnCola(centro, pkg);
        } else {
            pkg.idDronAsignado = centro.drones[idxDron].id;
            ponerEnCola(centro, pkg);
            cout << "  [OK] Paquete '" << id << "' pre-asignado al dron '"
                 << pkg.idDronAsignado << "'.\n"
                 << "       Usa la Opcion 6 para ejecutar el vuelo.\n";
        }
    } else if (accion == 2) {
        ponerEnCola(centro, pkg);
    } else {
        // Opcion 3: flujo completo automatico (como antes)
        registrarPaquete(centro, pkg);
        cout << "  [OK] Envio automatico ejecutado para '" << id << "'.\n";
    }

    pausar();
}

void opcionVerCola(CentroLogistico& centro) {
    mostrarColaEspera(centro);
    pausar();
}

// Opcion 5: Extrae y procesa el primer paquete de la cola (FIFO).
// Si hay dron libre con bateria, lo envia. Si no, lo reencola.
void opcionProcesarCola(CentroLogistico& centro) {
    mostrarColaEspera(centro);
    procesarCola(centro);
    pausar();
}

// Opcion 6: El operador elige un dron con paquete asignado.
// Se calcula la ruta con Dijkstra, se comprueba la bateria y
// se ejecuta el vuelo (o se cancela con mensaje claro).
void opcionEjecutarVuelo(CentroLogistico& centro) {
    // Mostramos la flota para que el operador sepa que drones existen
    mostrarFlota(centro);

    string idDron;
    cout << "\n  ID del dron a hacer volar: ";
    getline(cin, idDron);

    // Efecto visual de calculo
    cout << "  Procesando vuelo";
    for (int i = 0; i < 3; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(350));
        cout << "." << flush;
    }
    cout << "\n";

    ejecutarVuelo(centro, idDron);
    pausar();
}

// Opcion 2: Recarga la bateria de todos los drones disponibles al 100%
void opcionRecargarBaterias(CentroLogistico& centro) {
    recargarBaterias(centro);
    pausar();
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

        // ---- GESTION DE FLOTA ----
        if      (opcion == 1) opcionVerFlota(centro);          // ver drones + baterias
        else if (opcion == 2) opcionRecargarBaterias(centro);  // recarga al 100%
        else if (opcion == 3) opcionAgregarDron(centro);       // agregar dron manual
        // ---- OPERATIVA DE ENVIOS ----
        else if (opcion == 4) opcionRegistrarPaquete(centro);  // registrar + asignar
        else if (opcion == 5) opcionProcesarCola(centro);      // FIFO: saca y envia
        else if (opcion == 6) opcionEjecutarVuelo(centro);     // Dijkstra + bateria
        // ---- ADMINISTRACION ----
        else if (opcion == 7) opcionResumen(centro);           // resumen del dia
        else if (opcion == 8) opcionGuiaUso();                 // guia actualizada
        else if (opcion == 9) opcionTests();                   // tests QA
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
