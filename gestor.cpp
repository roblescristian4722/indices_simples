#include "gestor.h"

Gestor::Gestor()
{
    Gestor::Indice actualizado;
    Gestor::Indice indiceTmp;
    string  cadTmp;
    char charTmp;
    unsigned char tam;
    long pos;
    fstream archivoDatos("usuarios.txt", ios::out | ios::in | ios::binary);
    fstream archivoIndices("indices.bin", ios::out | ios::in | ios::binary);

    m_pos = 0;
    if (!archivoDatos.is_open())
        archivoDatos.open("usuarios.txt", ios::out | ios::trunc | ios::binary);

    if (!archivoIndices.is_open())
    {
        indiceTmp.referencia = 0;
        archivoIndices.open("indices.bin", ios::out | ios::trunc | ios::binary);
        archivoIndices.write((char*)&indiceTmp, sizeof(indiceTmp));
    }
    else
    {
        archivoIndices.read((char*)&indiceTmp, sizeof(indiceTmp));
        m_pos = indiceTmp.referencia;
        while (!archivoIndices.eof())
        {
            archivoIndices.read((char*)&indiceTmp, sizeof(indiceTmp));
            if (archivoIndices.eof())
                break;
            
            m_indices.push_back(indiceTmp);
        }    
    }
    archivoIndices.close();
    archivoDatos.close();
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
    long pos;
    char auxChar;
    unsigned char tam;
    string codigo;
    string aux;
    Usuario usuarioTmp;
    fstream archivo("usuarios.txt", ios::in);

    if (!m_indices.size())
    {
        cout << " Aún no se han ingresado usuarios " << endl << endl
             << " Presione ENTER para continuar..." << endl;
        return;
    }

    cout << " Ingrese el código a buscar: ";
    getline(cin, codigo);

    pos = busqueda_binaria(m_indices, codigo);
    if (pos == -1)
    {
        cout << " Lo sentimos, el código no se ha encontrado." << endl
             << " Presione ENTER para continuar..." << endl;
        return;
    }

    archivo.seekg(pos);
    for (int i = 0; i < CANTIDAD_CAMPOS; ++i)
    {
        archivo.read((char*)&tam, sizeof(tam));
        if (archivo.eof())
            break;                

        aux = "";
        for (int j = 0; j < int(tam); ++j)
        {
            archivo.get(auxChar);
            aux += auxChar;
        }
        switch (i)
        {
            case CAMPO_COD:
                usuarioTmp.setCodigo(aux);
            break;
            case CAMPO_ALTURA:
                usuarioTmp.setAltura(stof(aux));
            break;
            case CAMPO_APE:
                usuarioTmp.setApellido(aux);
            break;
            case CAMPO_NOM:
                usuarioTmp.setNombre(aux);
            break;
            case CAMPO_PESO:
                usuarioTmp.setPeso(stof(aux));
            break;
            case CAMPO_SEXO:
                usuarioTmp.setGenero(aux[0]);
            break;
            case CAMPO_EDAD:
                usuarioTmp.setEdad(stoi(aux));
            break;
        }   
    }
    CLEAR;
    cout << endl
            <<" Código: " << usuarioTmp.getCodigo() << endl
            << " Nombre: " << usuarioTmp.getNombre() << endl
            << " Apellido: " << usuarioTmp.getApellido() << endl
            << " Edad: " << usuarioTmp.getEdad() << endl
            << " Género: " << usuarioTmp.getGenero() << endl
            << " Peso: " << usuarioTmp.getPeso() << endl
            << " Altura: " << usuarioTmp.getAltura() << endl
            << "----------------------------------------------"
            << endl << endl
            << " Presione ENTER para continuar..." << endl;
}

