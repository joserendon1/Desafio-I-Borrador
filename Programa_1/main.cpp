#include <iostream>
#include <fstream>

int compressRLE(const unsigned char* input, int inputLength, unsigned char* output) {
    if (inputLength == 0) return 0;

    int outputIndex = 0;
    int count = 1;
    unsigned char currentChar = input[0];

    for (int i = 1; i <= inputLength; i++) {
        if (i < inputLength && input[i] == currentChar) {
            count++;
        } else {
            int digits = 0;
            int tempCount = count;
            while (tempCount > 0) {
                digits++;
                tempCount /= 10;
            }

            tempCount = count;
            for (int d = digits - 1; d >= 0; d--) {
                output[outputIndex + d] = '0' + (tempCount % 10);
                tempCount /= 10;
            }
            outputIndex += digits;

            output[outputIndex++] = currentChar;

            if (i < inputLength) {
                currentChar = input[i];
                count = 1;
            }
        }
    }

    return outputIndex;
}

void encryptBuffer(const unsigned char* input, int inputLength,
                   unsigned char* output, int n, unsigned char K) {
    for (int i = 0; i < inputLength; i++) {
        unsigned char rotated = (input[i] << n) | (input[i] >> (8 - n));
        output[i] = rotated ^ K;
    }
}

bool writeToFile(const char* filename, const unsigned char* data, int length) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error: No se pudo crear el archivo " << filename << std::endl;
        return false;
    }

    for (int i = 0; i < length; i++) {
        file.put(static_cast<char>(data[i]));
    }

    file.close();
    return true;
}

bool isValidRotation(int n) {
    return n > 0 && n < 8;
}

int main() {

    int n = 3;
    unsigned char K = 0xAB;

    if (!isValidRotation(n)) {
        std::cout << "Error: n debe estar entre 1 y 7" << std::endl;
        return 1;
    }

    const unsigned char mensaje[] = "vidaantesquemuertefuerzaantesquedebilidadviajeantesquedestino";
    int mensajeLength = sizeof(mensaje) - 1;

    unsigned char comprimido[200] = {0};
    int comprimidoLength = compressRLE(mensaje, mensajeLength, comprimido);

    std::cout << "Mensaje original: " << mensaje << std::endl;
    std::cout << "Comprimido: ";
    for (int i = 0; i < comprimidoLength; i++) {
        std::cout << comprimido[i];
    }
    std::cout << std::endl;

    unsigned char encriptado[200] = {0};
    encryptBuffer(comprimido, comprimidoLength, encriptado, n, K);

    if (writeToFile("mensaje_encriptado.txt", encriptado, comprimidoLength)) {
        std::cout << "Archivo guardado: mensaje_encriptado.txt" << std::endl;
        std::cout << "Longitud: " << comprimidoLength << " bytes" << std::endl;
        std::cout << "Parámetros usados: n=" << n << ", K=0x" << std::hex << (int)K << std::dec << std::endl;
    }

    return 0;
}
