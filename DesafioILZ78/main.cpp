#include <iostream>
#include <fstream>

// ==================== FUNCIONES BÁSICAS ====================

unsigned char rotarDerecha(unsigned char byte, int n) {
    n = n % 8;
    if (n == 0) return byte;
    return (byte >> n) | (byte << (8 - n));
}

int calcularLongitud(const char* str) {
    int len = 0;
    while (str[len] != '\0' && len < 1000) {
        len++;
    }
    return len;
}

bool buscarSubcadena(const char* texto, int lenTexto, const char* subcadena, int lenSubcadena) {
    if (lenSubcadena <= 0 || lenTexto < lenSubcadena) return false;

    for (int i = 0; i <= lenTexto - lenSubcadena; i++) {
        bool encontrado = true;
        for (int j = 0; j < lenSubcadena; j++) {
            if (texto[i + j] != subcadena[j]) {
                encontrado = false;
                break;
            }
        }
        if (encontrado) return true;
    }
    return false;
}

// ==================== DESCOMPRESOR LZ78 ====================

void decompressLZ78(const unsigned char* compressed, int compressedLen, char* output, int& outputLen) {
    // Crear diccionario
    char** dict = new char*[65536];
    int* dictLens = new int[65536];
    int dictSize = 1;

    dict[0] = nullptr;
    dictLens[0] = 0;
    outputLen = 0;

    // Procesar tokens (3 bytes cada uno)
    for (int i = 0; i < compressedLen; i += 3) {
        if (i + 2 >= compressedLen) break;

        // Leer token (big-endian)
        int prefixIndex = (compressed[i] << 8) | compressed[i + 1];
        char nextChar = compressed[i + 2];

        // Si el carácter es 0, fin del stream
        if (nextChar == 0) break;

        if (prefixIndex == 0) {
            // Caso especial: nuevo carácter individual
            output[outputLen++] = nextChar;

            // Añadir al diccionario
            if (dictSize < 65536) {
                dictLens[dictSize] = 1;
                dict[dictSize] = new char[1];
                dict[dictSize][0] = nextChar;
                dictSize++;
            }
        } else if (prefixIndex < dictSize) {
            // Obtener el prefijo del diccionario
            int prefixLen = dictLens[prefixIndex];

            // Copiar el prefijo a la salida
            for (int j = 0; j < prefixLen; j++) {
                if (outputLen < 99999) {
                    output[outputLen++] = dict[prefixIndex][j];
                }
            }

            // Añadir el nuevo carácter
            if (outputLen < 99999) {
                output[outputLen++] = nextChar;
            }

            // Añadir nueva entrada al diccionario
            if (dictSize < 65536 && outputLen < 99999) {
                dictLens[dictSize] = prefixLen + 1;
                dict[dictSize] = new char[prefixLen + 1];

                // Copiar prefijo
                for (int j = 0; j < prefixLen; j++) {
                    dict[dictSize][j] = dict[prefixIndex][j];
                }
                // Añadir nuevo carácter
                dict[dictSize][prefixLen] = nextChar;
                dictSize++;
            }
        }

        if (outputLen >= 99999) break; // Prevenir desbordamiento
    }

    output[outputLen] = '\0';

    // Liberar memoria del diccionario
    for (int i = 1; i < dictSize; i++) {
        delete[] dict[i];
    }
    delete[] dict;
    delete[] dictLens;
}

// ==================== DETECCIÓN Y PRUEBA ====================

