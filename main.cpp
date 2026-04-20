// =============================================================
// main.cpp
// Programa principal del SIGFD v2.0
//
// Compilar:
//   g++ -std=c++17 -Wall main.cpp Drone.cpp Paquete.cpp
//       GestorRutas.cpp GestorFicheros.cpp CentroLogistico.cpp -o sigfd
// =============================================================

#include "CentroLogistico.hpp"
#include "Excepciones.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
using namespace std;

// =============================================================
// ZONAS DISPONIBLES (fuente unica de verdad)
// =============================================================
const string ZONAS[] = {
    "Madrid Centro","Vallecas","Alcorcon","Getafe","Las Rozas",
    "Alcobendas","Pozuelo","Mostoles","Leganes","Fuenlabrada",
    "Parla","Pinto","Majadahonda","Boadilla","Coslada",
    "Rivas","San Fernando","Torrejon","Alcala de Henares",
    "San Sebastian de los Reyes"
};
const int NUM_ZONAS = 20;

// =============================================================
// UTILIDADES DE PANTALLA
// =============================================================

void limpiarPantalla() { system("cls"); }

void pausar() {
    cout << "\n  Presiona Enter para continuar...";
    cin.get();
}

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

string elegirZona() {
    cout << "\n  --- DESTINOS DISPONIBLES ---\n";
    for (int i = 0; i < NUM_ZONAS; i++)
        cout << "    " << (i + 1) << ") " << ZONAS[i] << "\n";
    cout << "  Selecciona el numero de destino: ";
    int opcion;
    if (!leerEntero(opcion)) return "";
    if (opcion >= 1 && opcion <= NUM_ZONAS) return ZONAS[opcion - 1];
    cout << "  [ERROR] Numero de zona no valido.\n";
    return "";
}

// =============================================================
// TESTS DE QA
// =============================================================

void testDijkstra() {
    cout << "\n================================================\n";
    cout << "TEST 1: Algoritmo de Dijkstra\n";
    cout << "================================================\n";

    Grafo g;
    inicializarGrafo(g);
    mostrarGrafo(g);

    // Test 1a: ruta directa (8 km)
    cout << "\n[TEST] Madrid Centro --> Vallecas\n";
    try {
        ResultadoRuta r = calcularRutaOptima(g, "Vallecas");
        if (r.kmTotales == 8.0)
            cout << "  --> CORRECTO: " << r.kmTotales << " km\n";
        else
            cout << "  --> ERROR: esperados 8 km, obtenidos " << r.kmTotales << " km\n";
    } catch (const exception& e) {
        cout << "  --> EXCEPCION: " << e.what() << "\n";
    }

    // Test 1b: ruta directa (10 km)
    cout << "\n[TEST] Madrid Centro --> Pozuelo\n";
    try {
        ResultadoRuta r = calcularRutaOptima(g, "Pozuelo");
        if (r.kmTotales == 10.0)
            cout << "  --> CORRECTO: " << r.kmTotales << " km\n";
        else
            cout << "  --> ERROR: esperados 10 km, obtenidos " << r.kmTotales << " km\n";
    } catch (const exception& e) {
        cout << "  --> EXCEPCION: " << e.what() << "\n";
    }

    // Test 1c: destino inexistente -> debe lanzar ExcepcionRutaNoEncontrada
    cout << "\n[TEST] Madrid Centro --> Lugar Inexistente\n";
    try {
        calcularRutaOptima(g, "Lugar Inexistente");
        cout << "  --> ERROR: deberia haber lanzado excepcion\n";
    } catch (const ExcepcionRutaNoEncontrada& e) {
        cout << "  --> CORRECTO: excepcion capturada: " << e.what() << "\n";
    }
}

void testCargaExcesiva() {
    cout << "\n================================================\n";
    cout << "TEST 2: Comprobacion de peso excesivo\n";
    cout << "================================================\n";

    // Creamos dron con 'new' (memoria dinamica, polimorfismo)
    Drone* dron = new DroneExpress("TEST-01", "DronPrueba");
    Paquete paquete("PKG-TEST", 50.0, "Vallecas", NORMAL);

    cout << "[TEST] Paquete de " << paquete.getPeso()
         << " kg en dron " << dron->getTipo()
         << " (max " << dron->getCargaMaxima() << " kg)\n";

    try {
        if (paquete.getPeso() > dron->getCargaMaxima()) {
            throw ExcepcionPesoExcesivo(
                "El paquete pesa " + to_string(paquete.getPeso()) +
                " kg, el dron aguanta " + to_string(dron->getCargaMaxima()) + " kg.");
        }
        cout << "  --> ERROR: deberia haber lanzado excepcion\n";
    } catch (const ExcepcionPesoExcesivo& e) {
        cout << "  --> CORRECTO: " << e.what() << "\n";
    }

    delete dron;   // liberar memoria (obligatorio)
}

