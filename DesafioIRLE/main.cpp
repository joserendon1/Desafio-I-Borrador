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

bool probarCombinacionRLE(const unsigned char* datosEncriptados, int tamaño,
                          const char* pista, int largoPista,
                          int rotacion, unsigned char clave,
                          char* resultadoFinal, int& posicionEncontrada) {

    if (tamaño <= 0 || largoPista <= 0 || rotacion < 1 || rotacion > 7) {
        return false;
    }

    unsigned char* desencriptado = new unsigned char[tamaño + 1];

    for (int i = 0; i < tamaño; i++) {
        unsigned char byte = datosEncriptados[i];
        byte = byte ^ clave;
        byte = rotarDerecha(byte, rotacion);
        desencriptado[i] = byte;
    }
    desencriptado[tamaño] = '\0';

    char resultadoRLE[100000] = {0};
    int lenRLE = 0;
    decompressRLE(desencriptado, tamaño, resultadoRLE, lenRLE);

    bool coincide = false;
    if (lenRLE >= largoPista) {
        if (buscarSubcadena(resultadoRLE, lenRLE, pista, largoPista)) {
            coincide = true;

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

            int copiarLen = (lenRLE < 99999) ? lenRLE : 99999;
            for (int i = 0; i < copiarLen; i++) {
                resultadoFinal[i] = resultadoRLE[i];
            }
            resultadoFinal[copiarLen] = '\0';
        }
    }

    delete[] desencriptado;
    return coincide;
}

int main() {
    std::cout << "=== SOLUCIONADOR DE DESAFIO (RLE - PISTA INTERNA) ===" << std::endl;
    std::cout << "La pista puede estar en cualquier parte del texto" << std::endl;

    const char* archivoEncriptado = "Encriptado1.txt";
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

    const char* archivoPista = "pista1.txt";
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

    for (int rotacion = 1; rotacion <= 7; rotacion++) {
        std::cout << "Probando rotacion " << rotacion << "/7..." << std::endl;

        for (int clave = 0; clave <= 255; clave++) {
            int posicion = -1;
            if (probarCombinacionRLE(datosEncriptados, tamaño, pista, largoPista,
                                     rotacion, (unsigned char)clave, resultadoFinal, posicion)) {
                rotacionEncontrada = rotacion;
                claveEncontrada = (unsigned char)clave;
                posicionPista = posicion;
                encontrado = true;

                std::cout << "¡ENCONTRADO! Rotacion: " << rotacion
                          << ", Clave: " << (int)clave << std::endl;
                break;
            }
        }

        if (encontrado) break;
    }

    if (encontrado) {
        std::cout << "\n=== SOLUCION ENCONTRADA ===" << std::endl;
        std::cout << "Parametros de encriptacion:" << std::endl;
        std::cout << "- Rotacion (n): " << rotacionEncontrada << " bits a la derecha" << std::endl;
        std::cout << "- Clave XOR (K): " << (int)claveEncontrada << " (0x" << std::hex << (int)claveEncontrada << std::dec << ")" << std::endl;
        std::cout << "- Metodo de compresion: RLE" << std::endl;
        std::cout << "- Posicion de la pista: caracter #" << posicionPista + 1 << std::endl;

        int longitudFinal = calcularLongitud(resultadoFinal);
        std::cout << "\n=== TEXTO COMPLETO RECONSTRUIDO ===" << std::endl;
        std::cout << "Longitud total: " << longitudFinal << " caracteres" << std::endl;

        std::cout << "\n=== CONTEXTO DE LA PISTA ===" << std::endl;
        int inicio = (posicionPista - 20 > 0) ? posicionPista - 20 : 0;
        int fin = (posicionPista + largoPista + 20 < longitudFinal) ? posicionPista + largoPista + 20 : longitudFinal;

        std::cout << "...";
        for (int i = inicio; i < fin; i++) {
            std::cout << resultadoFinal[i];
        }
        std::cout << "..." << std::endl;

        std::cout << "\n=== TEXTO COMPLETO ===" << std::endl;
        std::cout << resultadoFinal << std::endl;

        std::ofstream salida("resultado_final.txt");
        salida << resultadoFinal;
        salida.close();
        std::cout << "\nResultado guardado en: resultado_final.txt" << std::endl;

    } else {
        std::cout << "\nNo se encontró la combinación correcta." << std::endl;
        std::cout << "Posibles causas:" << std::endl;
        std::cout << "1. La pista no existe en el texto descomprimido" << std::endl;
        std::cout << "2. El método de compresión es LZ78 en lugar de RLE" << std::endl;
        std::cout << "3. Los parámetros están fuera de los rangos probados" << std::endl;
    }

    delete[] datosEncriptados;
    std::cout << "\n=== FIN ===" << std::endl;
    return 0;
}
