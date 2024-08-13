#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(0));

    // Genera un número aleatorio entre 1 y 100
    int randomNumber = rand() % 100 + 1;

    // Imprime
    printf("Número aleatorio: %d\n", randomNumber);

    return 0;
}