void Gestor::capturar(const Usuario& usuario)
{
    string aux = "";
    string aux2 = "";
    unsigned char tam;
    Gestor::Indice indiceTmp;
    fstream archivoDatos("usuarios.txt", ios::out | ios::in | ios::binary | ios::app);

    if (!archivoDatos.is_open())
        cerr << "Error en el archivo de salida" << endl;

    // Se crea y añade el índice en memoria
    strcpy(indiceTmp.codigo, usuario.getCodigo().c_str());
    indiceTmp.referencia = m_pos;
    m_indices.push_back(indiceTmp);
    sort(m_indices.begin(), m_indices.end());

    /// Se añaden los datos del usuario en el archivo de datos ///
    tam = usuario.getCodigo().length();
    archivoDatos.write((char*)&tam, sizeof(tam));
    archivoDatos << usuario.getCodigo();

    tam = usuario.getNombre().length();
    archivoDatos.write((char*)&tam, sizeof(tam));
    archivoDatos << usuario.getNombre();
    
    tam = usuario.getApellido().length();
    archivoDatos.write((char*)&tam, sizeof(tam));
    archivoDatos << usuario.getApellido();
    
    tam = to_string(usuario.getEdad()).length();
    archivoDatos.write((char*)&tam, sizeof(tam));
    archivoDatos << usuario.getEdad();

    tam = CHAR_SIZE;
    archivoDatos.write((char*)&tam, sizeof(tam));
    archivoDatos << usuario.getGenero();

    aux = to_string(usuario.getPeso());
    tam = aux.length();
    archivoDatos.write((char*)&tam, sizeof(tam));
    archivoDatos << aux;

    aux = to_string(usuario.getMasaCorporal());
    tam = aux.length();
    archivoDatos.write((char*)&tam, sizeof(tam));
    archivoDatos << aux;

    tam = usuario.getTipoSangre().length();
    archivoDatos.write((char*)&tam, sizeof(tam));
    archivoDatos << usuario.getTipoSangre();

    aux2 = to_string(usuario.getAltura());
    tam = aux2.length();
    archivoDatos.write((char*)&tam, sizeof(tam));
    archivoDatos << aux2;

    m_pos += archivoDatos.tellg();
    cout << m_pos << endl;

    archivoDatos.close();
    actualizarIndices();
    cout << endl
        << " Usuario añadido exitosamente." << endl
        << " Presione ENTER para continuar..." << endl;
}

