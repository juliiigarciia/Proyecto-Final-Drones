// =============================================================
// CentroLogistico.cpp
// Implementacion de las funciones principales del sistema.
// Gestiona la flota (array de drones), la cola de espera
// (array de paquetes) y el historial de vuelos (array de registros).
// =============================================================

#include "CentroLogistico.hpp"
#include <iostream>
#include <string>

using namespace std;

// =============================================================
// Inicializacion
// =============================================================

// Inicializa el centro: pone contadores a 0 y prepara el grafo
void inicializarCentro(CentroLogistico& c, string nombre) {
    c.nombre      = nombre;
    c.numDrones   = 0;
    c.numCola     = 0;
    c.numVuelos   = 0;
    c.totalEnvios = 0;

    inicializarGrafo(c.grafo);

    cout << "\n*** Centro Logistico '" << nombre << "' iniciado ***\n";
}

// =============================================================
// Funciones internas de apoyo
// =============================================================

// Busca un dron por ID en la flota.
// Devuelve su indice o -1 si no existe.
static int buscarDronPorId(const CentroLogistico& c, string id) {
    for (int i = 0; i < c.numDrones; i++) {
        if (c.drones[i].id == id) return i;
    }
    return -1;
}

// Comprueba si ya existe un paquete con ese ID (en cola o en historial)
static bool existePaquete(const CentroLogistico& c, string id) {
    for (int i = 0; i < c.numCola; i++) {
        if (c.cola[i].id == id) return true;
    }
    for (int i = 0; i < c.numVuelos; i++) {
        if (c.historial[i].idPaquete == id) return true;
    }
    return false;
}

// Busca el primero dron DISPONIBLE del tipo indicado que pueda
// cargar el peso. Devuelve su indice o -1 si no hay ninguno.
static int buscarDronPorTipo(const CentroLogistico& c, double peso, TipoDron tipo) {
    for (int i = 0; i < c.numDrones; i++) {
        if (c.drones[i].tipo      == tipo       &&
            c.drones[i].estado    == DISPONIBLE  &&
            c.drones[i].cargaMaxima >= peso)
        {
            return i;
        }
    }
    return -1;
}

// Busca cualquier dron DISPONIBLE que pueda con el peso,
// sin importar el tipo. Se usa como alternativa si el ideal no esta.
static int buscarDronCualquiera(const CentroLogistico& c, double peso) {
    for (int i = 0; i < c.numDrones; i++) {
        if (c.drones[i].estado      == DISPONIBLE &&
            c.drones[i].cargaMaxima >= peso)
        {
            return i;
        }
    }
    return -1;
}

// Realiza el envio completo:
//   1. Calcula la ruta con Dijkstra
//   2. Calcula el consumo de bateria
//   3. Verifica que el dron tiene bateria suficiente
//   4. Descuenta la bateria y marca el dron en vuelo
//   5. Guarda el registro en el historial
//   6. Simula el regreso (dron vuelve a DISPONIBLE)
static void realizarEnvio(CentroLogistico& c, int idxDron, Paquete& p) {
    Drone& dron = c.drones[idxDron];

    ResultadoRuta ruta = calcularRutaOptima(c.grafo, p.destino);

    if (!ruta.encontrada) {
        cout << "[ERROR] No hay ruta hacia: " << p.destino << "\n";
        // Metemos el paquete en la cola de espera
        if (c.numCola < MAX_COLA) {
            c.cola[c.numCola] = p;
            c.numCola++;
        }
        return;
    }

    // Calculo del consumo de bateria para el viaje
    double consumoPorKm = calcularBateria(dron, p.peso);
    double kmNecesarios = ruta.kmTotales * consumoPorKm;

    // Verificamos que el dron tiene bateria suficiente
    if (dron.bateriaActual < kmNecesarios) {
        cout << "[ERROR] Dron " << dron.id << " sin bateria suficiente.\n"
             << "         Necesita " << kmNecesarios
             << " km, tiene " << dron.bateriaActual << " km.\n";
        // Metemos el paquete en cola
        if (c.numCola < MAX_COLA) {
            c.cola[c.numCola] = p;
            c.numCola++;
        }
        return;
    }

    // Descontamos bateria y marcamos el dron en vuelo
    dron.bateriaActual -= kmNecesarios;
    dron.estado         = EN_VUELO;
    p.idDronAsignado    = dron.id;

    // Construimos el texto de la ruta para el informe
    string rutaTexto = "";
    for (int i = 0; i < ruta.numNodos; i++) {
        if (i > 0) rutaTexto += " -> ";
        rutaTexto += ruta.nodos[i];
    }

    // Guardamos el registro del vuelo en el historial
    if (c.numVuelos < MAX_HISTORIAL) {
        RegistroVuelo vuelo;
        vuelo.fecha        = obtenerFechaHoraActual();
        vuelo.idDron       = dron.id;
        vuelo.tipoDron     = tipoDronATexto(dron.tipo);
        vuelo.idPaquete    = p.id;
        vuelo.ruta         = rutaTexto;
        vuelo.kmRecorridos = ruta.kmTotales;

        c.historial[c.numVuelos] = vuelo;
        c.numVuelos++;
    }

    c.totalEnvios++;

    cout << "[ENVIO] Dron "  << dron.id
         << " --> Paquete "  << p.id
         << " --> "          << p.destino
         << " | Ruta: "      << rutaTexto
         << " | "            << ruta.kmTotales << " km\n";

    // Simulamos que el dron regresa y queda disponible de nuevo
    dron.estado = DISPONIBLE;
}

