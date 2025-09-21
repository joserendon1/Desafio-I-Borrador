#include <iostream>
#include <fstream>

int decompressRLE(const unsigned char* input, int inputLength, unsigned char* output) {
    if (inputLength == 0) return 0;

    int outputIndex = 0;
    int i = 0;

    while (i < inputLength) {
        int count = 0;
        while (i < inputLength && input[i] >= '0' && input[i] <= '9') {
            count = count * 10 + (input[i] - '0');
            i++;
        }

        if (i < inputLength) {
            unsigned char c = input[i++];
            for (int j = 0; j < count; j++) {
                output[outputIndex++] = c;
            }
        }
    }

    return outputIndex;
}

void decryptBuffer(const unsigned char* input, int inputLength,
                   unsigned char* output, int n, unsigned char K) {
    for (int i = 0; i < inputLength; i++) {
        unsigned char temp = input[i] ^ K;
        output[i] = (temp >> n) | (temp << (8 - n));
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
    while (file.get(c) && length < maxSize) {
        buffer[length++] = static_cast<unsigned char>(c);
    }

    file.close();
    return length;
}

int main() {

    int n = 3;
    unsigned char K = 0xAB;

    unsigned char encriptado[200] = {0};
    int encriptadoLength = readFromFile("mensaje_encriptado.txt", encriptado, 200);

    if (encriptadoLength <= 0) {
        std::cout << "Error: No se pudo leer el archivo o está vacío" << std::endl;
        return 1;
    }

    std::cout << "Archivo leído: " << encriptadoLength << " bytes" << std::endl;

    unsigned char comprimido[200] = {0};
    decryptBuffer(encriptado, encriptadoLength, comprimido, n, K);

    std::cout << "Desencriptado: ";
    for (int i = 0; i < encriptadoLength; i++) {
        std::cout << comprimido[i];
    }
    std::cout << std::endl;

    unsigned char mensajeFinal[200] = {0};
    int mensajeFinalLength = decompressRLE(comprimido, encriptadoLength, mensajeFinal);

    std::cout << "Mensaje final: ";
    for (int i = 0; i < mensajeFinalLength; i++) {
        std::cout << mensajeFinal[i];
    }
    std::cout << std::endl;
    std::cout << "Longitud: " << mensajeFinalLength << " caracteres" << std::endl;

    return 0;
}
