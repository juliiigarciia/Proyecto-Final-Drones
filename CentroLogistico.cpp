// =============================================================
// CentroLogistico.cpp
// Implementacion de la clase CentroLogistico.
// Usa vector<Drone*> (memoria dinamica), lanza excepciones
// y demuestra polimorfismo dinamico al llamar a d->calcularBateria().
// =============================================================
#include "CentroLogistico.hpp"
#include <iostream>
#include <stdexcept>
using namespace std;

// Definicion de la variable estatica
int CentroLogistico::totalEnviosSesion = 0;

// =============================================================
// Constructor / Destructor
// =============================================================

CentroLogistico::CentroLogistico(const string& nombre)
    : nombre(nombre), totalEnvios(0)
{
    inicializarGrafo(grafo);
    cout << "\n*** Centro Logistico '" << nombre << "' iniciado ***\n";
}

// El destructor libera TODA la memoria dinamica de la flota
CentroLogistico::~CentroLogistico() {
    for (Drone* d : drones) {
        delete d;   // libera cada objeto creado con 'new'
    }
    drones.clear();
}

int CentroLogistico::getTotalEnviosSesion() { return totalEnviosSesion; }

// =============================================================
// Metodos privados de apoyo
// =============================================================

Drone* CentroLogistico::buscarDronPorId(const string& id) const {
    for (Drone* d : drones)
        if (d->getId() == id) return d;
    return nullptr;   // nullptr es el "null" moderno de C++
}

Drone* CentroLogistico::buscarDronPorTipo(double peso, const string& tipo) const {
    for (Drone* d : drones) {
        if (d->getTipo()       == tipo       &&
            d->getEstado()     == DISPONIBLE  &&
            d->getCargaMaxima() >= peso)
            return d;
    }
    return nullptr;
}

Drone* CentroLogistico::buscarDronCualquiera(double peso) const {
    for (Drone* d : drones) {
        if (d->getEstado()     == DISPONIBLE &&
            d->getCargaMaxima() >= peso)
            return d;
    }
    return nullptr;
}

bool CentroLogistico::existePaquete(const string& id) const {
    for (const Paquete& p : cola)
        if (p.getId() == id) return true;
    for (const RegistroVuelo& v : historial)
        if (v.idPaquete == id) return true;
    return false;
}

// realizarEnvio: ejecuta el vuelo completo.
// Lanza ExcepcionRutaNoEncontrada o ExcepcionBateria si algo falla.
void CentroLogistico::realizarEnvio(Drone& dron, Paquete& paquete) {
    // Dijkstra: puede lanzar ExcepcionRutaNoEncontrada
    ResultadoRuta ruta = calcularRutaOptima(grafo, paquete.getDestino());

    // Calculo de consumo (polimorfismo dinamico: llama a la version
    // correcta de calcularBateria segun el tipo real del dron)
    double consumoPorKm = dron.calcularBateria(paquete.getPeso());
    double kmNecesarios = ruta.kmTotales * consumoPorKm;

    if (!dron.tieneBateriaSuficiente(kmNecesarios)) {
        throw ExcepcionBateria(
            "Dron " + dron.getId() +
            " necesita " + to_string(kmNecesarios) +
            " km de bateria pero solo tiene " +
            to_string(dron.getBateriaActual()) + " km.");
    }

    // Comprobar peso (polimorfismo dinamico: getCargaMaxima() virtual)
    if (paquete.getPeso() > dron.getCargaMaxima()) {
        throw ExcepcionPesoExcesivo(
            "Paquete " + paquete.getId() + " pesa " +
            to_string(paquete.getPeso()) + " kg, dron aguanta " +
            to_string(dron.getCargaMaxima()) + " kg.");
    }

    // Ejecutar vuelo
    dron.setBateriaActual(dron.getBateriaActual() - kmNecesarios);
    dron.setEstado(EN_VUELO);
    paquete.setIdDronAsignado(dron.getId());

    // Construir texto de la ruta
    string rutaTexto;
    for (int i = 0; i < ruta.numNodos; i++) {
        if (i > 0) rutaTexto += " -> ";
        rutaTexto += ruta.nodos[i];
    }

    // Guardar registro en el historial
    RegistroVuelo vuelo;
    vuelo.fecha        = obtenerFechaHoraActual();
    vuelo.idDron       = dron.getId();
    vuelo.tipoDron     = dron.getTipo();
    vuelo.idPaquete    = paquete.getId();
    vuelo.ruta         = rutaTexto;
    vuelo.kmRecorridos = ruta.kmTotales;
    historial.push_back(vuelo);

    totalEnvios++;
    totalEnviosSesion++;

    cout << "[ENVIO] " << dron.getId()
         << " --> " << paquete.getId()
         << " --> " << paquete.getDestino()
         << " | " << rutaTexto
         << " | " << ruta.kmTotales << " km\n";

    dron.setEstado(DISPONIBLE);   // regresa al deposito (simulado)
}

