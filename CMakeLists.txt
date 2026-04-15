# CMakeLists.txt
# Para compilar el proyecto con CMake:
#   cmake -S . -B build
#   cmake --build build
# Y luego ejecutar: ./build/sigfd  (o build\Debug\sigfd.exe en Windows)

cmake_minimum_required(VERSION 3.14)
project(SIGFD VERSION 1.0 LANGUAGES CXX)

# Usamos el estandar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Activamos advertencias del compilador para detectar errores
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Definimos el ejecutable y todos los archivos fuente
add_executable(sigfd
    main.cpp
    Drone.cpp
    Paquete.cpp
    GestorRutas.cpp
    GestorFicheros.cpp
    CentroLogistico.cpp
)