bool probarCombinacionLZ78(const unsigned char* datosEncriptados, int tamaño,
                           const char* pista, int largoPista,
                           int rotacion, unsigned char clave,
                           char* resultadoFinal, int& posicionEncontrada) {

    if (tamaño <= 0 || largoPista <= 0 || rotacion < 1 || rotacion > 7) {
        return false;
    }

    // Buffer para datos desencriptados
    unsigned char* desencriptado = new unsigned char[tamaño + 1];

    // Desencriptar: XOR primero, luego rotación derecha
    for (int i = 0; i < tamaño; i++) {
        unsigned char byte = datosEncriptados[i];
        byte = byte ^ clave;
        byte = rotarDerecha(byte, rotacion);
        desencriptado[i] = byte;
    }
    desencriptado[tamaño] = '\0';

    // Descomprimir LZ78
    char resultadoLZ78[100000] = {0};
    int lenLZ78 = 0;
    decompressLZ78(desencriptado, tamaño, resultadoLZ78, lenLZ78);

    // Buscar la pista en CUALQUIER posición del texto
    bool coincide = false;
    if (lenLZ78 >= largoPista) {
        if (buscarSubcadena(resultadoLZ78, lenLZ78, pista, largoPista)) {
            coincide = true;

            // Encontrar la posición exacta donde está la pista
            for (int i = 0; i <= lenLZ78 - largoPista; i++) {
                bool match = true;
                for (int j = 0; j < largoPista; j++) {
                    if (resultadoLZ78[i + j] != pista[j]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    posicionEncontrada = i;
                    break;
                }
            }

            // Copiar resultado completo
            int copiarLen = (lenLZ78 < 99999) ? lenLZ78 : 99999;
            for (int i = 0; i < copiarLen; i++) {
                resultadoFinal[i] = resultadoLZ78[i];
            }
            resultadoFinal[copiarLen] = '\0';
        }
    }

    delete[] desencriptado;
    return coincide;
}

// ==================== FUNCIÓN PRINCIPAL ====================

int main() {
    std::cout << "=== SOLUCIONADOR DE DESAFIO (LZ78) ===" << std::endl;
    std::cout << "Buscando parametros n (rotacion) y K (clave XOR)" << std::endl;
    std::cout << "Metodo: LZ78" << std::endl;

    // Leer archivo encriptado
    const char* archivoEncriptado = "Encriptado4.txt";
    std::ifstream archivo(archivoEncriptado, std::ios::binary | std::ios::ate);
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo abrir " << archivoEncriptado << std::endl;
        return 1;
    }

    int tamaño = archivo.tellg();
    archivo.seekg(0, std::ios::beg);
    unsigned char* datosEncriptados = new unsigned char[tamaño];
    archivo.read(reinterpret_cast<char*>(datosEncriptados), tamaño);
    archivo.close();

    std::cout << "Archivo encriptado: " << archivoEncriptado << " (" << tamaño << " bytes)" << std::endl;

    // Leer pista
    const char* archivoPista = "pista4.txt";
    std::ifstream pistaFile(archivoPista);
    if (!pistaFile.is_open()) {
        std::cout << "Error: No se pudo abrir " << archivoPista << std::endl;
        delete[] datosEncriptados;
        return 1;
    }

    char pista[1000] = {0};
    pistaFile.getline(pista, 1000);
    pistaFile.close();

    int largoPista = calcularLongitud(pista);
    std::cout << "Pista a buscar: \"" << pista << "\" (" << largoPista << " caracteres)" << std::endl;
    std::cout << "Nota: La pista puede estar en cualquier posición del texto" << std::endl;

    char resultadoFinal[100000] = {0};
    int rotacionEncontrada = -1;
    unsigned char claveEncontrada = 0;
    int posicionPista = -1;
    bool encontrado = false;

    std::cout << "\n=== BUSCANDO PARAMETROS ===" << std::endl;
    std::cout << "Rango de busqueda:" << std::endl;
    std::cout << "- Rotacion (n): 1 a 7" << std::endl;
    std::cout << "- Clave XOR (K): 0 a 255" << std::endl;
    std::cout << "Total de combinaciones: 7 × 256 = 1,792" << std::endl;
    std::cout << "\nIniciando busqueda..." << std::endl;

    // Probar todas las combinaciones
    for (int rotacion = 1; rotacion <= 7; rotacion++) {
        std::cout << "Probando rotacion " << rotacion << "/7..." << std::endl;

        for (int clave = 0; clave <= 255; clave++) {
            int posicion = -1;
            if (probarCombinacionLZ78(datosEncriptados, tamaño, pista, largoPista,
                                      rotacion, (unsigned char)clave, resultadoFinal, posicion)) {
                rotacionEncontrada = rotacion;
                claveEncontrada = (unsigned char)clave;
                posicionPista = posicion;
                encontrado = true;

                std::cout << "¡COMBINACION ENCONTRADA!" << std::endl;
                std::cout << "Rotacion: " << rotacion << ", Clave: 0x" << std::hex << clave << std::dec << std::endl;
                break;
            }
        }

        if (encontrado) break;
    }

    if (encontrado) {
        std::cout << "\n=== SOLUCION ENCONTRADA ===" << std::endl;
        std::cout << "Parametros de encriptacion:" << std::endl;
        std::cout << "- Rotacion (n): " << rotacionEncontrada << " bits a la derecha" << std::endl;
        std::cout << "- Clave XOR (K): 0x" << std::hex << (int)claveEncontrada << std::dec;
        std::cout << " (" << (int)claveEncontrada << " en decimal)" << std::endl;
        std::cout << "- Metodo de compresion: LZ78" << std::endl;
        std::cout << "- Posicion de la pista: caracter #" << posicionPista + 1 << std::endl;

        int longitudFinal = calcularLongitud(resultadoFinal);
        std::cout << "\n=== TEXTO COMPLETO RECONSTRUIDO ===" << std::endl;
        std::cout << "Longitud: " << longitudFinal << " caracteres" << std::endl;

        // Mostrar contexto alrededor de la pista
        std::cout << "\n=== CONTEXTO DE LA PISTA ===" << std::endl;
        int inicio = (posicionPista - 20 > 0) ? posicionPista - 20 : 0;
        int fin = (posicionPista + largoPista + 20 < longitudFinal) ? posicionPista + largoPista + 20 : longitudFinal;

        std::cout << "...";
        for (int i = inicio; i < fin; i++) {
            std::cout << resultadoFinal[i];
        }
        std::cout << "..." << std::endl;

        // Mostrar texto completo
        std::cout << "\n=== TEXTO COMPLETO ===" << std::endl;
        std::cout << resultadoFinal << std::endl;

        // Guardar resultado
        std::ofstream salida("resultado_lz78.txt");
        salida << resultadoFinal;
        salida.close();
        std::cout << "\nResultado guardado en: resultado_lz78.txt" << std::endl;

    } else {
        std::cout << "\n=== NO SE ENCONTRARON LOS PARAMETROS ===" << std::endl;
        std::cout << "Posibles causas:" << std::endl;
        std::cout << "1. La pista no existe en el texto descomprimido" << std::endl;
        std::cout << "2. El metodo de compresión es RLE en lugar de LZ78" << std::endl;
        std::cout << "3. Los parametros estan fuera de los rangos probados" << std::endl;
        std::cout << "4. El formato del archivo encriptado es diferente" << std::endl;
    }

    delete[] datosEncriptados;
    std::cout << "\n=== FIN ===" << std::endl;
    return 0;
}