void Gestor::eliminar()
{
    bool found = false;
    unsigned int opc;
    unsigned char tam;
    char auxChar;
    string aux;
    Gestor::Indice indiceTmp;
    fstream archivoDatos("usuarios.txt", ios::in | ios::out);
    fstream tmp("usuarios.tmp", ios::out);

    mostrar();
    cout << " Salir = 0" << endl;
    if (!m_indices.size())
    {
        cout << " Aún no se han ingresado datos. Presione ENTER para continuar" << endl;
        return;
    }

    cout << " Ingrese número del usuario a eliminar: ";
    cin >> opc;

    if (opc <= m_indices.size() && opc)
    {
        while (!archivoDatos.eof())
            for (int i = 0; i < CANTIDAD_CAMPOS; i++)
            {
                aux = "";
                archivoDatos.read((char*)&tam, sizeof(tam));
                if (archivoDatos.eof())
                    break;
                
                while(tam--)
                {
                    archivoDatos.get(auxChar);
                    aux += auxChar;
                }

                if (m_indices[opc] == aux && !i)
                    found = true;

                if (found && i == CANTIDAD_CAMPOS - 1)
                    found = false;

                if (!found)
                {
                    tmp.write((char*)&tam, sizeof(tam));
                    tmp << aux;         
                }
            }

        m_indices.erase(m_indices.begin() + opc - 1);
        tmp.close();
        archivoDatos.close();
        actualizarIndices();
        remove("usuarios.txt");
        rename("usuarios.tmp", "usuarios.txt");
    }
    else if (!opc)
        return;
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
    unsigned int codMod;
    char opc;
    char auxChar;
    unsigned char tam;
    bool found = false;
    string aux;
    fstream archivoDatos("usuarios.txt", ios::out | ios::in | ios::binary);
    fstream tmp("usuarios.tmp", ios::out);

    mostrar();

    if (!m_indices.size())
    {
        cout << " Aún no se han ingresado datos. Presione ENTER para continuar" << endl;
        return;
    }

    cout << " Ingrese número del usuario a modificar: ";
    cin >> codMod;

    if (codMod <= m_indices.size() && codMod)
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
                    << char(OPC_CAMPO_TIPO_SANGRE) << ") Tipo de sangre" << endl
                    << char(OPC_CAMPO_ALTURA) << ") Altura" << endl
                    << char(OPC_CAMPO_CANCELAR) << ") Cancelar" << endl
                    << "Opción: ";
            cin >> opc;
        }while(opc < OPC_CAMPO_NOM || opc > OPC_CAMPO_CANCELAR);
        
        if (opc != OPC_CAMPO_CANCELAR)
        {
            archivoDatos.seekg(m_indices[codMod - 1].referencia);
            for (int i = 0; i < CANTIDAD_CAMPOS; ++i)
            {
                archivoDatos.read((char*)&tam, sizeof(tam));
                
                aux = "";
                for (int j = 0; j < int(tam); ++j)
                {
                    archivoDatos.get(auxChar);
                    aux += auxChar;
                }
                
                switch (i)
                {
                    case CAMPO_COD:
                        usuarioTmp.setCodigo(aux);
                        cout << aux << endl;
                    break;
                    case CAMPO_EDAD:
                        usuarioTmp.setEdad(stoi(aux));
                    break;
                    case CAMPO_PESO:
                        usuarioTmp.setPeso(stof(aux));
                    break;
                    case CAMPO_SEXO:
                        usuarioTmp.setGenero(aux[0]);
                    break;
                    case CAMPO_APE:
                        usuarioTmp.setApellido(aux);
                    break;
                    case CAMPO_NOM:
                        usuarioTmp.setNombre(aux);
                    break;
                    case CAMPO_TIPO_SANGRE:
                        usuarioTmp.setTipoSangre(aux);
                    break;
                    case CAMPO_MASA:
                        usuarioTmp.setMasaCorporal(stof(aux));
                    break;
                    case CAMPO_ALTURA:
                        usuarioTmp.setAltura(stof(aux));
                        modificar_datos(usuarioTmp, opc);
                    break;
                }
            }
            
            archivoDatos.seekg(0);
            for (long cont = 0; !archivoDatos.eof(); cont++)
            {
                for (int i = 0; i < CANTIDAD_CAMPOS; ++i)
                {
                    archivoDatos.read((char*)&tam, sizeof(tam));
                    if (archivoDatos.eof())
                        break;

                    aux = "";
                    for (int j = 0; j < int(tam); ++j)
                    {
                        archivoDatos.get(auxChar);
                        aux += auxChar;
                    }
                    if (!i)
                    {
                        m_indices[cont].referencia = long(archivoDatos.tellg()) - long(sizeof(tam)) - long(tam * sizeof(tam));
                        if (aux == usuarioTmp.getCodigo())
                            found = true;
                    }
                    
                    if (found)
                    {
                        switch (i)
                        {
                            case CAMPO_COD:
                                tam = usuarioTmp.getCodigo().length();
                                tmp.write((char*)&tam, sizeof(tam));
                                tmp << usuarioTmp.getCodigo();
                            break;
                            case CAMPO_EDAD:
                                tam = to_string(usuarioTmp.getEdad()).length();
                                tmp.write((char*)&tam, sizeof(tam));
                                tmp << to_string(usuarioTmp.getEdad());
                            break;
                            case CAMPO_PESO:
                                tam = to_string(usuarioTmp.getPeso()).length();
                                tmp.write((char*)&tam, sizeof(tam));
                                tmp << to_string(usuarioTmp.getPeso());
                            break;
                            case CAMPO_SEXO:
                                tam = CHAR_SIZE;
                                tmp.write((char*)&tam, sizeof(tam));
                                tmp << usuarioTmp.getGenero();
                            break;
                            break;
                            case CAMPO_APE:
                                tam = usuarioTmp.getApellido().length();
                                tmp.write((char*)&tam, sizeof(tam));
                                tmp << usuarioTmp.getApellido();
                            break;
                            case CAMPO_NOM:
                                tam = usuarioTmp.getNombre().length();
                                tmp.write((char*)&tam, sizeof(tam));
                                tmp << usuarioTmp.getNombre();
                            break;
                            case CAMPO_TIPO_SANGRE:
                                tam = usuarioTmp.getTipoSangre().length();
                                tmp.write((char*)&tam, sizeof(tam));
                                tmp << usuarioTmp.getTipoSangre();
                            break;
                            case CAMPO_MASA:
                                tam = to_string(usuarioTmp.getMasaCorporal()).length();
                                tmp.write((char*)&tam, sizeof(tam));
                                tmp << to_string(usuarioTmp.getMasaCorporal());
                            break;
                            case CAMPO_ALTURA:
                                tam = to_string(usuarioTmp.getAltura()).length();
                                tmp.write((char*)&tam, sizeof(tam));
                                tmp << to_string(usuarioTmp.getAltura());
                                found = false;
                        }
                    }
                    else
                    {
                        tmp.write((char*)&tam, sizeof(tam));
                        tmp << aux;
                    }
                }   
            }
            tmp.close();
            archivoDatos.close();
            remove("usuarios.txt");
            rename("usuarios.tmp", "usuarios.txt");

            cout << endl
                    << " Dato modificado correctamente" << endl
                    << " Presione ENTER para continuar..." << endl;
        }
    }
    else if (!codMod)
        return;
    else
    {
        cout << endl
                << " Dato inválido, presione ENTER para continuar..."
                << endl;
        cin.get();       
    }
}

