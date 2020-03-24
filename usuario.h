#ifndef USUARIO_H
#define USUARIO_H

#include <iostream>
using namespace std;

class Usuario
{
public:
    Usuario(){}
    ~Usuario(){}

    // GETTERS
    string getCodigo() const;
    string getNombre() const;
    string getApellido() const;
    string getTipoSangre() const;
    unsigned int getEdad() const ;
    char getGenero() const;
    float getMasaCorporal() const;
    float getAltura() const;
    float getPeso() const;

    // SETTERS
    void setCodigo(const string codigo);
    void setNombre(const string nombre);
    void setApellido(const string apellido);
    void setTipoSangre(const string tipoSangre);
    void setEdad(unsigned int edad);
    void setGenero(char genero);
    void setMasaCorporal(float masaCorporal);
    void setAltura(float altura);
    void setPeso(float peso);
    
private:
    string m_codigo;
    string m_nombre;
    string m_apellido;
    string m_tipoSangre;
    unsigned int m_edad;
    char m_genero;
    float m_altura;
    float m_peso;
    float m_masaCorporal;
};

#endif