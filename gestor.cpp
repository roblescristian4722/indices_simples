#include "gestor.h"

Gestor::Gestor()
{
    Gestor::Indice indiceTmp;
    string aux;
    Usuario usuarioTmp;
    fstream archivoDatos("usuarios.txt", ios::out | ios::in);
    fstream archivoIndices("indices.bin", ios::out | ios::in | ios::binary);

    m_posFinal = 0;

    if (!archivoDatos.is_open())
        archivoDatos.open("usuarios.txt", ios::out);

    if (!archivoIndices.is_open())
    {
        indiceTmp.referencia = m_posFinal;
        archivoIndices.open("indices.bin", ios::out | ios::trunc | ios::binary);
        archivoIndices.write((char *)&indiceTmp, sizeof(indiceTmp));
    }
    else
    {
        archivoIndices.read((char*)&indiceTmp, sizeof(indiceTmp));
        m_posFinal = indiceTmp.referencia;

        while (!archivoIndices.eof())
        {
            archivoIndices.read((char*)&indiceTmp, sizeof(indiceTmp));
            if (archivoIndices.eof())
                break;
            m_indices.push_back(indiceTmp);
        }
    }
    archivoDatos.close();
    archivoIndices.close();
}

Gestor::~Gestor()
{}

void Gestor::menu()
{
    char opc;
    do
    {
        CLEAR;
        cout << " Ingrese la opción a ejecutar: " << endl
             << " " << char(OPC_CAPTURAR) << ") Capturar usuario" << endl
             << " " << char(OPC_MOSTRAR) << ") Mostrar usuarios" << endl
             << " " << char(OPC_BUSCAR) << ") Buscar usuario (por código)" << endl
             << " " << char(OPC_MODIFICAR) << ") Modificar usuario" << endl
             << " " << char(OPC_ELIMINAR) << ") Eliminar usuario" << endl
             << " " << char(OPC_SALIR) << ") Salir" << endl
             << " Opción: ";

        cin >> opc;
        CLEAR;
        cin.ignore();
        switch (opc)
        {
            case OPC_CAPTURAR:
            {
                Usuario usuarioTmp;
                capturar_datos(usuarioTmp);
                capturar(usuarioTmp);
                cin.ignore();
            }
            break;

            case OPC_MODIFICAR:
                modificar();
            break;

            case OPC_BUSCAR:
                buscar();
            break;

            case OPC_ELIMINAR:
                eliminar();
            break;

            case OPC_MOSTRAR:
                CLEAR;
                mostrar();
            break;

            case OPC_SALIR:
                cout << " Hasta la próxima" << endl;
            return;

            default:
                cout << " Dato inválido, presione ENTER para continuar..." << endl;
            break;
        }
        cin.get();
    } while (opc != OPC_SALIR);
}

void Gestor::buscar()
{
    string aux;
    string codigo;
    Usuario usuarioTmp;
    fstream archivoDatos("usuarios.txt", ios::in);
    int res;

    if (!m_indices.size())
        cout << " Aún no se han ingresado usuarios " << endl << endl
             << " Presione ENTER para continuar..." << endl;
    else
    {
        cout << " Ingrese el código a buscar: ";
        getline(cin, codigo);
        res = busqueda_binaria(m_indices, codigo);
        if (res == -1)
            cout << " Código no encontrado, presione ENTER para continuar..." << endl;
        else
        {
            archivoDatos.seekg(m_indices[res].referencia);
            getline(archivoDatos, aux, '|');
            usuarioTmp.setCodigo(aux);
            getline(archivoDatos, aux, '|');
            usuarioTmp.setNombre(aux);
            getline(archivoDatos, aux, '|');
            usuarioTmp.setApellido(aux);
            getline(archivoDatos, aux, '|');
            usuarioTmp.setEdad(stoi(aux));
            getline(archivoDatos, aux, '|');
            usuarioTmp.setGenero(aux[0]);
            getline(archivoDatos, aux, '|');
            usuarioTmp.setPeso(stof(aux));
            getline(archivoDatos, aux, '|');
            usuarioTmp.setMasaCorporal(stof(aux));
            getline(archivoDatos, aux, '|');
            usuarioTmp.setTipoSangre(aux);
            getline(archivoDatos, aux);
            usuarioTmp.setAltura(stof(aux));
            //CLEAR;
            cout << endl
                    << " Usuario #" << res + 1 << endl
                    << " Código: " << usuarioTmp.getCodigo() << endl
                    << " Nombre: " << usuarioTmp.getNombre() << endl
                    << " Apellido: " << usuarioTmp.getApellido() << endl
                    << " Edad: " << usuarioTmp.getEdad() << endl
                    << " Género: " << usuarioTmp.getGenero() << endl
                    << " Peso: " << usuarioTmp.getPeso() << endl
                    << " Altura: " << usuarioTmp.getAltura() << endl
                    << "----------------------------------------------"
                    << endl
                    << endl
                    << " Presione ENTER para continuar..." << endl;
        }
    }
}

