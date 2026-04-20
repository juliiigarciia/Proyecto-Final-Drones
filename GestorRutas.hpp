// =============================================================
// GestorRutas.hpp
// Grafo de zonas de Madrid y algoritmo de Dijkstra.
// Incluye Excepciones.hpp para poder lanzar
// ExcepcionRutaNoEncontrada desde calcularRutaOptima.
// =============================================================
#pragma once
#include <string>
#include "Excepciones.hpp"

const int MAX_NODOS   = 25;
const int MAX_VECINOS = 8;
const int MAX_RUTA    = 30;

struct Arista {
    int    idDestino;
    double distancia;
};

struct Nodo {
    std::string nombre;
    Arista      vecinos[MAX_VECINOS];
    int         numVecinos;
};

struct Grafo {
    Nodo nodos[MAX_NODOS];
    int  numNodos;
};

struct ResultadoRuta {
    std::string nodos[MAX_RUTA];
    int    numNodos;
    double kmTotales;
    bool   encontrada;
};

void           inicializarGrafo(Grafo& g);
void           agregarConexion(Grafo& g, std::string origen,
                               std::string destino, double distancia);
int            buscarNodo(const Grafo& g, std::string nombre);
void           mostrarGrafo(const Grafo& g);

// Lanza ExcepcionRutaNoEncontrada si el destino no existe en el grafo
// o si no hay camino desde Madrid Centro.
ResultadoRuta  calcularRutaOptima(const Grafo& g, std::string destino);
