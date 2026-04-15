@echo off
REM compilar.bat - Script para compilar el proyecto SIGFD con MinGW/g++
REM Requiere tener MinGW instalado (g++ disponible en el PATH).
REM Descarga MinGW: https://www.mingw-w64.org/

echo ===================================
echo  Compilando SIGFD con g++...
echo ===================================

g++ -std=c++17 -Wall -Wextra ^
    main.cpp ^
    Drone.cpp ^
    Paquete.cpp ^
    GestorRutas.cpp ^
    GestorFicheros.cpp ^
    CentroLogistico.cpp ^
    -o sigfd.exe

IF %ERRORLEVEL% EQU 0 (
    echo.
    echo  Compilacion exitosa: sigfd.exe
    echo  Ejecuta: sigfd.exe
) ELSE (
    echo.
    echo  Error en la compilacion. Revisa los mensajes anteriores.
)
pause