void Gestor::capturar(const Usuario& usuario)
{
    fstream archivoDatos("usuarios.txt", ios::out | ios::in | ios::app);
    Gestor::Indice indiceTmp;

    if (!archivoDatos.is_open())
    {
        cout << "Error en el archivoDatos de salida" << endl;
        return;
    }
    
    strcpy(indiceTmp.codigo, usuario.getCodigo().c_str());
    indiceTmp.referencia = m_posFinal;
    m_indices.push_back(indiceTmp);

    archivoDatos << usuario.getCodigo() << '|' << usuario.getNombre() << '|'
                 << usuario.getApellido() << '|' << usuario.getEdad() << '|'
                 << usuario.getGenero() << '|' << usuario.getPeso() << '|'
                 << usuario.getPeso() << '|' << usuario.getTipoSangre() << '|'
                 << usuario.getAltura() << '\n';

    m_posFinal += archivoDatos.tellg();
    archivoDatos.close();
    actualizarIndices();
    cout << endl
         << " Usuario añadido exitosamente." << endl
         << " Presione ENTER para continuar..." << endl;
}


void Gestor::eliminar()
{
    unsigned int elim;
    int res;
    long largElim;
    long pos;
    bool found = false;
    fstream archivoDatos("usuarios.txt", ios::in);
    fstream archivoTmp("usuarios.tmp", ios::out);
    string aux;
    Usuario usuarioTmp;

    
    mostrar();
    if (!m_indices.size())
    {
        cout << " Aún no hay datos ingresados." << endl
             << " presione ENTER para continuar" << endl;
        return;
    }

    cout << " Ingrese número del usuario a eliminar: ";
    cin >> elim;
    if (elim <= m_indices.size() && elim)
    {
        m_posFinal = 0;
        while (!archivoDatos.eof())
        {
            // Se lee el código y la posición de este
            pos = archivoDatos.tellg();
            getline(archivoDatos, aux, '|');
            if (archivoDatos.eof())
                break;

            if (aux != m_indices[elim - 1].codigo)
            {
                res = busqueda_binaria(m_indices, aux);

                if (!found)
                    m_indices[res].referencia = pos;
                else
                    m_indices[res].referencia = pos - largElim;
                
                // Se escribe el código y su separador en el tmp
                archivoTmp << aux << '|';
                // Se lee el resto del registro y se escribe en el tmp
                getline(archivoDatos, aux);
                archivoTmp << aux << '\n';
                // Se le suma el total de bytes del registro a la posición
                // final del archivo
                m_posFinal += archivoTmp.tellp();
            }
            else
            {
                found = true;
                m_indices.erase(m_indices.begin() + elim - 1);
                getline(archivoDatos, aux);
                largElim = archivoDatos.tellg();
                largElim -= pos;   
            }
        }
        archivoDatos.close();
        archivoTmp.close();
        remove("usuarios.txt");
        rename("usuarios.tmp", "usuarios.txt");
        actualizarIndices();
    }
    else
    {
        cout << endl
            << " Dato inválido, presione ENTER para continuar..."
            << endl;
        cin.get();
    }
}