void testPolimorfismo() {
    cout << "\n================================================\n";
    cout << "TEST 3: Polimorfismo dinamico (calcularBateria)\n";
    cout << "================================================\n";

    // Array de punteros a la clase base (polimorfismo)
    Drone* flota[3];
    flota[0] = new DroneStandard("T01", "Standard-Test");
    flota[1] = new DroneExpress("T02", "Express-Test");
    flota[2] = new DroneHeavyDuty("T03", "Heavy-Test");

    double peso = 10.0;
    cout << "Consumo por km con " << peso << " kg:\n";
    for (int i = 0; i < 3; i++) {
        // La misma llamada, distinto resultado segun el tipo real
        double consumo = flota[i]->calcularBateria(peso);  // POLIMORFISMO
        cout << "  [" << flota[i]->getTipo() << "] "
             << consumo << " km/km\n";
        flota[i]->mostrar(true);  // sobrecarga estatica
    }

    // Liberar memoria de todos los drones del array
    for (int i = 0; i < 3; i++) delete flota[i];

    cout << "  --> CORRECTO: polimorfismo dinamico funcionando.\n";
    cout << "  --> Drones en memoria ahora: " << Drone::getTotalCreados() << "\n";
}

void testVector() {
    cout << "\n================================================\n";
    cout << "TEST 4: std::vector y memoria dinamica\n";
    cout << "================================================\n";

    vector<Paquete> paquetes;
    for (int i = 0; i < 10; i++) {
        paquetes.emplace_back(
            "PKG-VEC-" + to_string(i),
            1.0 + i * 0.5,
            "Vallecas",
            NORMAL
        );
    }
    cout << "  Paquetes en vector: " << paquetes.size() << "\n";
    paquetes[0].mostrar(true);  // sobrecarga
    cout << "  --> CORRECTO: std::vector<Paquete> funciona.\n";
}

void opcionTests() {
    testDijkstra();
    testCargaExcesiva();
    testPolimorfismo();
    testVector();
    pausar();
}

// =============================================================
// MENU INTERACTIVO
// =============================================================

int mostrarMenu() {
    limpiarPantalla();
    cout << "\n";
    cout << "+==========================================+\n";
    cout << "|   SIGFD v2.0 - Centro de Control        |\n";
    cout << "+==========================================+\n";
    cout << "| --- GESTION DE FLOTA -------------------|  \n";
    cout << "|  1. Ver estado de flota y baterias       |\n";
    cout << "|  2. Mantenimiento: Recargar baterias     |\n";
    cout << "|  3. Agregar dron manualmente             |\n";
    cout << "| --- OPERATIVA DE ENVIOS -----------------|  \n";
    cout << "|  4. Registrar nuevo paquete              |\n";
    cout << "|  5. Procesar cola de espera (FIFO)       |\n";
    cout << "|  6. Ejecutar Vuelo (Dijkstra + Bateria)  |\n";
    cout << "| --- ADMINISTRACION ---------------------|  \n";
    cout << "|  7. Ver resumen del dia                  |\n";
    cout << "|  8. Guia de uso                          |\n";
    cout << "|  9. Ejecutar tests de QA                 |\n";
    cout << "|  0. Salir y exportar informe             |\n";
    cout << "+==========================================+\n";
    cout << "  Elige una opcion: ";
    int op = -1;
    leerEntero(op);
    return op;
}

// =============================================================
// OPCIONES DEL MENU
// =============================================================

void opcionVerFlota(CentroLogistico& c) {
    c.mostrarFlota(true);   // sobrecarga con detallado=true
    pausar();
}