// =============================================================
// Gestion de Drones
// =============================================================

// Agrega un dron a la flota.
// Falla si ya existe un dron con ese ID o si la flota esta llena.
bool agregarDron(CentroLogistico& c, Drone d) {
    if (buscarDronPorId(c, d.id) != -1) {
        cout << "[ERROR] Ya existe un dron con ID: " << d.id << "\n";
        return false;
    }
    if (c.numDrones >= MAX_DRONES) {
        cout << "[ERROR] Flota llena. No se puede agregar: " << d.id << "\n";
        return false;
    }

    c.drones[c.numDrones] = d;
    c.numDrones++;

    cout << "[FLOTA] Dron " << d.id
         << " (" << tipoDronATexto(d.tipo) << ") agregado a la flota.\n";
    return true;
}

// Elimina un dron por su ID.
// Desplaza el resto del array para no dejar huecos.
bool eliminarDron(CentroLogistico& c, string idDron) {
    int pos = buscarDronPorId(c, idDron);

    if (pos == -1) {
        cout << "[ERROR] No existe ningun dron con ID: " << idDron << "\n";
        return false;
    }

    cout << "[FLOTA] Eliminando dron " << c.drones[pos].id << "...\n";

    // Desplazamos los elementos para cerrar el hueco
    for (int i = pos; i < c.numDrones - 1; i++) {
        c.drones[i] = c.drones[i + 1];
    }
    c.numDrones--;

    return true;
}

// Muestra la informacion de todos los drones de la flota
void mostrarFlota(const CentroLogistico& c) {
    cout << "\n=== Flota de Drones (" << c.numDrones << " unidades) ===\n";

    if (c.numDrones == 0) {
        cout << "  (sin drones registrados)\n";
        return;
    }

    for (int i = 0; i < c.numDrones; i++) {
        mostrarDron(c.drones[i]);
    }
}

// =============================================================
// Gestion de Paquetes
// =============================================================

// Intenta asignar automaticamente un dron al paquete segun su peso:
//   < 5 kg   --> Express
//   5-15 kg  --> Standard
//   > 15 kg  --> HeavyDuty
// Si el tipo ideal no esta disponible, busca cualquier alternativa.
// Si no hay ningun dron libre, manda el paquete a la cola de espera.


void procesarCola(CentroLogistico& c) {
    if (c.numCola == 0) return;

    cout << "\n[COLA] Procesando paquetes en espera...\n";

    int i = 0;
    while (i < c.numCola) {
        Paquete p = c.cola[i];

        int idxDron = buscarDronCualquiera(c, p.peso);

        if (idxDron != -1) {
            cout << "[COLA] Intentando enviar paquete " << p.id << "\n";

            realizarEnvio(c, idxDron, p);

            // eliminar de la cola (shift)
            for (int j = i; j < c.numCola - 1; j++) {
                c.cola[j] = c.cola[j + 1];
            }
            c.numCola--;
        }
        else {
            i++; // pasar al siguiente
        }
    }
}



