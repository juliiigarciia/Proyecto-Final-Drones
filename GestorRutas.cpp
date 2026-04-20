// =============================================================
// GestorRutas.cpp
// Implementacion del grafo y Dijkstra.
// calcularRutaOptima lanza ExcepcionRutaNoEncontrada
// en vez de imprimir un error y devolver encontrada=false.
// =============================================================
#include "GestorRutas.hpp"
#include <iostream>
#include <string>
using namespace std;

const double INF = 1e18;

// Busca nodo por nombre; devuelve indice o -1
int buscarNodo(const Grafo& g, string nombre) {
    for (int i = 0; i < g.numNodos; i++)
        if (g.nodos[i].nombre == nombre) return i;
    return -1;
}

// Busca o crea nodo; lanza si el grafo esta lleno
static int buscarOCrearNodo(Grafo& g, string nombre) {
    int pos = buscarNodo(g, nombre);
    if (pos != -1) return pos;
    if (g.numNodos >= MAX_NODOS)
        throw ExcepcionRutaNoEncontrada("Grafo lleno, no se puede anadir: " + nombre);
    g.nodos[g.numNodos].nombre     = nombre;
    g.nodos[g.numNodos].numVecinos = 0;
    return g.numNodos++;
}

void inicializarGrafo(Grafo& g) {
    g.numNodos = 0;
    agregarConexion(g, "Madrid Centro", "Vallecas",    8.0);
    agregarConexion(g, "Madrid Centro", "Alcorcon",   14.0);
    agregarConexion(g, "Madrid Centro", "Getafe",     13.0);
    agregarConexion(g, "Madrid Centro", "Las Rozas",  19.0);
    agregarConexion(g, "Madrid Centro", "Alcobendas", 16.0);
    agregarConexion(g, "Madrid Centro", "Pozuelo",    10.0);
    agregarConexion(g, "Alcorcon",  "Mostoles",    5.0);
    agregarConexion(g, "Alcorcon",  "Leganes",     6.0);
    agregarConexion(g, "Alcorcon",  "Fuenlabrada", 8.0);
    agregarConexion(g, "Getafe",    "Leganes",     5.0);
    agregarConexion(g, "Getafe",    "Fuenlabrada", 7.0);
    agregarConexion(g, "Getafe",    "Parla",      10.0);
    agregarConexion(g, "Getafe",    "Pinto",       9.0);
    agregarConexion(g, "Pozuelo",   "Majadahonda", 6.0);
    agregarConexion(g, "Pozuelo",   "Boadilla",    8.0);
    agregarConexion(g, "Las Rozas", "Majadahonda", 4.0);
    agregarConexion(g, "Vallecas",  "Coslada",    11.0);
    agregarConexion(g, "Vallecas",  "Rivas",      15.0);
    agregarConexion(g, "Coslada",   "San Fernando", 3.0);
    agregarConexion(g, "San Fernando", "Torrejon", 9.0);
    agregarConexion(g, "Torrejon",  "Alcala de Henares", 11.0);
    agregarConexion(g, "Alcobendas","San Sebastian de los Reyes", 2.0);
}

void agregarConexion(Grafo& g, string origen, string destino, double distancia) {
    int idO = buscarOCrearNodo(g, origen);
    int idD = buscarOCrearNodo(g, destino);
    Nodo& n = g.nodos[idO];
    if (n.numVecinos >= MAX_VECINOS) {
        cerr << "[ERROR] Demasiados vecinos en nodo: " << origen << "\n";
        return;
    }
    n.vecinos[n.numVecinos].idDestino = idD;
    n.vecinos[n.numVecinos].distancia = distancia;
    n.numVecinos++;
}

void mostrarGrafo(const Grafo& g) {
    cout << "\n=== Grafo de Rutas ===\n";
    for (int i = 0; i < g.numNodos; i++) {
        const Nodo& n = g.nodos[i];
        cout << "  " << n.nombre << " -->\n";
        if (n.numVecinos == 0) { cout << "    (sin salidas)\n"; continue; }
        for (int j = 0; j < n.numVecinos; j++)
            cout << "    --> " << g.nodos[n.vecinos[j].idDestino].nombre
                 << " (" << n.vecinos[j].distancia << " km)\n";
    }
}

// =============================================================
// Dijkstra - lanza ExcepcionRutaNoEncontrada en caso de fallo
// =============================================================
ResultadoRuta calcularRutaOptima(const Grafo& g, string destino) {
    int idOrigen = buscarNodo(g, "Madrid Centro");
    int idDest   = buscarNodo(g, destino);

    if (idDest == -1)
        throw ExcepcionRutaNoEncontrada(
            "El destino '" + destino + "' no existe en el grafo.");

    ResultadoRuta resultado;
    resultado.encontrada = false;
    resultado.numNodos   = 0;
    resultado.kmTotales  = 0.0;

    if (idOrigen == idDest) {
        resultado.nodos[0]  = "Madrid Centro";
        resultado.numNodos  = 1;
        resultado.encontrada = true;
        return resultado;
    }

    double dist[MAX_NODOS];
    int    anterior[MAX_NODOS];
    bool   visitado[MAX_NODOS];
    for (int i = 0; i < g.numNodos; i++) {
        dist[i]     = INF;
        anterior[i] = -1;
        visitado[i] = false;
    }
    dist[idOrigen] = 0.0;

    for (int iter = 0; iter < g.numNodos; iter++) {
        int    u = -1;
        double menorDist = INF;
        for (int i = 0; i < g.numNodos; i++)
            if (!visitado[i] && dist[i] < menorDist) { menorDist = dist[i]; u = i; }
        if (u == -1 || u == idDest) break;
        visitado[u] = true;
        for (int j = 0; j < g.nodos[u].numVecinos; j++) {
            int    v        = g.nodos[u].vecinos[j].idDestino;
            double nuevaDist = dist[u] + g.nodos[u].vecinos[j].distancia;
            if (nuevaDist < dist[v]) { dist[v] = nuevaDist; anterior[v] = u; }
        }
    }

    if (dist[idDest] >= INF)
        throw ExcepcionRutaNoEncontrada(
            "No hay camino desde Madrid Centro hasta '" + destino + "'.");

    // Reconstruir ruta al reves
    string rutaInv[MAX_RUTA];
    int    numInv = 0, actual = idDest;
    while (actual != -1 && numInv < MAX_RUTA) {
        rutaInv[numInv++] = g.nodos[actual].nombre;
        actual = anterior[actual];
    }
    for (int i = 0; i < numInv; i++)
        resultado.nodos[i] = rutaInv[numInv - 1 - i];

    resultado.numNodos   = numInv;
    resultado.kmTotales  = dist[idDest];
    resultado.encontrada = true;
    return resultado;
}