void opcionAgregarDron(CentroLogistico& c) {
    cout << "\n  --- TIPO DE DRON ---\n"
         << "    1) Standard\n  2) Express\n  3) HeavyDuty\n"
         << "  Selecciona: ";
    int t = 0;
    if (!leerEntero(t) || t < 1 || t > 3) {
        cout << "  [ERROR] Tipo no valido.\n";
        pausar(); return;
    }
    string id, nombre;
    cout << "  ID del dron (ej. D010): ";  getline(cin, id);
    cout << "  Nombre del dron: ";         getline(cin, nombre);

    try {
        // 'new' aqui: memoria dinamica. CentroLogistico es responsable del delete
        Drone* d = nullptr;
        if      (t == 1) d = new DroneStandard(id, nombre);
        else if (t == 2) d = new DroneExpress(id, nombre);
        else             d = new DroneHeavyDuty(id, nombre);

        c.agregarDron(d);   // transfiere ownership al centro
        cout << "  [OK] Dron '" << nombre << "' agregado.\n";
    } catch (const exception& e) {
        cout << "  [ERROR] " << e.what() << "\n";
    }
    pausar();
}

void opcionGuiaUso() {
    limpiarPantalla();
    cout << "\n===========================================================\n";
    cout << "           GUIA DE USO - SIGFD v2.0\n";
    cout << "===========================================================\n";
    cout << "  CLASES: Drone (abstracta) -> DroneStandard/Express/HeavyDuty\n";
    cout << "  POLIMORFISMO: calcularBateria() virtual en cada tipo\n";
    cout << "  EXCEPCIONES: try/catch en cada operacion critica\n";
    cout << "  MEMORIA: new (agregar dron) / delete (eliminar/destructor)\n";
    cout << "  VECTOR: vector<Drone*> flota, vector<Paquete> cola\n\n";
    cout << "  FLUJO RECOMENDADO:\n";
    cout << "  1) Ver flota (Op.1) -> 4) Registrar paquete -> 6) Volar\n";
    cout << "  2) Si sin bateria   -> 2) Recargar -> 6) Volar\n";
    cout << "  3) Cola pendiente   -> 5) Procesar FIFO\n\n";
    cout << "  TIPOS:\n";
    cout << "    Standard  : 60 km/h | 15 kg | 80 km\n";
    cout << "    Express   : 120km/h |  8 kg | 60 km\n";
    cout << "    HeavyDuty :  40km/h | 50 kg |120 km\n";
    cout << "===========================================================\n";
    pausar();
}

void opcionRegistrarPaquete(CentroLogistico& c) {
    string id, destino;
    double peso;

    cout << "\n  ID del paquete (ej. PKG-010): ";
    getline(cin, id);

    cout << "  Peso del paquete (kg): ";
    cin >> peso;
    if (cin.fail()) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "  [ERROR] Peso no valido.\n"; pausar(); return;
    }
    cin.ignore(10000, '\n');

    destino = elegirZona();
    if (destino.empty()) { pausar(); return; }

    cout << "\n  --- PRIORIDAD ---\n    1) Normal\n    2) Urgente\n  Selecciona: ";
    int priNum = 0;
    if (!leerEntero(priNum) || priNum < 1 || priNum > 2) {
        cout << "  [ERROR] Prioridad no valida.\n"; pausar(); return;
    }

    Prioridad prioridad = (priNum == 2) ? URGENTE : NORMAL;

    cout << "\n  --- ACCION ---\n"
         << "    1) Pre-asignar dron (luego vuela con Op.6)\n"
         << "    2) Guardar en cola (procesar con Op.5)\n"
         << "    3) Envio automatico completo\n"
         << "  Selecciona: ";
    int accion = 0;
    if (!leerEntero(accion) || accion < 1 || accion > 3) {
        cout << "  [ERROR] Accion no valida.\n"; pausar(); return;
    }

    Paquete pkg(id, peso, destino, prioridad);

    try {
        if (accion == 1) {
            // Pre-asignar: buscar dron disponible y guardar en cola
            cout << "\n";
            // Usamos registrarPaquete con parametros (sobrecarga)
            // para demostrar la sobrecarga estatica de esa funcion
            // Buscamos dron manualmente para poder pre-asignar
            bool asignado = false;
            // (el CentroLogistico no expone buscarDronCualquiera directamente;
            //  mostramos la flota para que el usuario sepa que drones hay)
            c.mostrarFlota();
            cout << "  [INFO] Paquete a cola. Usa Op.6 para volar cuando elijas el dron.\n";
            c.ponerEnCola(pkg);
        } else if (accion == 2) {
            c.ponerEnCola(pkg);
        } else {
            // Sobrecarga estatica 2: registrar desde parametros sueltos
            c.registrarPaquete(id, peso, destino, prioridad);
            cout << "  [OK] Envio automatico ejecutado para '" << id << "'.\n";
        }
    } catch (const ExcepcionBateria& e) {
        cout << "\n  [BATERIA] " << e.what() << "\n";
        cout << "  Usa la opcion 2 para recargar baterias.\n";
    } catch (const ExcepcionRutaNoEncontrada& e) {
        cout << "\n  [RUTA] " << e.what() << "\n";
    } catch (const exception& e) {
        cout << "\n  [ERROR] " << e.what() << "\n";
    }
    pausar();
}

