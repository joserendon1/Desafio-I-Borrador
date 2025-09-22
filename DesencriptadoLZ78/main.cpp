#include <iostream>
#include <fstream>

// Función para rotar bits a la derecha
unsigned char rotarDerecha(unsigned char byte, int n) {
    return (byte >> n) | (byte << (8 - n));
}

// Función para leer archivo binario
unsigned char* leerArchivoBinario(const char* nombreArchivo, int& tamaño) {
    std::ifstream archivo(nombreArchivo, std::ios::binary | std::ios::ate);
    if (!archivo.is_open()) {
        std::cout << "Error al abrir: " << nombreArchivo << std::endl;
        tamaño = 0;
        return nullptr;
    }

    tamaño = archivo.tellg();
    archivo.seekg(0, std::ios::beg);

    unsigned char* datos = new unsigned char[tamaño];
    archivo.read(reinterpret_cast<char*>(datos), tamaño);
    archivo.close();

    return datos;
}

int main() {
    std::cout << "=== DESENCRIPTADOR ENCRIPTADO4 ===" << std::endl;

    const char* nombreArchivo = "Encriptado2.txt";
    const int rotacion = 3;
    const unsigned char clave = 0x5A;

    std::cout << "Archivo: " << nombreArchivo << std::endl;
    std::cout << "Parametros: Rotacion=" << rotacion << ", Clave=0x" << std::hex << (int)clave << std::dec << std::endl;

    // Leer archivo
    int tamañoEntrada;
    unsigned char* datosEncriptados = leerArchivoBinario(nombreArchivo, tamañoEntrada);

    if (!datosEncriptados || tamañoEntrada == 0) {
        std::cout << "Error: No se pudo leer el archivo" << std::endl;
        return 1;
    }

    std::cout << "size del archivo: " << tamañoEntrada << " bytes" << std::endl;

    // Crear array para resultado
    unsigned char* resultado = new unsigned char[tamañoEntrada + 1];

    // Desencriptar
    for (int i = 0; i < tamañoEntrada; i++) {
        unsigned char byte = datosEncriptados[i];
        byte = byte ^ clave;                   // Aplicar XOR con 0x40
        byte = rotarDerecha(byte, rotacion);   // Aplicar rotación 3 bits derecha
        resultado[i] = byte;
    }
    resultado[tamañoEntrada] = '\0';  // Añadir terminador

    std::cout << "\n=== DESENCRIPTACION COMPLETADA ===" << std::endl;

    // Mostrar array en formato C (hexadecimal)
    std::cout << "\n=== ARRAY EN HEXADECIMAL ===" << std::endl;
    std::cout << "unsigned char resultado[" << tamañoEntrada + 1 << "] = {";
    for (int i = 0; i < tamañoEntrada; i++) {
        std::cout << "0x";
        if (resultado[i] < 16) std::cout << "0";
        std::cout << std::hex << (int)resultado[i];
        if (i < tamañoEntrada - 1) std::cout << ", ";
        if ((i + 1) % 15 == 0) std::cout << "\n    ";
    }
    std::cout << "};" << std::dec << std::endl;

    // Mostrar array en formato C (caracteres)
    std::cout << "\n=== ARRAY COMO CARACTERES ===" << std::endl;
    std::cout << "char resultado[" << tamañoEntrada + 1 << "] = {";
    for (int i = 0; i < tamañoEntrada; i++) {
        if (resultado[i] >= 32 && resultado[i] <= 126) {
            std::cout << "'" << resultado[i] << "'";
        } else {
            std::cout << "0x";
            if (resultado[i] < 16) std::cout << "0";
            std::cout << std::hex << (int)resultado[i];
        }
        if (i < tamañoEntrada - 1) std::cout << ", ";
        if ((i + 1) % 10 == 0) std::cout << "\n    ";
    }
    std::cout << "};" << std::dec << std::endl;

    // Mostrar vista previa como texto
    std::cout << "\n=== VISTA PREVIA COMO TEXTO ===" << std::endl;
    std::cout << "Primeros 300 caracteres:" << std::endl;
    for (int i = 0; i < 300 && i < tamañoEntrada; i++) {
        if (resultado[i] >= 32 && resultado[i] <= 126) {
            std::cout << resultado[i];
        } else {
            // Mostrar caracteres no imprimibles como hex entre []
            std::cout << "[" << std::hex << (int)resultado[i] << std::dec << "]";
        }
    }
    std::cout << std::endl;

    // Mostrar información estadística
    std::cout << "\n=== ESTADISTICAS ===" << std::endl;
    int letrasMinusculas = 0;
    int letrasMayusculas = 0;
    int digitos = 0;
    int otros = 0;

    for (int i = 0; i < tamañoEntrada; i++) {
        if (resultado[i] >= 'a' && resultado[i] <= 'z') letrasMinusculas++;
        else if (resultado[i] >= 'A' && resultado[i] <= 'Z') letrasMayusculas++;
        else if (resultado[i] >= '0' && resultado[i] <= '9') digitos++;
        else otros++;
    }

    std::cout << "Letras minusculas: " << letrasMinusculas << std::endl;
    std::cout << "Letras mayusculas: " << letrasMayusculas << std::endl;
    std::cout << "Digitos: " << digitos << std::endl;
    std::cout << "Otros caracteres: " << otros << std::endl;

    // Liberar memoria
    delete[] datosEncriptados;
    delete[] resultado;

    std::cout << "\n=== FIN ===" << std::endl;
    return 0;
}