void Gestor::modificar()
{   
    Usuario usuarioTmp;
    fstream archivoDatos;
    fstream archivoTmp;
    string aux;
    long mod;
    int res;
    long pos;
    long posMod;
    bool found;
    long largMod;
    char opc;

    mostrar();    
    if (m_indices.size())
    {
        cout << " Ingrese número del usuario a modificar: ";
        cin >> mod;

        if (mod <= m_indices.size() && mod)
        {   
            do
            {
                cout << endl
                     << " Seleccione el campo a modificar:" << endl
                     << char(OPC_CAMPO_NOM) << ") Nombre" << endl
                     << char(OPC_CAMPO_APE) << ") Apellido" << endl
                     << char(OPC_CAMPO_EDAD) << ") Edad" << endl
                     << char(OPC_CAMPO_SEXO) << ") Sexo" << endl
                     << char(OPC_CAMPO_PESO) << ") Peso" << endl
                     << char(OPC_CAMPO_TIPO_SANGRE) << ") Tipo de sanguíneo" << endl
                     << char(OPC_CAMPO_ALTURA) << ") Altura" << endl
                     << char(OPC_CAMPO_CANCELAR) << ") Cancelar" << endl
                     << "Opción: ";
                cin >> opc;
            }while(opc < OPC_CAMPO_NOM || opc > OPC_CAMPO_CANCELAR);
            
            if (opc != OPC_CAMPO_CANCELAR)
            {
                m_posFinal = 0;
                archivoDatos.open("usuarios.txt", ios::in | ios::out);
                archivoTmp.open("usuarios.tmp", ios::out);
                while (!archivoDatos.eof())
                {
                    getline(archivoDatos, aux, '|');
                    if (archivoDatos.eof())
                        break;

                    if (aux != m_indices[mod - 1].codigo)
                    {
                        archivoTmp << aux << '|';

                        getline(archivoDatos, aux);
                        archivoTmp << aux << '\n';
                    }
                    else
                    {
                        usuarioTmp.setCodigo(aux);
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setNombre(aux);
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setApellido(aux);
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setEdad(stoi(aux));
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setGenero(aux[0]);
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setPeso(stof(aux));
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setMasaCorporal(stof(aux));
                        getline(archivoDatos, aux, '|');
                        usuarioTmp.setTipoSangre(aux);
                        getline(archivoDatos, aux);
                        usuarioTmp.setAltura(stof(aux));

                        modificar_datos(usuarioTmp, opc);
                        
                        archivoTmp << usuarioTmp.getCodigo() << '|'
                                   << usuarioTmp.getNombre() << '|'
                                   << usuarioTmp.getApellido() << '|'
                                   << usuarioTmp.getEdad() << '|'
                                   << usuarioTmp.getGenero() << '|'
                                   << usuarioTmp.getPeso() << '|'
                                   << usuarioTmp.getMasaCorporal() << '|'
                                   << usuarioTmp.getTipoSangre() << '|'
                                   << usuarioTmp.getAltura() << '\n';
                    }
                }
                archivoTmp.close();
                archivoDatos.close();
                archivoTmp.open("usuarios.tmp", ios::in | ios::out);
                archivoDatos.open("usuarios.txt", ios::out);
                
                while (!archivoTmp.eof())
                {
                    pos = archivoTmp.tellg();
                    getline(archivoTmp, aux, '|');
                    if (archivoTmp.eof())
                        break;

                    res = busqueda_binaria(m_indices, aux);
                    m_indices[res].referencia = pos;
                    archivoDatos << aux << '|';

                    getline(archivoTmp, aux);
                    archivoDatos << aux << '\n';
                    m_posFinal += archivoTmp.tellp();
                }

                archivoDatos.close();
                archivoTmp.close();
                remove("usuarios.tmp");
                
                actualizarIndices();

                cout << endl
                     << " Dato modificado correctamente" << endl
                     << " Presione ENTER para continuar..." << endl;
            }
        }

        else
        {
            cout << endl
                 << " Dato inválido, presione ENTER para continuar..."
                 << endl;
            cin.get();       
        }
    }
}

