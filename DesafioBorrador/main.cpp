#include <iostream>

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

unsigned char encryptByte(unsigned char byte, int n, unsigned char K) {
    // Rotación a izquierda de n bits
    unsigned char rotated = (byte << n) | (byte >> (8 - n));
    // XOR con clave K
    return rotated ^ K;
}

unsigned char decryptByte(unsigned char byte, int n, unsigned char K) {
    // Revertir XOR primero
    unsigned char temp = byte ^ K;
    // Rotación a derecha de n bits (revertir izquierda)
    return (temp >> n) | (temp << (8 - n));
}

void encryptBuffer(const unsigned char* input, int inputLength,
                   unsigned char* output, int n, unsigned char K) {
    for (int i = 0; i < inputLength; i++) {
        output[i] = encryptByte(input[i], n, K);
    }
}

void decryptBuffer(const unsigned char* input, int inputLength,
                   unsigned char* output, int n, unsigned char K) {
    for (int i = 0; i < inputLength; i++) {
        output[i] = decryptByte(input[i], n, K);
    }
}

bool isValidRotation(int n) {
    return n > 0 && n < 8;
}

void printAsHex(const unsigned char* data, int length) {
    for (int i = 0; i < length; i++) {
        printf("%02X ", data[i]);
    }
    std::cout << std::endl;
}
void printAsChars(const unsigned char* data, int length) {
    for (int i = 0; i < length; i++) {
        std::cout << data[i];
    }
    std::cout << std::endl;
}

int main() {
    const unsigned char testRLE[] = "AYYXXDDFFFFFA";
    int testRLELength = sizeof(testRLE) - 1;

    int n = 3;
    unsigned char K = 0xAB;

    unsigned char compressed[100] = {0};
    int compressedLength = compressRLE(testRLE, testRLELength, compressed);

    std::cout << "Original: ";
    printAsChars(testRLE, testRLELength);

    std::cout << "Comprimido RLE: ";
    printAsChars(compressed, compressedLength);

    unsigned char encrypted[100] = {0};
    encryptBuffer(compressed, compressedLength, encrypted, n, K);

    std::cout << "Encriptado (hex): ";
    printAsHex(encrypted, compressedLength);

    unsigned char decrypted[100] = {0};
    decryptBuffer(encrypted, compressedLength, decrypted, n, K);

    std::cout << "Desencriptado: ";
    printAsChars(decrypted, compressedLength);

    unsigned char final[100] = {0};
    int finalLength = decompressRLE(decrypted, compressedLength, final);

    std::cout << "Rotacion: " << n << std::endl;
    std::cout << "Clave de encriptacion: " << K << std::endl;
    std::cout << "Descomprimido final: ";
    printAsChars(final, finalLength);
    std::cout << "Longitud final: " << finalLength << std::endl;

    return 0;
}