// =============================================================
// Gestion de drones
// =============================================================

void CentroLogistico::agregarDron(Drone* d) {
    if ((int)drones.size() >= MAX_DRONES_FLOTA)
        throw ExcepcionFlotaLlena("Flota llena. No se puede agregar: " + d->getId());
    if (buscarDronPorId(d->getId()) != nullptr)
        throw ExcepcionDronNoEncontrado("Ya existe un dron con ID: " + d->getId());
    drones.push_back(d);
    cout << "[FLOTA] Dron " << d->getId()
         << " (" << d->getTipo() << ") agregado.\n";
}

bool CentroLogistico::eliminarDron(const string& id) {
    for (int i = 0; i < (int)drones.size(); i++) {
        if (drones[i]->getId() == id) {
            delete drones[i];               // libera memoria
            drones.erase(drones.begin() + i); // elimina del vector
            cout << "[FLOTA] Dron " << id << " eliminado.\n";
            return true;
        }
    }
    throw ExcepcionDronNoEncontrado("No existe ningun dron con ID: " + id);
}

// Sobrecarga estatica 1: sin parametros
void CentroLogistico::mostrarFlota() const {
    cout << "\n=== Flota de Drones (" << drones.size() << " unidades) ===\n";
    if (drones.empty()) { cout << "  (sin drones registrados)\n"; return; }
    for (Drone* d : drones) d->mostrar();  // polimorfismo: cada tipo imprime igual
}

// Sobrecarga estatica 2: con parametro detallado
void CentroLogistico::mostrarFlota(bool detallado) const {
    cout << "\n=== Flota de Drones (" << drones.size() << " unidades) ===\n";
    if (drones.empty()) { cout << "  (sin drones registrados)\n"; return; }
    for (Drone* d : drones) d->mostrar(detallado); // polimorfismo
}

// =============================================================
// Gestion de paquetes
// =============================================================

// Sobrecarga estatica 1: desde objeto Paquete
void CentroLogistico::registrarPaquete(const Paquete& p) {
    if (existePaquete(p.getId()))
        throw runtime_error("Ya existe un paquete con ID: " + p.getId());

    cout << "\n[PAQUETE] Registrando " << p.getId()
         << " (" << p.getDestino() << ", " << p.getPeso() << " kg)\n";

    Drone* dron = nullptr;
    double peso = p.getPeso();
    if      (peso < 5.0)  dron = buscarDronPorTipo(peso, "Express");
    else if (peso <= 15.0) dron = buscarDronPorTipo(peso, "Standard");
    else                   dron = buscarDronPorTipo(peso, "HeavyDuty");

    if (!dron) dron = buscarDronCualquiera(peso);

    if (dron) {
        Paquete copia = p;
        realizarEnvio(*dron, copia);
    } else {
        cola.push_back(p);
        cout << "[COLA] Sin drones disponibles. Paquete " << p.getId() << " en cola.\n";
    }
}