void Gestor::mostrar()
{
    fstream archivoDatos("usuarios.txt", ios::in);
    Usuario usuarioTmp;
    string aux;

    if (!m_indices.size())
    {
        cout << " Aún no se han ingresado usuarios" << endl;
        return;
    }

    for (size_t i = 0; i < m_indices.size(); i++)
    {
        archivoDatos.seekg(m_indices[i].referencia);
        getline(archivoDatos, aux, '|');
        usuarioTmp.setCodigo(aux);
        getline(archivoDatos, aux, '|');
        usuarioTmp.setNombre(aux);
        getline(archivoDatos, aux, '|');
        usuarioTmp.setApellido(aux);
        getline(archivoDatos, aux, '|');
        usuarioTmp.setEdad(stoi(aux));
        getline(archivoDatos, aux, '|');
        usuarioTmp.setGenero(aux[0]);
        getline(archivoDatos, aux, '|');
        usuarioTmp.setPeso(stof(aux));
        getline(archivoDatos, aux, '|');
        usuarioTmp.setMasaCorporal(stof(aux));
        getline(archivoDatos, aux, '|');
        usuarioTmp.setTipoSangre(aux);
        getline(archivoDatos, aux);
        usuarioTmp.setAltura(stof(aux));

        cout << " Usuario #" << i + 1 << endl
             << " Código: " << usuarioTmp.getCodigo() << endl
             << " Nombre: " << usuarioTmp.getNombre() << endl
             << " Apellido: " << usuarioTmp.getApellido() << endl
             << " Edad: " << usuarioTmp.getEdad() << endl
             << " Género: " << usuarioTmp.getGenero() << endl
             << " Peso: " << usuarioTmp.getPeso() << endl
             << " Altura: " << usuarioTmp.getAltura() << endl
             << "----------------------------------------------"
             << endl;
    }
    cout << " Presione ENTER para continuar" << endl;
}

bool Gestor::codigo_usado(const string codigo)
{
    for (int i = 0; i < m_indices.size(); i++)
        if (busqueda_binaria(m_indices, codigo) != -1)
            return true;
    return false;
}

