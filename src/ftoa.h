#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

// Función para invertir una cadena
void reverse(char* str, int len) {
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Función para convertir un entero a cadena
int intToStr(int x, char str[], int d) {
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
    while (i < d)
        str[i++] = '0';
    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Función para verificar si un float es NaN
bool isNan(float n) {
    return n != n;
}

// Función para convertir un float a cadena
void ftoa(float n, char* res, int afterpoint) {
    // Verificar si el número es NaN
    if (isNan(n)) {
        strcpy(res, "nan");
        return;
    }

    int ipart = (int)n;
    float fpart = n - (float)ipart;

    // Convertir la parte entera a cadena
    int i = intToStr(ipart, res, 0);

    // Verificar si se necesita precisión después del punto decimal
    if (afterpoint != 0) {
        res[i] = '.';
        fpart = fpart * pow(10, afterpoint);
        intToStr((int)fpart, res + i + 1, afterpoint); 
    }
}