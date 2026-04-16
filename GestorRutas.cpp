// =============================================================
// GestorRutas.cpp
// Mariano
// Implementacion del grafo de zonas y el algoritmo de Dijkstra.
// Sin map/set: usamos arrays con contador y indices enteros.
//
// COMO FUNCIONA DIJKSTRA:
//   1. Empieza en el origen con distancia 0.
//   2. Asigna distancia "infinita" al resto de nodos.
//   3. Visita el nodo no visitado mas cercano.
//   4. Actualiza distancias de sus vecinos si encuentra mejor camino.
//   5. Repite hasta llegar al destino.
// =============================================================

#include "GestorRutas.hpp"
#include <iostream>
#include <string>

using namespace std;

// Valor que representa "distancia infinita" en Dijkstra
const double INF = 1e18;

// =============================================================
// Funciones internas de apoyo (solo visibles en este fichero)
// =============================================================

// Busca un nodo por nombre en el grafo.
// Devuelve su indice, o -1 si no existe.
int buscarNodo(const Grafo& g, string nombre) {
    for (int i = 0; i < g.numNodos; i++) {
        if (g.nodos[i].nombre == nombre) return i;
    }
    return -1;
}

// Busca un nodo por nombre. Si no existe, lo crea y devuelve su indice.
static int buscarOCrearNodo(Grafo& g, string nombre) {
    int pos = buscarNodo(g, nombre);
    if (pos != -1) return pos;

    // El nodo no existe: lo creamos
    if (g.numNodos >= MAX_NODOS) {
        cout << "[ERROR] Grafo lleno. No se puede anadir: " << nombre << "\n";
        return -1;
    }

    g.nodos[g.numNodos].nombre     = nombre;
    g.nodos[g.numNodos].numVecinos = 0;
    g.numNodos++;

    return g.numNodos - 1;
}

// =============================================================
// Inicializacion
// =============================================================

// Crea el grafo con las zonas y conexiones del sistema SIGFD
void inicializarGrafo(Grafo& g) {
    g.numNodos = 0;

    // Conexiones predefinidas del sistema:
    //   Almacen Central --> Zona Norte (5 km)  -- ruta directa
    //   Almacen Central --> Zona Este  (3 km)
    //   Almacen Central --> Zona Sur   (8 km)
    //   Zona Este       --> Zona Norte (4 km)  -- alternativa: 3+4=7 km
    //
    // Dijkstra elegira la ruta directa de 5 km para Zona Norte.
    agregarConexion(g, "Almacen Central", "Zona Norte", 5.0);
    agregarConexion(g, "Almacen Central", "Zona Sur",   8.0);
    agregarConexion(g, "Almacen Central", "Zona Este",  3.0);
    agregarConexion(g, "Zona Este",       "Zona Norte", 4.0);
}

// Agrega una conexion dirigida entre dos nodos.
// Si los nodos no existen, los crea automaticamente.
void agregarConexion(Grafo& g, string origen, string destino, double distancia) {
    int idOrigen  = buscarOCrearNodo(g, origen);
    int idDestino = buscarOCrearNodo(g, destino);

    if (idOrigen == -1 || idDestino == -1) return;

    Nodo& nodo = g.nodos[idOrigen];

    if (nodo.numVecinos >= MAX_VECINOS) {
        cout << "[ERROR] Nodo '" << origen << "' tiene demasiados vecinos.\n";
        return;
    }

    nodo.vecinos[nodo.numVecinos].idDestino = idDestino;
    nodo.vecinos[nodo.numVecinos].distancia = distancia;
    nodo.numVecinos++;
}

// =============================================================
// Visualizacion
// =============================================================

