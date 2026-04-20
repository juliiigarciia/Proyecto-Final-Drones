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
}

// Añade el paquete directamente a la cola de espera
void ponerEnCola(CentroLogistico& c, Paquete p) {
    if (existePaquete(c, p.id)) {
        cout << "[ERROR] Ya existe un paquete con ID: " << p.id << "\n";
        return;
    }

    if (c.numCola < MAX_COLA) {
        c.cola[c.numCola] = p;
        c.numCola++;
        cout << "[COLA] Paquete " << p.id << " añadido a la espera.\n";
    } else {
        cout << "[ERROR] Cola de espera llena.\n";
    }
}

// Muestra todos los paquetes pendientes de asignacion
void mostrarColaEspera(const CentroLogistico& c) {
    cout << "\n=== Cola de Espera (" << c.numCola << " paquetes) ===\n";

    if (c.numCola == 0) {
        cout << "  (sin paquetes en espera)\n";
        return;
    }

    for (int i = 0; i < c.numCola; i++) {
        cout << "  [" << (i + 1) << "] ";
        mostrarPaquete(c.cola[i]);
    }
}

// =============================================================
// Procesar Cola (logica FIFO)
// =============================================================

// CONCEPTO FIFO (First In, First Out): el primer paquete que entro
// es el primero en salir, igual que una cola real en correos.
// Implementacion: accedemos a cola[0] y luego desplazamos todo el
// array una posicion hacia atras (lo mismo que hace std::queue).
//
// Flujo:
//   1. Si la cola esta vacia, avisamos y salimos.
//   2. Sacamos el primer paquete (indice 0).
//   3. Buscamos un dron disponible que soporte el peso.
//   4a. Si hay dron: llamamos a realizarEnvio (ya gestiona bateria).
//   4b. Si no hay dron: volvemos a meter el paquete al final de la cola.
void procesarCola(CentroLogistico& c) {
    // Verificamos si hay algo en la cola
    if (c.numCola == 0) {
        cout << "\n[COLA] La cola de espera esta vacia. No hay nada que procesar.\n";
        return;
    }

    // Extraemos el primer paquete (FIFO: front)
    Paquete paquete = c.cola[0];

    // Desplazamos el array una posicion hacia atras para cerrar el hueco
    // Esto es O(n) pero con arrays estaticos es la unica opcion sin punteros
    for (int i = 0; i < c.numCola - 1; i++) {
        c.cola[i] = c.cola[i + 1];
    }
    c.numCola--;

    cout << "\n[COLA] Procesando paquete '" << paquete.id
         << "' (" << paquete.peso << " kg) -> " << paquete.destino << "\n";

    // Buscamos un dron disponible adecuado al peso
    int idxDron = -1;
    if (paquete.peso < 5.0) {
        idxDron = buscarDronPorTipo(c, paquete.peso, EXPRESS);
        if (idxDron == -1) idxDron = buscarDronCualquiera(c, paquete.peso);
    } else if (paquete.peso <= 15.0) {
        idxDron = buscarDronPorTipo(c, paquete.peso, STANDARD);
        if (idxDron == -1) idxDron = buscarDronCualquiera(c, paquete.peso);
    } else {
        idxDron = buscarDronPorTipo(c, paquete.peso, HEAVY_DUTY);
        if (idxDron == -1) idxDron = buscarDronCualquiera(c, paquete.peso);
    }

    if (idxDron != -1) {
        // Hay dron disponible: intentamos el envio (puede fallar por bateria)
        cout << "[COLA] Dron encontrado: " << c.drones[idxDron].id
             << ". Ejecutando envio...\n";
        realizarEnvio(c, idxDron, paquete);
    } else {
        // No hay dron: devolvemos el paquete al final de la cola
        cout << "[COLA] Sin drones disponibles. Paquete '" << paquete.id
             << "' devuelto al final de la cola.\n";
        if (c.numCola < MAX_COLA) {
            c.cola[c.numCola] = paquete;
            c.numCola++;
        }
    }
}

// =============================================================
// Ejecutar Vuelo (logica de bateria completa)
// =============================================================

