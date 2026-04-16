// =============================================================
// GestorRutas.hpp
// Mariano
// Representacion del grafo de zonas y algoritmo de Dijkstra.
// En vez de map/set, usamos arrays con contador.
//
// Grafo predefinido:
//   Almacen Central --> Zona Norte (5 km)
//   Almacen Central --> Zona Sur   (8 km)
//   Almacen Central --> Zona Este  (3 km)
//   Zona Este       --> Zona Norte (4 km)
// =============================================================

#ifndef GESTOR_RUTAS_HPP
#define GESTOR_RUTAS_HPP

#include <string>

// ---- Constantes del grafo ----
const int MAX_NODOS   = 10;  // maximo de nodos en el grafo
const int MAX_VECINOS = 5;   // maximo de conexiones por nodo
const int MAX_RUTA    = 10;  // maximo de nodos en una ruta resultado

// ---- Arista: conexion entre dos nodos ----
struct Arista {
    int    idDestino;   // indice del nodo destino en el array de nodos
    double distancia;   // km
};

// ---- Nodo: zona o punto del grafo ----
struct Nodo {
    std::string nombre;
    Arista      vecinos[MAX_VECINOS];
    int         numVecinos;
};

// ---- Grafo: conjunto de nodos con sus conexiones ----
struct Grafo {
    Nodo nodos[MAX_NODOS];
    int  numNodos;
};

// ---- Resultado de ejecutar Dijkstra ----
struct ResultadoRuta {
    std::string nodos[MAX_RUTA];  // nodos en orden (origen --> destino)
    int    numNodos;
    double kmTotales;
    bool   encontrada;
};

// ---- Funciones ----

// Crea el grafo con las zonas y conexiones predefinidas del sistema
void inicializarGrafo(Grafo& g);

// Agrega una conexion dirigida entre dos nodos (crea los nodos si no existen)
void agregarConexion(Grafo& g, std::string origen, std::string destino, double distancia);

// Busca un nodo por nombre. Devuelve su indice o -1 si no existe.
int buscarNodo(const Grafo& g, std::string nombre);

// Muestra todas las conexiones del grafo por pantalla
void mostrarGrafo(const Grafo& g);

// Ejecuta Dijkstra desde "Almacen Central" hasta el destino indicado
ResultadoRuta calcularRutaOptima(const Grafo& g, std::string destino);

#endif // GESTOR_RUTAS_HPP