void mostrarGrafo(const Grafo& g) {
    cout << "\n=== Grafo de Rutas ===\n";
    for (int i = 0; i < g.numNodos; i++) {
        const Nodo& n = g.nodos[i];
        cout << "  " << n.nombre << " -->\n";

        if (n.numVecinos == 0) {
            cout << "    (sin salidas)\n";
        }

        for (int j = 0; j < n.numVecinos; j++) {
            int dest = n.vecinos[j].idDestino;
            cout << "    --> " << g.nodos[dest].nombre
                 << " (" << n.vecinos[j].distancia << " km)\n";
        }
    }
}

// =============================================================
// Dijkstra
// =============================================================

// Calcula la ruta mas corta desde "Almacen Central" hasta el destino.
// Devuelve un ResultadoRuta con la lista de nodos y la distancia total.
ResultadoRuta calcularRutaOptima(const Grafo& g, string destino) {
    ResultadoRuta resultado;
    resultado.encontrada = false;
    resultado.numNodos   = 0;
    resultado.kmTotales  = 0.0;

    // Buscamos los indices de origen y destino en el grafo
    int idOrigen = buscarNodo(g, "Almacen Central");
    int idDest   = buscarNodo(g, destino);

    if (idDest == -1) {
        cout << "[ERROR] El destino '" << destino << "' no existe en el grafo.\n";
        return resultado;
    }

    // Caso especial: el destino es el mismo origen
    if (idOrigen == idDest) {
        resultado.nodos[0]  = "Almacen Central";
        resultado.numNodos  = 1;
        resultado.kmTotales = 0.0;
        resultado.encontrada = true;
        return resultado;
    }

    // PASO 1: Inicializacion de arrays de Dijkstra
    double dist[MAX_NODOS];      // distancia minima conocida desde el origen
    int    anterior[MAX_NODOS];  // nodo anterior en el camino optimo
    bool   visitado[MAX_NODOS];  // true si ya fue procesado

    for (int i = 0; i < g.numNodos; i++) {
        dist[i]     = INF;   // distancia desconocida = infinita
        anterior[i] = -1;    // sin nodo anterior
        visitado[i] = false;
    }
    dist[idOrigen] = 0.0;  // el origen esta a 0 km de si mismo

    // PASO 2: Bucle principal de Dijkstra
    for (int iter = 0; iter < g.numNodos; iter++) {

        // 2a) Encontrar el nodo no visitado con menor distancia
        int    u         = -1;
        double menorDist = INF;

        for (int i = 0; i < g.numNodos; i++) {
            if (!visitado[i] && dist[i] < menorDist) {
                menorDist = dist[i];
                u = i;
            }
        }

        // Si no hay nodo alcanzable o ya llegamos al destino, paramos
        if (u == -1 || u == idDest) break;

        visitado[u] = true;

        // 2b) Relajar las aristas del nodo actual
        for (int j = 0; j < g.nodos[u].numVecinos; j++) {
            int    v         = g.nodos[u].vecinos[j].idDestino;
            double nuevaDist = dist[u] + g.nodos[u].vecinos[j].distancia;

            // Si encontramos un camino mas corto, lo actualizamos
            if (nuevaDist < dist[v]) {
                dist[v]     = nuevaDist;
                anterior[v] = u;
            }
        }
    }

    // PASO 3: Reconstruir la ruta siguiendo los punteros 'anterior'
    if (dist[idDest] >= INF) {
        return resultado;  // no se encontro camino
    }

    // Reconstruimos al reves (destino --> origen)
    string rutaInversa[MAX_RUTA];
    int    numInversa = 0;
    int    actual     = idDest;

    while (actual != -1 && numInversa < MAX_RUTA) {
        rutaInversa[numInversa] = g.nodos[actual].nombre;
        numInversa++;
        actual = anterior[actual];
    }

    // Invertimos para obtener el orden correcto (origen --> destino)
    for (int i = 0; i < numInversa; i++) {
        resultado.nodos[i] = rutaInversa[numInversa - 1 - i];
    }

    resultado.numNodos  = numInversa;
    resultado.kmTotales = dist[idDest];
    resultado.encontrada = true;

    return resultado;
}
