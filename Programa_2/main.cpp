#include <iostream>
#include <fstream>

int decompressRLE(const unsigned char* input, int inputLength, unsigned char* output, int outputMaxSize) {
    if (inputLength == 0) return 0;

    int outputIndex = 0;
    int i = 0;

    while (i < inputLength && outputIndex < outputMaxSize - 1) {
        int count = 0;
        while (i < inputLength && input[i] >= '0' && input[i] <= '9') {
            count = count * 10 + (input[i] - '0');
            i++;
        }

        if (i < inputLength && outputIndex < outputMaxSize - 1) {
            unsigned char c = input[i++];
            for (int j = 0; j < count && outputIndex < outputMaxSize - 1; j++) {
                output[outputIndex++] = c;
            }
        }
    }
    output[outputIndex] = '\0';
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
    while (file.get(c) && length < maxSize - 1) {
        buffer[length++] = static_cast<unsigned char>(c);
    }
    buffer[length] = '\0';
    file.close();
    return length;
}

bool contieneFragmento(const unsigned char* mensaje, int mensajeLength,
                       const unsigned char* fragmento, int fragmentoLength) {
    if (fragmentoLength == 0) return true;
    if (mensajeLength < fragmentoLength) return false;

    for (int i = 0; i <= mensajeLength - fragmentoLength; i++) {
        bool coincide = true;
        for (int j = 0; j < fragmentoLength; j++) {
            if (mensaje[i + j] != fragmento[j]) {
                coincide = false;
                break;
            }
        }
        if (coincide) return true;
    }
    return false;
}

int main() {

    const unsigned char fragmentoConocido[] = "fuerza";
    const int fragmentoLength = 5;
    const int BUFFER_SIZE = 1000;

    unsigned char encriptado[BUFFER_SIZE] = {0};
    int longitud = readFromFile("mensaje_encriptado.txt", encriptado, BUFFER_SIZE);

    if (longitud <= 0) return 1;

    std::cout << "Archivo leido: " << longitud << " bytes" << std::endl;
    std::cout << "Buscando parametros n y K..." << std::endl;
    std::cout << std::endl;

    int intentos = 0;
    const int total_combinaciones = 7 * 256;

    for (int n = 1; n < 8; n++) {
        for (int K = 0; K <= 255; K++) {

            intentos++;
            unsigned char desencriptado[BUFFER_SIZE] = {0};
            decryptBuffer(encriptado, longitud, desencriptado, n, static_cast<unsigned char>(K));

            unsigned char mensajeFinal[BUFFER_SIZE] = {0};
            int longitudFinal = decompressRLE(desencriptado, longitud, mensajeFinal, BUFFER_SIZE);

            if (longitudFinal > 0 && contieneFragmento(mensajeFinal, longitudFinal, fragmentoConocido, fragmentoLength)) {
                std::cout << std::endl << "Parametros encontrados" << std::endl;
                std::cout << "n = " << n << std::endl;
                std::cout << "K = 0x" << std::hex << K << std::dec << " (" << K << " decimal)" << std::endl;
                std::cout << "En el intento: " << intentos << " de " << total_combinaciones << std::endl;

                std::cout << "Mensaje descifrado: ";
                for (int i = 0; i < longitudFinal; i++) std::cout << mensajeFinal[i];
                std::cout << std::endl;

                return 0;
            }

            if (intentos % 100 == 0) {
                std::cout << "Intento " << intentos << "/" << total_combinaciones << std::endl;
            }
        }
    }
    std::cout << std::endl;
    std::cout << "Error: No se encontraron los parametros" << std::endl;
    return 1;
}
