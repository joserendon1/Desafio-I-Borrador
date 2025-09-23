#include <iostream>
#include <fstream>



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

// ==================== DESCOMPRESOR RLE ====================

void decompressRLE(const unsigned char* compressed, int compressedLen, char* output, int& outputLen) {
    outputLen = 0;

    if (compressedLen < 3) return;

    for (int i = 0; i <= compressedLen - 3; i += 3) {
        int length = (compressed[i] << 8) | compressed[i + 1];
        char character = compressed[i + 2];

        if (length <= 0 || length > 1000) continue;

        for (int j = 0; j < length && outputLen < 99999; j++) {
            output[outputLen++] = character;
        }

        if (outputLen >= 99999) break;
    }

    output[outputLen] = '\0';
}

// ==================== DESCOMPRESOR LZ78 ====================

void decompressLZ78(const unsigned char* compressed, int compressedLen, char* output, int& outputLen) {
    char** dict = new char*[65536];
    int* dictLens = new int[65536];
    int dictSize = 1;

    dict[0] = nullptr;
    dictLens[0] = 0;
    outputLen = 0;

    for (int i = 0; i < compressedLen; i += 3) {
        if (i + 2 >= compressedLen) break;

        int prefixIndex = (compressed[i] << 8) | compressed[i + 1];
        char nextChar = compressed[i + 2];

        if (nextChar == 0) break;

        if (prefixIndex == 0) {
            output[outputLen++] = nextChar;

            if (dictSize < 65536) {
                dictLens[dictSize] = 1;
                dict[dictSize] = new char[1];
                dict[dictSize][0] = nextChar;
                dictSize++;
            }
        } else if (prefixIndex < dictSize) {
            int prefixLen = dictLens[prefixIndex];

            for (int j = 0; j < prefixLen; j++) {
                if (outputLen < 99999) {
                    output[outputLen++] = dict[prefixIndex][j];
                }
            }

            if (outputLen < 99999) {
                output[outputLen++] = nextChar;
            }

            if (dictSize < 65536 && outputLen < 99999) {
                dictLens[dictSize] = prefixLen + 1;
                dict[dictSize] = new char[prefixLen + 1];

                for (int j = 0; j < prefixLen; j++) {
                    dict[dictSize][j] = dict[prefixIndex][j];
                }
                dict[dictSize][prefixLen] = nextChar;
                dictSize++;
            }
        }

        if (outputLen >= 99999) break;
    }

    output[outputLen] = '\0';

    for (int i = 1; i < dictSize; i++) {
        delete[] dict[i];
    }
    delete[] dict;
    delete[] dictLens;
}

// ==================== DETECCIÓN AUTOMÁTICA ====================