// FLUJO COMPLETO de un vuelo:
//   1. El operador elige que dron quiere hacer volar.
//   2. Verificamos que el dron tiene un paquete pre-asignado
//      (idDronAsignado coincide con el ID del dron).
//   3. Llamamos a Dijkstra para obtener los km reales de la ruta.
//   4. Calculamos el gasto de bateria con la formula:
//        consumoPorKm = calcularBateria(dron, peso_paquete)
//        kmGastados   = kmRuta * consumoPorKm
//      Esto penaliza drones cargados con paquetes pesados.
//   5. Si bateria >= gasto: vuelo OK, se descuenta la bateria.
//      Si bateria < gasto:  vuelo cancelado, se recomienda recargar.
void ejecutarVuelo(CentroLogistico& c, string idDron) {
    // Buscamos el dron en la flota (buscarDronPorId devuelve indice o -1)
    int idxDron = buscarDronPorId(c, idDron);
    if (idxDron == -1) {
        cout << "[ERROR] No existe ningun dron con ID '" << idDron << "'.\n";
        return;
    }

    Drone& dron = c.drones[idxDron];

    // Buscamos en la cola un paquete asignado a este dron
    // Un paquete asignado tiene su campo idDronAsignado con el ID del dron
    int idxPkg = -1;
    for (int i = 0; i < c.numCola; i++) {
        if (c.cola[i].idDronAsignado == idDron) {
            idxPkg = i;
            break;
        }
    }

    if (idxPkg == -1) {
        cout << "[ERROR] El dron '" << idDron
             << "' no tiene ningun paquete asignado.\n"
             << "        Usa la opcion 4 para registrar un paquete y asignarlo.\n";
        return;
    }

    // Extraemos el paquete asignado
    Paquete paquete = c.cola[idxPkg];

    // Quitamos el paquete de la cola desplazando el array
    for (int i = idxPkg; i < c.numCola - 1; i++) {
        c.cola[i] = c.cola[i + 1];
    }
    c.numCola--;

    // Calculamos la ruta con Dijkstra
    cout << "\n[VUELO] Calculando ruta para dron '" << dron.id
         << "' -> '" << paquete.destino << "'...\n";
    ResultadoRuta ruta = calcularRutaOptima(c.grafo, paquete.destino);

    if (!ruta.encontrada) {
        cout << "[ERROR] No se encontro ruta hacia: " << paquete.destino
             << ". Paquete devuelto a la cola.\n";
        // Devolvemos el paquete a la cola
        if (c.numCola < MAX_COLA) {
            c.cola[c.numCola] = paquete;
            c.numCola++;
        }
        return;
    }

    // Calculamos el consumo real de bateria
    // consumoPorKm > 1.0 siempre: el dron gasta mas km de bateria
    // de los que vuela fisicamente (por el peso, viento, etc.)
    double consumoPorKm = calcularBateria(dron, paquete.peso);
    double kmGastados   = ruta.kmTotales * consumoPorKm;

    cout << "  Ruta: ";
    for (int i = 0; i < ruta.numNodos; i++) {
        if (i > 0) cout << " -> ";
        cout << ruta.nodos[i];
    }
    cout << "\n  Distancia: " << ruta.kmTotales << " km";
    cout << "\n  Consumo estimado: " << kmGastados << " km de bateria";
    cout << "\n  Bateria actual del dron: " << dron.bateriaActual << " km\n";

    // Verificamos si el dron tiene bateria suficiente
    if (dron.bateriaActual < kmGastados) {
        cout << "\n[CANCELADO] Bateria insuficiente.\n"
             << "  Necesita: " << kmGastados << " km | Tiene: "
             << dron.bateriaActual << " km\n"
             << "  --> Usa la opcion 2 (Recargar baterias) y vuelve a intentarlo.\n";
        // Devolvemos el paquete a la cola
        paquete.idDronAsignado = idDron;  // mantenemos la asignacion
        if (c.numCola < MAX_COLA) {
            c.cola[c.numCola] = paquete;
            c.numCola++;
        }
        return;
    }

    // Todo OK: descontamos bateria, registramos vuelo, marcamos entregado
    dron.bateriaActual -= kmGastados;
    dron.estado = EN_VUELO;  // marcamos en vuelo momentaneamente

    string rutaTexto = "";
    for (int i = 0; i < ruta.numNodos; i++) {
        if (i > 0) rutaTexto += " -> ";
        rutaTexto += ruta.nodos[i];
    }

    if (c.numVuelos < MAX_HISTORIAL) {
        RegistroVuelo vuelo;
        vuelo.fecha        = obtenerFechaHoraActual();
        vuelo.idDron       = dron.id;
        vuelo.tipoDron     = tipoDronATexto(dron.tipo);
        vuelo.idPaquete    = paquete.id;
        vuelo.ruta         = rutaTexto;
        vuelo.kmRecorridos = ruta.kmTotales;
        c.historial[c.numVuelos] = vuelo;
        c.numVuelos++;
    }
    c.totalEnvios++;

    // El dron regresa al deposito y queda disponible
    dron.estado = DISPONIBLE;

    cout << "\n[OK] VUELO EXITOSO!\n"
         << "  Dron    : " << dron.id << " (" << tipoDronATexto(dron.tipo) << ")\n"
         << "  Paquete : " << paquete.id << " (" << paquete.peso << " kg)\n"
         << "  Entrega : " << paquete.destino << "\n"
         << "  Bateria restante: " << dron.bateriaActual << " km de "
         << dron.autonomia << " km\n";
}

// =============================================================
// Mantenimiento: Recargar Baterias
// =============================================================

// Recarga la bateria de TODOS los drones al maximo (su autonomia).
// Esto simula una noche de carga o una parada en deposito.
// La autonomia es la capacidad maxima; bateriaActual es la carga real.
void recargarBaterias(CentroLogistico& c) {
    cout << "\n[MANT] Iniciando recarga de baterias...\n";
    for (int i = 0; i < c.numDrones; i++) {
        // Solo recargamos drones que no esten en vuelo activo
        if (c.drones[i].estado != EN_VUELO) {
            double anterior = c.drones[i].bateriaActual;
            c.drones[i].bateriaActual = c.drones[i].autonomia;  // 100%
            cout << "  [" << c.drones[i].id << "] " << c.drones[i].nombre
                 << ": " << anterior << " km -> " << c.drones[i].autonomia
                 << " km (RECARGADO)\n";
        } else {
            cout << "  [" << c.drones[i].id << "] " << c.drones[i].nombre
                 << ": EN VUELO - no se puede recargar ahora.\n";
        }
    }
    cout << "[MANT] Recarga completada. Todos los drones listos.\n";
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
