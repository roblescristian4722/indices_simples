#ifndef GESTOR_H
#define GESTOR_H

#if defined(_WIN32) | defined(_WIN64)
    #define CLEAR system("cls")
#else
    #define CLEAR system("clear")
#endif

#define TOTAL_CAMPOS 9

#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <regex>
#include <string>
#include <cstring>
#include "usuario.h"
using namespace std;

class Gestor
{
public:
    struct Indice
    {
        Indice() {}
        Indice(const char *cod, long ref);
        ~Indice();

        bool operator<(const Indice &other);
        bool operator>(const Indice &other);
        bool operator==(const Indice &other);
        void operator=(const Indice &other);
        bool operator<(const string &other);
        bool operator>(const string &other);
        bool operator==(const string &other);

        // Atributos
        char codigo[9];
        long referencia;
    };

    Gestor();
    ~Gestor();

    void menu();
    void capturar(const Usuario& usuario);
    void mostrar();
    void modificar();
    void buscar();
    void eliminar();
    void capturar_datos(Usuario& usuario);
    bool codigo_usado(const string codigo);
    void modificar_datos(Usuario& usuario, char i);
    int busqueda_binaria(vector<Gestor::Indice> &vec, string dato);
    void actualizarIndices();

private:
    vector<Gestor::Indice> m_indices;
    long m_posFinal;
    enum OPC_MENU
    {
        OPC_CAPTURAR = '1',
        OPC_MOSTRAR,
        OPC_BUSCAR,
        OPC_MODIFICAR,
        OPC_ELIMINAR,
        OPC_SALIR
    };

    enum OPC_CAMPOS_MOD
    {
        OPC_CAMPO_NOM = '1',
        OPC_CAMPO_APE,
        OPC_CAMPO_EDAD,
        OPC_CAMPO_SEXO,
        OPC_CAMPO_PESO,
        OPC_CAMPO_MASA,
        OPC_CAMPO_TIPO_SANGRE,
        OPC_CAMPO_ALTURA,
        OPC_CAMPO_CANCELAR
    };

    enum CAMPOS_ACCEDER_ARCHIVO
    {
        CAMPO_COD = 0,
        CAMPO_NOM,
        CAMPO_APE,
        CAMPO_EDAD,
        CAMPO_SEXO,
        CAMPO_PESO,
        CAMPO_MASA,
        CAMPO_TIPO_SANGRE,
        CAMPO_ALTURA
    };
};

#endif