void Gestor::capturar_datos(Usuario &usuario)
{
    string codigo;
    string nombre;
    string apellido;
    string genero;
    string tipoSangre;
    unsigned int edad;
    float altura;
    float peso;
    bool continuar = false;

    // Expresiones regulares
    regex expCodigo("([1-9]{1}[0-9]{8})$");
    regex expNombre("(?:[a-zA-ZñÑ]{4,})(?: [a-zA-ZñÑ]{4,})?{1,2}");
    regex expApellido("(?:[a-zA-ZñÑ]{4,})(?: [a-zA-ZñÑ]{3,})$");
    regex expGenero("(?:[MF]){1}$");
    regex expTipoSangre("(AB|A|B|O)(?:[+-]{1})$");

    cout << " Presione ENTER para continuar e ingrese los siguientes datos"
         << endl
         << endl;

    // Obtención de código
    do
    {
        CLEAR;
        cout << " Código (9 dígitos): ";
        getline(cin, codigo);
        if (codigo_usado(codigo))
        {
            cout << endl
                 << " Error, código en uso. Presione ENTER para continuar";
            cin.get();
        }
        else if (!regex_match(codigo, expCodigo))
        {
            cout << endl
                 << " Error el código debe de tener 9 dígitos y no debe de"
                 << " empezar con cero" << endl
                 << " Presione ENTER para continuar";
            cin.get();
        }
        else
            continuar = true;
    } while (!continuar);
    continuar = false;

    // Obtención de nombre
    do
    {
        CLEAR;
        cout << " Nombre: ";
        getline(cin, nombre);
        if (!regex_match(nombre, expNombre))
        {
            cout << " Error, introducir mínimo un nombre y máximo tres "
                 << endl
                 << " (mínimo cuatro letras por nombre)"
                 << endl
                 << " Presione ENTER para continuar"
                 << endl;
            cin.get();
        }
        else
            continuar = true;
    } while (!continuar);
    continuar = false;

    // Obtención de apellido
    do
    {
        CLEAR;
        cout << " Apellido: ";
        getline(cin, apellido);
        if (!regex_match(apellido, expApellido))
        {
            cout << " Error, se deben de introducir dos apellidos"
                 << " separados por espacios"
                 << endl
                 << " Presione ENTER para continuar"
                 << endl;
            cin.get();
        }
        else
            continuar = true;
    } while (!continuar);

    do
    {
        CLEAR;
        cout << " Edad (de 12 a 115 años): ";
        cin >> edad;
    } while (edad > 115 || edad < 12);

    do
    {
        CLEAR;
        cout << " Género (M = masculino | F = femenino): ";
        cin >> genero;
    } while (!regex_match(genero, expGenero));

    do
    {
        CLEAR;
        cout << " Peso (de 30 a 300 kg): ";
        cin >> peso;
    } while (peso > 300 || peso < 30);

    continuar = false;
    cin.ignore();
    //Tipo de sangre
    do
    {
        CLEAR;
        cout << " Tipo de sangre (AB, A, B, O)(+, -): ";
        getline(cin, tipoSangre);
        if (!regex_match(tipoSangre, expTipoSangre))
        {
            cout << endl
                 << " Error, vuelva a intentarlo" << endl;
            cin.get();
        }
        else
            continuar = true;
    } while (!continuar);

    do
    {
        CLEAR;
        cout << " Altura (de 0.8 a 2.5 mts): ";
        cin >> altura;
        ;
    } while (altura < 0.8 || altura > 2.5);

    usuario.setAltura(altura);
    usuario.setApellido(apellido);
    usuario.setCodigo(codigo);
    usuario.setEdad(edad);
    usuario.setGenero(genero[0]);
    usuario.setNombre(nombre);
    usuario.setPeso(peso);
    usuario.setTipoSangre(tipoSangre);
    usuario.setMasaCorporal(peso / (altura * altura));
}