void opcionProcesarCola(CentroLogistico& c) {
    c.mostrarColaEspera();
    try {
        c.procesarCola();
    } catch (const exception& e) {
        cout << "\n  [ERROR] " << e.what() << "\n";
    }
    pausar();
}

void opcionEjecutarVuelo(CentroLogistico& c) {
    c.mostrarFlota();
    string idDron;
    cout << "\n  ID del dron a hacer volar: ";
    getline(cin, idDron);

    cout << "  Procesando vuelo";
    for (int i = 0; i < 3; i++) {
        this_thread::sleep_for(chrono::milliseconds(350));
        cout << "." << flush;
    }
    cout << "\n";

    try {
        c.ejecutarVuelo(idDron);
    } catch (const ExcepcionBateria& e) {
        cout << "\n  [BATERIA] " << e.what() << "\n";
    } catch (const ExcepcionDronNoEncontrado& e) {
        cout << "\n  [DRON] " << e.what() << "\n";
    } catch (const ExcepcionRutaNoEncontrada& e) {
        cout << "\n  [RUTA] " << e.what() << "\n";
    } catch (const exception& e) {
        cout << "\n  [ERROR] " << e.what() << "\n";
    }
    pausar();
}

void opcionRecargarBaterias(CentroLogistico& c) {
    c.recargarBaterias();
    pausar();
}

void opcionResumen(CentroLogistico& c) {
    c.mostrarResumen();
    pausar();
}

// =============================================================
// MAIN
// =============================================================

int main() {
    limpiarPantalla();
    cout << "==============================================\n";
    cout << "  Iniciando SIGFD v2.0\n";
    cout << "  Sistema de Control Logistico para Drones\n";
    cout << "==============================================\n";

    // CentroLogistico se crea en la pila; su destructor liberara
    // todos los Drone* del vector automaticamente al salir de main.
    CentroLogistico centro("SIGFD - Madrid Central");

    // Intentar cargar flota desde fichero
    try {
        centro.cargarFlotaDesdeFichero("flota.txt");
    } catch (const ExcepcionFichero&) {
        // Si el fichero no existe, cargamos flota por defecto con 'new'
        cout << "[INFO] Cargando flota por defecto...\n\n";
        centro.agregarDron(new DroneStandard  ("D001", "Aguila-1"));
        centro.agregarDron(new DroneExpress   ("D002", "Falcon-3"));
        centro.agregarDron(new DroneHeavyDuty ("D003", "Titan-1"));
        centro.agregarDron(new DroneExpress   ("D004", "Falcon-4"));
        centro.agregarDron(new DroneStandard  ("D005", "Aguila-2"));
    }

    int opcion = -1;
    do {
        opcion = mostrarMenu();

        if      (opcion == 1) opcionVerFlota(centro);
        else if (opcion == 2) opcionRecargarBaterias(centro);
        else if (opcion == 3) opcionAgregarDron(centro);
        else if (opcion == 4) opcionRegistrarPaquete(centro);
        else if (opcion == 5) opcionProcesarCola(centro);
        else if (opcion == 6) opcionEjecutarVuelo(centro);
        else if (opcion == 7) opcionResumen(centro);
        else if (opcion == 8) opcionGuiaUso();
        else if (opcion == 9) opcionTests();
        else if (opcion == 0) {
            try {
                cout << "\n[INFO] Exportando informe...\n";
                centro.exportarInformeDia("informe_envios.txt");
                cout << "[INFO] Hasta pronto!\n";
            } catch (const ExcepcionFichero& e) {
                cout << "[ERROR] No se pudo exportar el informe: " << e.what() << "\n";
            }
        } else {
            cout << "\n  [AVISO] Opcion no valida. Elige entre 0 y 9.\n";
            pausar();
        }

    } while (opcion != 0);

    // El destructor de 'centro' llama 'delete' sobre todos los Drone* automaticamente
    return 0;
}