bool probarCombinacion(const unsigned char* datosEncriptados, int tamaño,
                       const char* pista, int largoPista,
                       int rotacion, unsigned char clave,
                       char* resultadoFinal, int& metodoEncontrado, int& posicionEncontrada) {

    if (tamaño <= 0 || largoPista <= 0 || rotacion < 1 || rotacion > 7) {
        return false;
    }

    // Desencriptar
    unsigned char* desencriptado = new unsigned char[tamaño + 1];
    for (int i = 0; i < tamaño; i++) {
        unsigned char byte = datosEncriptados[i];
        byte = byte ^ clave;
        byte = rotarDerecha(byte, rotacion);
        desencriptado[i] = byte;
    }
    desencriptado[tamaño] = '\0';

    // Probar RLE primero
    char resultadoRLE[100000] = {0};
    int lenRLE = 0;
    decompressRLE(desencriptado, tamaño, resultadoRLE, lenRLE);

    if (lenRLE >= largoPista) {
        if (buscarSubcadena(resultadoRLE, lenRLE, pista, largoPista)) {
            // Encontrar posición
            for (int i = 0; i <= lenRLE - largoPista; i++) {
                bool match = true;
                for (int j = 0; j < largoPista; j++) {
                    if (resultadoRLE[i + j] != pista[j]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    posicionEncontrada = i;
                    break;
                }
            }

            // Copiar resultado
            int copiarLen = (lenRLE < 99999) ? lenRLE : 99999;
            for (int i = 0; i < copiarLen; i++) {
                resultadoFinal[i] = resultadoRLE[i];
            }
            resultadoFinal[copiarLen] = '\0';
            metodoEncontrado = 1; // RLE
            delete[] desencriptado;
            return true;
        }
    }

    // Probar LZ78 si RLE falla
    char resultadoLZ78[100000] = {0};
    int lenLZ78 = 0;
    decompressLZ78(desencriptado, tamaño, resultadoLZ78, lenLZ78);

    if (lenLZ78 >= largoPista) {
        if (buscarSubcadena(resultadoLZ78, lenLZ78, pista, largoPista)) {
            // Encontrar posición
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

            // Copiar resultado
            int copiarLen = (lenLZ78 < 99999) ? lenLZ78 : 99999;
            for (int i = 0; i < copiarLen; i++) {
                resultadoFinal[i] = resultadoLZ78[i];
            }
            resultadoFinal[copiarLen] = '\0';
            metodoEncontrado = 2; // LZ78
            delete[] desencriptado;
            return true;
        }
    }

    delete[] desencriptado;
    return false;
}

// ==================== FUNCIÓN PRINCIPAL ====================

int main() {
    std::cout << "=== SOLUCIONADOR COMPLETO DE DESAFIO ===" << std::endl;
    std::cout << "Deteccion automatica: RLE o LZ78" << std::endl;
    std::cout << "Buscando parametros n (rotacion) y K (clave XOR)" << std::endl;

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
    std::cout << "Pista conocida: \"" << pista << "\" (" << largoPista << " caracteres)" << std::endl;

    char resultadoFinal[100000] = {0};
    int rotacionEncontrada = -1;
    unsigned char claveEncontrada = 0;
    int metodoEncontrado = 0; // 0=no encontrado, 1=RLE, 2=LZ78
    int posicionPista = -1;
    bool encontrado = false;

    std::cout << "\n=== INICIANDO BUSQUEDA AUTOMATICA ===" << std::endl;
    std::cout << "Probando ambos metodos: RLE y LZ78" << std::endl;
    std::cout << "Rango de parametros:" << std::endl;
    std::cout << "- Rotacion (n): 1 a 7" << std::endl;
    std::cout << "- Clave XOR (K): 0 a 255" << std::endl;
    std::cout << "\nBuscando..." << std::endl;

    // Probar todas las combinaciones
    for (int rotacion = 1; rotacion <= 7; rotacion++) {
        std::cout << "Probando rotacion " << rotacion << "/7..." << std::endl;

        for (int clave = 0; clave <= 255; clave++) {
            int posicion = -1;
            int metodo = 0;

            if (probarCombinacion(datosEncriptados, tamaño, pista, largoPista,
                                  rotacion, (unsigned char)clave, resultadoFinal, metodo, posicion)) {
                rotacionEncontrada = rotacion;
                claveEncontrada = (unsigned char)clave;
                metodoEncontrado = metodo;
                posicionPista = posicion;
                encontrado = true;

                std::cout << "¡SOLUCION ENCONTRADA!" << std::endl;
                break;
            }
        }

        if (encontrado) break;
    }

    if (encontrado) {
        std::cout << "\n=== SOLUCION COMPLETA ENCONTRADA ===" << std::endl;
        std::cout << "Parametros de encriptacion:" << std::endl;
        std::cout << "- Rotacion (n): " << rotacionEncontrada << " bits a la derecha" << std::endl;
        std::cout << "- Clave XOR (K): 0x" << std::hex << (int)claveEncontrada << std::dec;
        std::cout << " (" << (int)claveEncontrada << " decimal)" << std::endl;
        std::cout << "- Metodo de compresion: " << (metodoEncontrado == 1 ? "RLE" : "LZ78") << std::endl;
        std::cout << "- Posicion de la pista: caracter #" << posicionPista + 1 << std::endl;

        int longitudFinal = calcularLongitud(resultadoFinal);
        std::cout << "\n=== TEXTO COMPLETO RECONSTRUIDO ===" << std::endl;
        std::cout << "Longitud: " << longitudFinal << " caracteres" << std::endl;

        // Mostrar contexto de la pista
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
        std::string nombreArchivoSalida = "resultado_";
        nombreArchivoSalida += (metodoEncontrado == 1 ? "rle" : "lz78");
        nombreArchivoSalida += ".txt";

        std::ofstream salida(nombreArchivoSalida);
        salida << resultadoFinal;
        salida.close();
        std::cout << "\nResultado guardado en: " << nombreArchivoSalida << std::endl;

    } else {
        std::cout << "\n=== NO SE ENCONTRÓ SOLUCION ===" << std::endl;
        std::cout << "Posibles causas:" << std::endl;
        std::cout << "1. La pista no coincide con el texto" << std::endl;
        std::cout << "2. Los parametros estan fuera del rango probado" << std::endl;
        std::cout << "3. El formato del archivo es diferente" << std::endl;
        std::cout << "4. Se uso un metodo de compresion diferente" << std::endl;
    }

    delete[] datosEncriptados;
    std::cout << "\n=== FIN DEL PROGRAMA ===" << std::endl;
    return 0;
}