void Gestor::modificar_datos(Usuario &usuario, char i)
{
    bool continuar = false;

    // Expresiones regulares
    regex expCodigo("([1-9]{1}[0-9]{8})$");
    regex expNombre("(?:[a-zA-ZñÑ]{4,})(?: [a-zA-ZñÑ]{4,})?{1,2}");
    regex expApellido("(?:[a-zA-ZñÑ]{4,})+(?: [a-zA-ZñÑ]{3,})$");
    regex expGenero("(?:[mMfF]){1}$");
    regex expTipoSangre("(AB|A|B|O)(?:[+-]{1})$");

    cin.ignore();
    switch (i)
    {
    case OPC_CAMPO_NOM:
    {
        string nombre;
        do
        {
            CLEAR;
            cout << " Nombre: ";
            getline(cin, nombre);
            if (!regex_match(nombre, expNombre))
            {
                cout << " Error, introducir mínimo un nombre y máximo tres "
                     << endl
                     << " (mínimo cuatro letras por nombre)"
                     << endl
                     << " Presione ENTER para continuar"
                     << endl;
                cin.get();
            }
            else
                continuar = true;
        } while (!continuar);
        usuario.setNombre(nombre);
    }
    break;

    case OPC_CAMPO_APE:
    {
        string apellido;
        do
        {
            CLEAR;
            cout << " Apellido: ";
            getline(cin, apellido);
            if (!regex_match(apellido, expApellido))
            {
                cout << " Error, se deben de introducir dos apellidos"
                     << " separados por espacios"
                     << endl
                     << " Presione ENTER para continuar"
                     << endl;
                cin.get();
            }
            else
                continuar = true;
        } while (!continuar);
        usuario.setApellido(apellido);
    }
    break;

    case OPC_CAMPO_EDAD:
    {
        unsigned int edad;
        do
        {
            CLEAR;
            cout << " Edad (de 12 a 115 años): ";
            cin >> edad;
        } while (edad > 115 || edad < 12);
        usuario.setEdad(edad);
    }
    break;

    case OPC_CAMPO_PESO:
    {
        float peso;
        do
        {
            CLEAR;
            cout << " Peso (de 30 a 300 kg): ";
            cin >> peso;
        } while (peso > 300 || peso < 30);
        usuario.setPeso(peso);
        usuario.setMasaCorporal(peso / (usuario.getAltura() * usuario.getAltura()));
    }
    break;

    case OPC_CAMPO_SEXO:
    {
        string genero;
        do
        {
            CLEAR;
            cout << " Género (M = masculino | F = femenino): ";
            cin >> genero;
        } while (!regex_match(genero, expGenero));
        usuario.setGenero(genero[0]);
    }
    break;

    case OPC_CAMPO_TIPO_SANGRE:
    {
        string tipoSangre;
        do
        {
            CLEAR;
            cout << " Tipo de sangre (AB, A, B, O)(+, -): ";
            getline(cin, tipoSangre);
            if (codigo_usado(tipoSangre))
            {
                cout << endl
                     << " Error, código en uso. Presione ENTER para continuar";
                cin.get();
            }
            else if (!regex_match(tipoSangre, expTipoSangre))
            {
                cout << endl
                     << " Error, vuelva a intentarlo" << endl;
                cin.get();
            }
            else
                continuar = true;
        } while (!continuar);
        usuario.setTipoSangre(tipoSangre);
    }
    break;

    case OPC_CAMPO_ALTURA:
    {
        float altura;
        do
        {
            CLEAR;
            cout << " Altura (de 0.8 a 2.5 mts): ";
            cin >> altura;
            ;
        } while (altura < 0.8 || altura > 2.5);
        usuario.setAltura(altura);
        usuario.setMasaCorporal(usuario.getPeso() / (altura * altura));
    }
    break;
    }
}

// INDICES
Gestor::Indice::Indice(const char *cod, long ref)
{
    strcpy(codigo, cod);
    referencia = ref;
}

Gestor::Indice::~Indice()
{
}

bool Gestor::Indice::operator<(const Gestor::Indice &other)
{
    return (strcmp(this->codigo, other.codigo) < 0) ? true : false;
}
bool Gestor::Indice::operator>(const Gestor::Indice &other)
{
    return (strcmp(this->codigo, other.codigo) > 0) ? true : false;
}
bool Gestor::Indice::operator==(const Gestor::Indice &other)
{
    return !strcmp(this->codigo, other.codigo);
}
bool Gestor::Indice::operator<(const string &other)
{
    return string(this->codigo) < other;
}
bool Gestor::Indice::operator>(const string &other)
{
    return string(this->codigo) > other;
}
bool Gestor::Indice::operator==(const string &other)
{
    return string(this->codigo) == other;
}
void Gestor::Indice::operator=(const Indice &other)
{
    strcpy(this->codigo, other.codigo);
    this->referencia = other.referencia;
}

int Gestor::busqueda_binaria(vector<Gestor::Indice> &vec, string dato)
{
    int l = 0;
    int r = int(vec.size() - 1);
    while (l <= r)
    {
        int m = (l + r) / 2;
        if (vec[m] == dato)
            return m;
        else if (vec[m] > dato)
            r = m - 1;
        else
            l = m + 1;
    }
    return -1;
}

void Gestor::actualizarIndices()
{
    Gestor::Indice indiceTmp;
    fstream archivoDatosIndices("indices.bin", ios::out | ios::binary | ios::trunc);

    sort(m_indices.begin(), m_indices.end());

    indiceTmp.referencia = m_posFinal;
    archivoDatosIndices.write((char *)&indiceTmp, sizeof(indiceTmp));

    for (size_t i = 0; i < m_indices.size(); ++i)
    {
        indiceTmp = m_indices[i];
        archivoDatosIndices.write((char *)&indiceTmp, sizeof(indiceTmp));
    }
    archivoDatosIndices.close();
}