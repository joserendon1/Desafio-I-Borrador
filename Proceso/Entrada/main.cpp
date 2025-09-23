#include <iostream>

int main()
{
    char n;
    char nombreArchivo[] = "EncriptadoX.txt";
    char nombrePista[] = "pistaX.txt";
    std::cin >> n;
    nombreArchivo[10] = n ;
    nombrePista[5] = n;

    return 0;
}
