
#include <Arduino.h>
#include <time.h>
#include <LittleFS.h>
#include "hardware/rtc.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include "connectivity.h"

void mountLittleFS() {
// Montar el sistema de archivos LittleFS

    if (!LittleFS.begin()) {
        Serial.println("LittleFS mount error.");
        return;
    } else{
        Serial.println("LittleFS mont succesfully.");
    }

}


void createFile(const char* file_path) {
// Función para crear y abrir un archivo para escribir

    File file = LittleFS.open(file_path, "r");

    if (file) {
        // El archivo existe, cerrarlo y eliminarlo
        file.close();
        if (LittleFS.remove(file_path)) {
            Serial.print("Existing file deleted: ");
            Serial.println(file_path);
        } else {
            Serial.print("Error deleting the existing file: ");
            Serial.println(file_path);
        }
    } else {
        Serial.print("File does not exist: ");
        Serial.println(file_path);
    }


    // Crear el archivo
    file = LittleFS.open(file_path, "w");
    if (!file) {
        Serial.println("File creation error");
    }else{
        Serial.println("File succesfully created");
        file.close();
    }

}

void writeFile(const char* file_path, const char* text) {
// Función para escribir datos en el archivo en modo append

    File file = LittleFS.open(file_path, "a");
    if (!file) {
        Serial.println("Error opening the file for writing");
        return;
    }else{
        // Obtener la fecha y hora actual del RTC

        datetime_t timeinfo;
        rtc_get_datetime(&timeinfo);
        //Serial.printf("Hora RTC dentro: %02d/%02d/%04d %02d:%02d:%02d\n", t.day, t.month, t.year, t.hour, t.min, t.sec);
        file.printf("[%02d],%s,\n", timeinfo.hour, text);
  
        /*file.printf("[%04d-%02d-%02d %02d:%02d:%02d],%.5f,%.5f\n",
                timeinfo.year, timeinfo.month, timeinfo.day,
                timeinfo.hour, timeinfo.min, timeinfo.sec, value_df, value_nova);*/
        file.close();
    }

}

float* readFile(const char* file_path, int startLine, int *listSize) {
// Función para leer valores de un archivo a partir de una línea específica

    File file = LittleFS.open(file_path, "r");

    if (!file) {
        Serial.println("Error opening the file");
        return NULL;
    }

    // Reserva inicial de memoria para *listSize elementos
    float* values = (float*)malloc(*listSize * sizeof(float));
    if (values == NULL) {
        Serial.println("Error allocating memory for the list");
        file.close();
        return NULL;
    }

    int i = 0;
    int linesCounter = 0; 
    char line[256]; // Número de caracteres que desea leer de cada linea, Ajusta el tamaño según tus necesidades
    //printf("Valor de startLines %d\n", startLine);

    // Leer cada línea del archivo
    while (file.available()) {
        file.readBytesUntil('\n', line, sizeof(line));
        line[strcspn(line, "\r\n")] = 0; // Eliminar los saltos de línea

        if (linesCounter >= startLine) {
            if (strcmp(line, "nan\n") == 0) {
                values[i] = NAN;  // Representación de NaN en C
            } else {
                sscanf(line, "%f", &values[i]);
            }

            i++;

            /*
            // Si alcanzamos el tamaño máximo de la lista, redimensionamos
            if (i >= *listSize) {
                *listSize *= 2; // Duplicar el tamaño de la lista
                float* temp = (float*)realloc(values, *listSize * sizeof(float));
                if (temp == NULL) {
                    Serial.println("Error reallocating memory for the list");
                    free(values);
                    file.close();
                    return NULL;
                }
                values = temp;
            }
            */

        }
        linesCounter++;
    }

    *listSize = linesCounter - startLine;
    
    file.close();
    //*listSize = i;

    return values;
}

struct DataEntry {
    struct tm timeinfo;
    float value;
};

void value_to_list(float *list, String value, int pos ){
    if (value.equalsIgnoreCase("nan")) {
        list[pos] = NAN;  // Representación de NaN en C
    } else {
        list[pos] = value.toFloat();
    }
}
/*
void value_to_list(float *list, char* value, int pos ){
    if (strcmp(value, "nan") == 0)  {
        list[pos] = NAN;  // Representación de NaN en C
    } else {
        list[pos] = atof(value);
    }
}
*/

// Función para convertir una cadena de fecha y hora a una estructura tm
int parse_date_time(const char *str, struct tm *timeinfo) {
    return sscanf(str, "[%d-%d-%d %d:%d:%d]", 
                  &timeinfo->tm_year, &timeinfo->tm_mon, &timeinfo->tm_mday,
                  &timeinfo->tm_hour, &timeinfo->tm_min, &timeinfo->tm_sec);
}