// Sobrecarga estatica 2: desde parametros individuales
void CentroLogistico::registrarPaquete(const string& id, double peso,
                                       const string& destino, Prioridad prioridad)
{
    Paquete p(id, peso, destino, prioridad);
    registrarPaquete(p);  // reutiliza la version anterior
}

void CentroLogistico::ponerEnCola(const Paquete& p) {
    if (existePaquete(p.getId()))
        throw runtime_error("Ya existe un paquete con ID: " + p.getId());
    cola.push_back(p);
    cout << "[COLA] Paquete " << p.getId() << " anadido a la espera.\n";
}

void CentroLogistico::mostrarColaEspera() const {
    cout << "\n=== Cola de Espera (" << cola.size() << " paquetes) ===\n";
    if (cola.empty()) { cout << "  (sin paquetes en espera)\n"; return; }
    for (int i = 0; i < (int)cola.size(); i++) {
        cout << "  [" << (i + 1) << "] ";
        cola[i].mostrar();
    }
}

void CentroLogistico::procesarCola() {
    if (cola.empty()) {
        cout << "\n[COLA] La cola esta vacia.\n";
        return;
    }

    // FIFO: sacamos el primero
    Paquete paquete = cola.front();
    cola.erase(cola.begin());   // vector::erase desplaza automaticamente

    cout << "\n[COLA] Procesando '" << paquete.getId()
         << "' (" << paquete.getPeso() << " kg) -> " << paquete.getDestino() << "\n";

    double peso = paquete.getPeso();
    Drone* dron = nullptr;
    if      (peso < 5.0)   dron = buscarDronPorTipo(peso, "Express");
    else if (peso <= 15.0)  dron = buscarDronPorTipo(peso, "Standard");
    else                    dron = buscarDronPorTipo(peso, "HeavyDuty");
    if (!dron) dron = buscarDronCualquiera(peso);

    if (dron) {
        try {
            realizarEnvio(*dron, paquete);
        } catch (const ExcepcionBateria& e) {
            cout << e.what() << "\n[COLA] Paquete devuelto al final de la cola.\n";
            cola.push_back(paquete);
        }
    } else {
        cout << "[COLA] Sin drones. Paquete '" << paquete.getId() << "' devuelto.\n";
        cola.push_back(paquete);
    }
}

