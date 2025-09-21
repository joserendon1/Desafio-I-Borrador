#include <iostream>
#include <fstream>

void testDiferencia() {

    const unsigned char valor_original = 200;

    std::cout << "Valor original: " << (int)valor_original << " (0xC8)" << std::endl;
    std::cout << std::endl;

    char con_signo = static_cast<char>(valor_original);
    unsigned char sin_signo = valor_original;

    std::cout << "char:          " << (int)con_signo <<  std::endl;
    std::cout << "unsigned char: " << (int)sin_signo <<  std::endl;
    std::cout << std::endl;

    const unsigned char K = 0xAB;
    const int n = 3;

    std::cout << "operacion xor con clave K=0xAB (171):" << std::endl;

    char xor_con_signo = con_signo ^ static_cast<char>(K);
    unsigned char xor_sin_signo = sin_signo ^ K;

    std::cout << "char XOR:          " << (int)xor_con_signo  << std::endl;
    std::cout << "unsigned char XOR: " << (int)xor_sin_signo  << std::endl;
    std::cout << std::endl;

    char rotacion_con_signo;
    unsigned char rotacion_sin_signo;

    rotacion_sin_signo = (xor_sin_signo >> n) | (xor_sin_signo << (8 - n));
    rotacion_con_signo = (xor_con_signo >> n) | (xor_con_signo << (8 - n));

    std::cout << "char rotacion: " << (int)rotacion_con_signo << std::endl;
    std::cout << "unsigned char rotacion: " << (int)rotacion_sin_signo << std::endl;
    std::cout << std::endl;

    const unsigned char byte_alto = 200;
    char alto_con_signo = static_cast<char>(byte_alto);
    unsigned char alto_sin_signo = byte_alto;

    std::cout << "Probando si el byte es digito:" << std::endl;
    std::cout << "char:          " << (alto_con_signo >= '0' && alto_con_signo <= '9') << std::endl;
    std::cout << "unsigned char: " << (alto_sin_signo >= '0' && alto_sin_signo <= '9') << std::endl;
    std::cout << std::endl;

    const unsigned char digito_real = '5';
    char digito_con_signo = static_cast<char>(digito_real);
    unsigned char digito_sin_signo = digito_real;

    std::cout << "Probando con digito '5':" << std::endl;
    std::cout << "char:          " << (digito_con_signo >= '0' && digito_con_signo <= '9') << std::endl;
    std::cout << "unsigned char: " << (digito_sin_signo >= '0' && digito_sin_signo <= '9') << std::endl;
}

void crearArchivoPrueba() {

    std::ofstream file("prueba_binario.bin", std::ios::binary);

    unsigned char datos[] = {65, 200, 50, 128, 255, 48};
    file.write(reinterpret_cast<const char*>(datos), sizeof(datos));
    file.close();

    std::cout << "Archivo creado con bytes: 65, 200, 50, 128, 255, 48" << std::endl;
}

void leerConDiferentesTipos() {

    std::ifstream file1("prueba_binario.bin", std::ios::binary);
    char buffer_char[6];
    file1.read(buffer_char, 6);
    file1.close();

    std::ifstream file2("prueba_binario.bin", std::ios::binary);
    unsigned char buffer_unsigned[6];
    char temp;
    for (int i = 0; i < 6; i++) {
        file2.get(temp);
        buffer_unsigned[i] = static_cast<unsigned char>(temp);
    }
    file2.close();

    std::cout << "valores:" << std::endl;
    std::cout << "Byte\tchar\tunsigned char" << std::endl;
    std::cout << "----\t----\t-------------" << std::endl;

    for (int i = 0; i < 6; i++) {
        std::cout << i << "\t" << (int)buffer_char[i] << "\t" << (int)buffer_unsigned[i] << std::endl;
    }
}

int main() {
    testDiferencia();
    crearArchivoPrueba();
    leerConDiferentesTipos();

    return 0;
}