void Gestor::mostrar()
{
    fstream archivoDatos("usuarios.txt", ios::in);
    fstream archivoIndices("indices.bin", ios::in | ios::binary);
    Usuario usuarioTmp;
    string cadTmp;
    char charTmp;
    unsigned char tam;

    if (!archivoIndices.is_open() || !archivoDatos.is_open())
    {
        cout << " Error en el archivo de entrada" << endl;
        return;
    }
    for (size_t j = 0; j < m_indices.size(); ++j)
    {
        // Se busca en la posición de memoria que indica el campo
        // referencia del índice
        archivoDatos.seekg(m_indices[j].referencia);

        for (int i = 0; i < CANTIDAD_CAMPOS; ++i)
        {
            cadTmp = "";
            archivoDatos.read((char*)&tam, sizeof(tam));

            while (int(tam--))
            {
                archivoDatos.get(charTmp);
                cadTmp += charTmp;
            }
            switch (i)
            {
                case CAMPO_ALTURA:
                    usuarioTmp.setAltura(stof(cadTmp));
                break;
                case CAMPO_APE:
                    usuarioTmp.setApellido(cadTmp);
                break;
                case CAMPO_NOM:
                    usuarioTmp.setNombre(cadTmp);
                break;
                case CAMPO_COD:
                    usuarioTmp.setCodigo(cadTmp);
                break;
                case CAMPO_PESO:
                    usuarioTmp.setPeso(stof(cadTmp));
                break;
                case CAMPO_SEXO:
                    usuarioTmp.setGenero(cadTmp[0]);
                break;
                case CAMPO_EDAD:
                    usuarioTmp.setEdad(stoi(cadTmp));
                break;
                case CAMPO_MASA:
                    usuarioTmp.setMasaCorporal(stof(cadTmp));
                break;
                case CAMPO_TIPO_SANGRE:
                    usuarioTmp.setTipoSangre(cadTmp);
                break;
            }
        }

        cout << " Usuario #" << j + 1 << endl
            << " Código: " << usuarioTmp.getCodigo() << endl
            << " Nombre: " << usuarioTmp.getNombre() << endl
            << " Apellido: " << usuarioTmp.getApellido() << endl
            << " Edad: "   << usuarioTmp.getEdad() << endl
            << " Género: " << usuarioTmp.getGenero() << endl
            << " Peso: " << usuarioTmp.getPeso() << endl
            << " Masa corporal: " << usuarioTmp.getMasaCorporal() << endl
            << " Tipo de sangre: " << usuarioTmp.getTipoSangre() << endl
            << " Altura: " << usuarioTmp.getAltura() << endl
            << "----------------------------------------------"
            << endl;
    }
    archivoIndices.close();
    archivoDatos.close();
}

void Gestor::capturar_datos(Usuario& usuario)
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
         << endl << endl;

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
    }while(!continuar);

    do
    {
        CLEAR;
        cout << " Edad (de 12 a 115 años): ";
        cin >> edad;
    }while(edad > 115 || edad < 12);

    do
    {
        CLEAR;
        cout << " Género (M = masculino | F = femenino): ";
        cin >> genero;
    } while(!regex_match(genero, expGenero));

    do
    {
        CLEAR;
        cout << " Peso (de 30 a 300 kg): ";
        cin >> peso;
    }while(peso > 300 || peso < 30);

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
    }while(!continuar);

    do
    {   
        CLEAR;
        cout << " Altura (de 0.8 a 2.5 mts): ";
        cin >> altura;;
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

bool Gestor::codigo_usado(const string codigo)
{
    for (int i = 0; i < m_indices.size(); i++)
        if (m_indices[i] == codigo)
            return true;
    return false;
}

void Gestor::modificar_datos(Usuario& usuario, char i)
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
            }while(!continuar);
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
            }while(edad > 115 || edad < 12);
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
            }while(peso > 300 || peso < 30);
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
            } while(!regex_match(genero, expGenero));
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
            }while(!continuar);
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
                cin >> altura;;
            } while (altura < 0.8 || altura > 2.5);
            usuario.setAltura(altura);
            usuario.setMasaCorporal(usuario.getPeso() / (altura * altura));
        }
        break;
    }
}

// INDICES
Gestor::Indice::Indice(const char* cod, long ref)
{
    strcpy(codigo, cod);
    referencia = ref;
}

Gestor::Indice::~Indice()
{}

bool Gestor::Indice::operator < (const Gestor::Indice &other)
{ return (strcmp(this->codigo, other.codigo) < 0) ? true : false; }
bool Gestor::Indice::operator > (const Gestor::Indice &other)
{ return (strcmp(this->codigo, other.codigo) > 0) ? true : false; }
bool Gestor::Indice::operator == (const Gestor::Indice &other)
{ return !strcmp(this->codigo, other.codigo); }
bool Gestor::Indice::operator < (const string &other)
{ return string(this->codigo) < other; }
bool Gestor::Indice::operator > (const string &other)
{ return string(this->codigo) > other; }
bool Gestor::Indice::operator == (const string &other)
{ return string(this->codigo) == other; }
void Gestor::Indice::operator = (const Indice &other)
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
    fstream archivoIndices("indices.bin", ios::out | ios::binary);

    indiceTmp.referencia = m_pos;
    archivoIndices.write((char*)&indiceTmp, sizeof(indiceTmp));

    for (size_t i = 0; i < m_indices.size(); ++i)
    {
        indiceTmp = m_indices[i];
        archivoIndices.write((char*)&indiceTmp, sizeof(indiceTmp));
    }
    archivoIndices.close();
}