// =============================================================
// ejecutarVuelo (opcion 6): el operador elige el dron
// =============================================================
void CentroLogistico::ejecutarVuelo(const string& idDron) {
    // buscarDronPorId devuelve nullptr si no existe
    Drone* dron = buscarDronPorId(idDron);
    if (!dron)
        throw ExcepcionDronNoEncontrado("No existe ningun dron con ID '" + idDron + "'.");

    // Buscar paquete asignado a este dron en la cola
    int idxPkg = -1;
    for (int i = 0; i < (int)cola.size(); i++) {
        if (cola[i].getIdDronAsignado() == idDron) { idxPkg = i; break; }
    }
    if (idxPkg == -1)
        throw runtime_error("El dron '" + idDron + "' no tiene paquete asignado. Usa opcion 4 primero.");

    Paquete paquete = cola[idxPkg];
    cola.erase(cola.begin() + idxPkg);

    cout << "\n[VUELO] Calculando ruta: " << idDron
         << " -> " << paquete.getDestino() << "...\n";

    // Llama a Dijkstra + comprobacion bateria (puede lanzar excepciones)
    try {
        ResultadoRuta ruta = calcularRutaOptima(grafo, paquete.getDestino());

        double consumoPorKm = dron->calcularBateria(paquete.getPeso()); // POLIMORFISMO
        double kmGastados   = ruta.kmTotales * consumoPorKm;

        cout << "  Ruta: ";
        for (int i = 0; i < ruta.numNodos; i++) {
            if (i > 0) cout << " -> ";
            cout << ruta.nodos[i];
        }
        cout << "\n  Distancia: " << ruta.kmTotales << " km"
             << " | Consumo: " << kmGastados << " km de bateria"
             << " | Bateria actual: " << dron->getBateriaActual() << " km\n";

        if (!dron->tieneBateriaSuficiente(kmGastados)) {
            // Devolvemos paquete a la cola con asignacion intacta
            cola.push_back(paquete);
            throw ExcepcionBateria(
                "Necesita " + to_string(kmGastados) +
                " km pero tiene " + to_string(dron->getBateriaActual()) +
                " km. Recarga con opcion 2.");
        }

        dron->setBateriaActual(dron->getBateriaActual() - kmGastados);
        dron->setEstado(EN_VUELO);

        string rutaTexto;
        for (int i = 0; i < ruta.numNodos; i++) {
            if (i > 0) rutaTexto += " -> ";
            rutaTexto += ruta.nodos[i];
        }
        RegistroVuelo vuelo;
        vuelo.fecha        = obtenerFechaHoraActual();
        vuelo.idDron       = dron->getId();
        vuelo.tipoDron     = dron->getTipo();
        vuelo.idPaquete    = paquete.getId();
        vuelo.ruta         = rutaTexto;
        vuelo.kmRecorridos = ruta.kmTotales;
        historial.push_back(vuelo);
        totalEnvios++;
        totalEnviosSesion++;

        dron->setEstado(DISPONIBLE);

        cout << "\n[OK] VUELO EXITOSO!\n"
             << "  Dron    : " << dron->getId() << " (" << dron->getTipo() << ")\n"
             << "  Paquete : " << paquete.getId() << " (" << paquete.getPeso() << " kg)\n"
             << "  Entrega : " << paquete.getDestino() << "\n"
             << "  Bateria restante: " << dron->getBateriaActual()
             << " / " << dron->getAutonomia() << " km\n";

    } catch (const ExcepcionRutaNoEncontrada& e) {
        cola.push_back(paquete);  // devolver paquete
        throw;  // re-lanzar para que main lo capture
    }
}

// =============================================================
// Mantenimiento
// =============================================================

void CentroLogistico::recargarBaterias() {
    cout << "\n[MANT] Iniciando recarga de baterias...\n";
    for (Drone* d : drones) {
        if (d->getEstado() != EN_VUELO) {
            double ant = d->getBateriaActual();
            d->recargar();
            cout << "  [" << d->getId() << "] " << ant
                 << " km -> " << d->getAutonomia() << " km (RECARGADO)\n";
        } else {
            cout << "  [" << d->getId() << "] EN VUELO - no se recarga ahora.\n";
        }
    }
    cout << "[MANT] Recarga completada.\n";
}

// =============================================================
// Ficheros
// =============================================================

void CentroLogistico::cargarFlotaDesdeFichero(const string& fichero) {
    cout << "\n[FICHERO] Cargando flota desde: " << fichero << "\n";
    // Puede lanzar ExcepcionFichero si no existe el archivo
    cargarFlota(drones, fichero);
}

void CentroLogistico::exportarInformeDia(const string& fichero) const {
    // Puede lanzar ExcepcionFichero si no puede crear el archivo
    exportarInforme(historial, fichero);
}

// =============================================================
// Informes
// =============================================================

void CentroLogistico::mostrarResumen() const {
    cout << "\n========== RESUMEN DEL DIA ==========\n"
         << "  Centro          : " << nombre       << "\n"
         << "  Drones en flota : " << drones.size() << "\n"
         << "  Envios hoy      : " << totalEnvios  << "\n"
         << "  Paquetes espera : " << cola.size()  << "\n"
         << "  Total sesion    : " << totalEnviosSesion << "\n"
         << "  Drones creados  : " << Drone::getTotalCreados() << " (en memoria)\n"
         << "=====================================\n";
}
