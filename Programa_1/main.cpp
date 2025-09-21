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

int readFromFile(const char* filename, unsigned char* buffer, int maxSize) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo " << filename << std::endl;
        return -1;
    }

    int length = 0;
    char c;
    while (file.get(c) && length < maxSize - 1) {
        buffer[length++] = static_cast<unsigned char>(c);
    }
    buffer[length] = '\0';
    file.close();
    return length;
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

int main() {

    int n = 3;
    unsigned char K = 0x5A;

    const int BUFFER_SIZE = 3000;
    unsigned char mensaje[BUFFER_SIZE] = {0};
    int mensajeLength = readFromFile("mensaje_original.txt", mensaje, BUFFER_SIZE);

    if (mensajeLength <= 0) {
        std::cout << "Error: No se pudo leer el archivo mensaje_original.txt" << std::endl;
        std::cout << "Asegurese de que el archivo exista en la misma carpeta" << std::endl;
        return 1;
    }

    std::cout << "PARAMETROS USADOS:" << std::endl;
    std::cout << "n = " << n << std::endl;
    std::cout << "K = 0x" << std::hex << (int)K << std::dec << " (" << (int)K << " decimal)" << std::endl;
    std::cout << "Mensaje leido: " << mensajeLength << " caracteres" << std::endl;
    std::cout << std::endl;

    unsigned char comprimido[BUFFER_SIZE] = {0};
    int comprimidoLength = compressRLE(mensaje, mensajeLength, comprimido);

    std::cout << "Mensaje original: ";
    for (int i = 0; i < mensajeLength; i++) {
        std::cout << mensaje[i];
    }
    std::cout << std::endl;

    std::cout << "Comprimido RLE: ";
    for (int i = 0; i < comprimidoLength; i++) {
        std::cout << comprimido[i];
    }
    std::cout << std::endl;

    unsigned char encriptado[BUFFER_SIZE] = {0};
    encryptBuffer(comprimido, comprimidoLength, encriptado, n, K);

    if (writeToFile("mensaje_encriptado.txt", encriptado, comprimidoLength)) {
        std::cout << std::endl << "ARCHIVO ENCRIPTADO GUARDADO:" << std::endl;
        std::cout << "Nombre: mensaje_encriptado.txt" << std::endl;
        std::cout << "Longitud: " << comprimidoLength << " bytes" << std::endl;
    }

    return 0;
}