void registrarPaquete(CentroLogistico& c, Paquete p) {
    if (existePaquete(c, p.id)) {
        cout << "[ERROR] Ya existe un paquete con ID: " << p.id << "\n";
        return;
    }

    cout << "\n[PAQUETE] Registrando paquete " << p.id
         << " (Destino: " << p.destino
         << ", Peso: "    << p.peso << " kg)\n";

    double peso  = p.peso;
    int    idxDron = -1;

    // Asignacion por peso
    if (peso < 5.0) {
        cout << "[ASIGNACION] Paquete ligero (" << peso << " kg): buscando Express...\n";
        idxDron = buscarDronPorTipo(c, peso, EXPRESS);
        if (idxDron == -1) {
            cout << "[ASIGNACION] Express no disponible. Buscando alternativa...\n";
            idxDron = buscarDronCualquiera(c, peso);
        }
    } else if (peso <= 15.0) {
        cout << "[ASIGNACION] Paquete medio (" << peso << " kg): buscando Standard...\n";
        idxDron = buscarDronPorTipo(c, peso, STANDARD);
        if (idxDron == -1) {
            cout << "[ASIGNACION] Standard no disponible. Buscando alternativa...\n";
            idxDron = buscarDronCualquiera(c, peso);
        }
    } else {
        cout << "[ASIGNACION] Paquete pesado (" << peso << " kg): buscando HeavyDuty...\n";
        idxDron = buscarDronPorTipo(c, peso, HEAVY_DUTY);
        if (idxDron == -1) {
            cout << "[ASIGNACION] HeavyDuty no disponible. Buscando alternativa...\n";
            idxDron = buscarDronCualquiera(c, peso);
        }
    }

    if (idxDron != -1) {
        realizarEnvio(c, idxDron, p);
    } else {
        cout << "[COLA] Sin drones disponibles. Paquete " << p.id
             << " en cola de espera.\n";
        if (c.numCola < MAX_COLA) {
            c.cola[c.numCola] = p;
            c.numCola++;
        }
    }
    procesarCola(c);
}

// Muestra todos los paquetes pendientes de asignacion
void mostrarColaEspera(const CentroLogistico& c) {
    cout << "\n=== Cola de Espera (" << c.numCola << " paquetes) ===\n";

    if (c.numCola == 0) {
        cout << "  (sin paquetes en espera)\n";
        return;
    }

    for (int i = 0; i < c.numCola; i++) {
        mostrarPaquete(c.cola[i]);
    }
}

// =============================================================
// Rutas (sobrecargadas con string / coordenadas)
// =============================================================

// Version 1: recibe el destino como nombre de zona.
// Usa Dijkstra e imprime la ruta optima.
void asignarRuta(const CentroLogistico& c, string idDron, string destino) {
    cout << "\n[RUTA] Calculando ruta para dron " << idDron
         << " hacia '" << destino << "'...\n";

    ResultadoRuta ruta = calcularRutaOptima(c.grafo, destino);

    if (!ruta.encontrada) {
        cout << "[ERROR] No se encontro ruta hacia: " << destino << "\n";
        return;
    }

    // Imprimimos la ruta nodo por nodo
    cout << "  Ruta optima: ";
    for (int i = 0; i < ruta.numNodos; i++) {
        if (i > 0) cout << " --> ";
        cout << ruta.nodos[i];
    }
    cout << "\n  Distancia total: " << ruta.kmTotales << " km\n";
}

// Version 2: recibe coordenadas X e Y y las mapea a una zona:
//   coordY > 0  --> Zona Norte
//   coordX > 0  --> Zona Este
//   resto       --> Zona Sur
void asignarRutaCoordenadas(const CentroLogistico& c, string idDron,
                             float coordX, float coordY)
{
    cout << "\n[RUTA] Ruta por coordenadas para dron " << idDron
         << " --> (" << coordX << ", " << coordY << ")\n";

    string zona;
    if (coordY > 0) {
        zona = "Zona Norte";
    } else if (coordX > 0) {
        zona = "Zona Este";
    } else {
        zona = "Zona Sur";
    }

    cout << "  Coordenadas mapeadas a zona: " << zona << "\n";
    asignarRuta(c, idDron, zona);  // reutilizamos la version con string
}

// =============================================================
// Ficheros
// =============================================================

// Carga la flota desde un fichero de texto
void cargarFlotaDesdeFichero(CentroLogistico& c, string fichero) {
    cout << "\n[FICHERO] Cargando flota desde: " << fichero << "\n";
    cargarFlota(c.drones, c.numDrones, MAX_DRONES, fichero);
}

// Exporta el historial de vuelos del dia a un fichero de texto
void exportarInformeDia(const CentroLogistico& c, string fichero) {
    exportarInforme(c.historial, c.numVuelos, fichero);
}

// =============================================================
// Resumen
// =============================================================

void mostrarResumen(const CentroLogistico& c) {
    cout << "\n========== RESUMEN DEL DIA ==========\n";
    cout << "  Centro: "            << c.nombre       << "\n";
    cout << "  Drones en flota: "   << c.numDrones    << "\n";
    cout << "  Envios realizados: " << c.totalEnvios  << "\n";
    cout << "  Paquetes en espera: " << c.numCola     << "\n";
    cout << "=====================================\n";
}
