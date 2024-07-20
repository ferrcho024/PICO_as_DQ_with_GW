#include <Arduino.h>
#include <stdio.h>
//#include <stdlib.h>

#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/time.h"
//#include "littlefs-lib/pico_hal.h"

#include "parameters.h"
//#include "file_func.h"
#include "connectivity.h"
#include "dimensions.h"
#include "ntp_client.h" // NTP 
#include "mqtt.h"
#include "sleep.h"
#include "ftoa.h"

// We need our utils functions for calculating MEAN, SD and Normalization
extern "C" {
#include "utils.h"
};

// Set debug info
#define DEBUG 5 //{0: No debug anything, 
                // 1: Debug full DQ and Autoencoder results, 
                // 2: Debug resume DQ only, 
                // 3: Debug resume Autoencoder only,
                // 4: Debug DQ and Autoencoder resume,
                // 5: Debug time and memory}

#define MAX_SIZE 60

// Execution times
char mensaje[200];
int posicion = 0;

// Settings DQ 
//extern char* in_txt;
extern PubSubClient client;
extern String in_txt;
extern bool callback;

//int listSize;  // Tamaño de la lista que almacena los values
//int startline; // Inicializa la lectura desde la línea 0
//int siataValue; // Contador para extraer el valor de la estación SIATA

bool ban = true;
int frec = 1000; // Espacio de tiempo entre los values que llegan (en milisegundos)
float values_df[MAX_SIZE];
float values_nova[MAX_SIZE];
float value_siata;

void value_to_list(float *list, const char* value, int pos ){
    if (strcmp(value, "nan") == 0)  {
        list[pos] = NAN;  // Representación de NaN en C
    } else {
        list[pos] = atof(value);
    }
}

// Función para convertir float a string
void floatToString(float num, char *str, int precision) {
    snprintf(str, precision + 3, "%.*f", precision, num);
}

void task1() {
  int cont = 0; // Variable de conteo de datos recibidos.
  float queue_df[MAX_SIZE];
  float queue_nova[MAX_SIZE];

  /*
  char c = getchar();

  if (pico_mount((c | ' ') == 'y') != LFS_ERR_OK) {
        Serial.printf("Error mounting FS\n");
    }else{
      Serial.printf("Se montó\n");
    }
  */

  callback = true;

  while(true){
    delay(frec/4);
    client.loop();

    if (cont > 59){
      callback = false;
      cont = 0;
      memcpy(values_df, queue_df, sizeof(queue_df));
      memcpy(values_nova, queue_nova, sizeof(queue_nova));
      ban = false;
    }

    if (callback){ 
      ledBlink(1); 

      in_txt += datos_df[cont];
      in_txt += ",";
      in_txt += datos_nova[cont];
      in_txt += ",";
      in_txt += dato_siata;

      //   Encontrar la posición de la primera coma
      int comaPos1 = in_txt.indexOf(',');

      // Extraer el token antes de la coma
      String df_value = in_txt.substring(0, comaPos1);

      int comaPos2 = in_txt.indexOf(',', comaPos1+1);
      String nova_value = in_txt.substring(comaPos1+1,comaPos2);
      
      in_txt = in_txt.substring(comaPos2 + 1);
      value_siata = in_txt.toFloat();

      in_txt = "";

      
    //   if (token != ID){
      /*
      Serial.print(F("Valor DF: "));
      Serial.println(df_value);
      Serial.print(F("Valor NOVA: "));
      Serial.println(nova_value);
      Serial.print(F("Valor SIATA: "));
      Serial.println(value_siata);
      Serial.println(cont);
      */      
    
      value_to_list(queue_df, df_value.c_str(), cont);
      value_to_list(queue_nova, nova_value.c_str(), cont);   

      //callback = false;
      delay(500);
      cont++;
    }
  }
}

void task2() {
  //float dimen[24][10];
  char outlier;
  float mae_loss;
  int decimales = 5;
  size_t listSize;
  static float input_data[MAX_SIZE];
  static float valuesFusioned[MAX_SIZE];
  char str_df[10]; //size of the number
  char str_nova[10]; //size of the number
  char str_siata[10]; //size of the number
  char dato[30]; //size of the number

  float p_com_df;   
  float p_com_nova;
  float uncer;
  float p_df;
  float p_nova;
  float a_df;
  float a_nova;
  float concor;
  float fusion;
  float DQIndex;
  float valueNorm; // value for normalized values as autoencoder input
  float acum; // Acumulator for Read predicted y value from output buffer (tensor)
  float pred_vals; // Value predicted for the Autoencoder

  #if DEBUG == 2
    Serial.print(F("\nPcomp_df,Pcomp_nova,Ppres_df,Ppres_nova,Pacc_df,Pacc_nova,Puncer,Pconcor,Pfusion,PDQIndex\n"));
  #endif

  #if DEBUG == 3
    Serial.print(F("\nPvalue,POUTLIER,Pmae\n"));
  #endif

  #if DEBUG == 5
    //Serial.print(F("t_beforeDQ,mem_beforeDQ,t_afertDQ,mem_afertDQ,t_initAuto,mem_initAuto,t_finAuto,mem_finAuto\n"));
    Serial.print(F("\nPt_beforeDQ,Pt_afertDQ,Pt_initAuto,Pt_finAuto\n"));
  #endif

  while (true) {

    delay(frec/2);

    if(!ban){

      enter_sleep_mode(6);
      //sleep_ms(2000); 

      #if DEBUG == 1
        Serial.print(F("\nTarea 2 ejecutándose en el núcleo 2\n"));
      #endif
      ban = true;

      listSize = sizeof(values_df) / sizeof(values_df[0]);
      //listSize = sizeof(values_nova)/4;

      #if DEBUG == 5
        // t_beforeDQ and mem_beforeDQ
        posicion += sprintf(mensaje + posicion, "%lu", time_us_32());
        mensaje[posicion++] = ',';
        //posicion += sprintf(mensaje + posicion, "%u", heap_get_free_size());
        //mensaje[posicion++] = ',';
      #endif

      p_com_df = completeness(values_df, listSize);   
      p_com_nova = completeness(values_nova, listSize);
      uncer = uncertainty(values_df, values_nova, listSize);
      p_df = precision(values_df, listSize);
      p_nova = precision(values_nova, listSize);
      a_df = accuracy(values_df, value_siata, listSize);
      a_nova = accuracy(values_nova, value_siata, listSize);
      concor = PearsonCorrelation(values_df, values_nova, listSize);
      plausability(p_com_df, p_com_nova, p_df, p_nova, a_df, a_nova, values_df, values_nova, listSize, valuesFusioned);
      fusion = calculateMean(valuesFusioned, listSize);
      DQIndex = DQ_Index(valuesFusioned, uncer, concor, value_siata, listSize);

      #if DEBUG == 5
        // t_afterDQ and mem_afterDQ
        posicion += sprintf(mensaje + posicion, "%lu", time_us_32());
        mensaje[posicion++] = ',';
        //posicion += sprintf(mensaje + posicion, "%u", heap_get_free_size());
        //mensaje[posicion++] = ',';
      #endif

      //Serial.print(F("Inicio Pausa\n"));
      //enter_sleep_mode(3);
      //Serial.print(F("Fin Pausa\n"));


      #if DEBUG == 1
        Serial.print(F("\n**********************************************\n"));
        Serial.print(F("********** Completeness DF: "));
        Serial.println(p_com_df, decimales);
        Serial.print(F("********** Completeness NOVA: "));
        Serial.println(p_com_nova, decimales);
        Serial.print(F("********** Uncertainty: "));
        Serial.println(uncer, decimales);
        Serial.print(F("********** Precision DF: "));
        Serial.println(p_df, decimales);
        Serial.print(F("********** Precision NOVA: "));
        Serial.println(p_nova, decimales);
        Serial.print(F("********** Accuracy DF: "));
        Serial.println(a_df, decimales);
        Serial.print(F("********** Accuracy NOVA: "));
        Serial.println(a_nova, decimales);
        Serial.print(F("********** Concordance: "));
        Serial.println(concor, decimales);
        Serial.print(F("********** Value Fusioned: "));
        Serial.println(fusion, decimales);
        Serial.print(F("********** DQ Index: "));
        Serial.println(DQIndex, decimales);
      #endif

      #if (DEBUG == 2) || (DEBUG == 4)
        Serial.print(p_com_df, decimales);
        Serial.print(F(","));
        Serial.print(p_com_nova, decimales);
        Serial.print(F(","));
        Serial.print(p_df, decimales);
        Serial.print(F(","));
        Serial.print(p_nova, decimales);
        Serial.print(F(","));
        Serial.print(a_df, decimales);
        Serial.print(F(","));
        Serial.print(a_nova, decimales);
        Serial.print(F(","));
        Serial.print(uncer, decimales);
        Serial.print(F(","));
        Serial.print(concor, decimales);
        Serial.print(F(","));
        Serial.print(fusion, decimales);
        Serial.print(F(","));
        Serial.println(DQIndex, decimales);
      #endif


      //String mqtt_msg = String(ID) + "," + String(fusion, decimales) + ",distancia," + String(DQIndex, decimales);
      //client.publish(TOPIC.c_str(), mqtt_msg);

      /*
      char mqtt_msg[50];
      sprintf(mqtt_msg, "%s,%.5f,distancia,%.5f",ID,fusion,DQIndex);
      client.publish(TOPIC.c_str(), mqtt_msg);

      char resultString[50];
      sprintf(resultString, "%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f",p_com_df,p_com_nova,p_df,p_nova,a_df,a_nova,uncer,concor,fusion,DQIndex);
      //writeFile(dimensions, resultString);
      dimen[siataValue%24][0] = p_com_df;
      dimen[siataValue%24][1] = p_com_nova;
      dimen[siataValue%24][2] = p_df;
      dimen[siataValue%24][3] = p_nova;
      dimen[siataValue%24][4] = a_df;
      dimen[siataValue%24][5] = a_nova;
      dimen[siataValue%24][6] = uncer;
      dimen[siataValue%24][7] = concor;
      dimen[siataValue%24][8] = fusion;
      dimen[siataValue%24][9] = DQIndex;

      //read_data_from_file("/spiffs/data.txt"); // Lee el archivo con formato de hora y valor float

      */

      //Serial.println("Antes invoke_status");

      #if DEBUG == 5
        // t_initAuto and mem_initAuto
        posicion += sprintf(mensaje + posicion, "%lu", time_us_32());
        mensaje[posicion++] = ',';
        //posicion += sprintf(mensaje + posicion, "%u", heap_get_free_size());
        //mensaje[posicion++] = ',';
      #endif

      #if DEBUG == 5
        // t_finAuto and mem_finAuto
        posicion += sprintf(mensaje + posicion, "%lu", time_us_32());
        //mensaje[posicion++] = ',';
        //posicion += sprintf(mensaje + posicion, "%u", heap_get_free_size());
        mensaje[posicion] = '\0';
        Serial.println(mensaje);
        mensaje[0] = '\0';
        posicion = 0;
      #endif
      
      enter_sleep_mode(6);

      ConnectToWiFi();
      //run_ntp_test();
      createMQTTClient();

      delay(500);

      ftoa(value_siata, str_siata, 5);

      for (int i = 0; i < listSize; i++ ){

        
        ftoa(values_df[i], str_df, 5);
        ftoa(values_nova[i], str_nova, 5);
        sprintf(dato, "%s,%s,%s,%s", ID.c_str(), str_df, str_nova, str_siata);
        client.publish((TOPIC + 1).c_str(), dato); 
        
        ledBlink(1);
        
        delay(500);
      }

      client.disconnect();
      WiFi.disconnect();

      enter_sleep_mode(6);

      callback = true;

      //Serial.print("Fin del código en núcleo 2\n");
    }

  }

    //printf("************ Free Memory task 2: %u bytes\n", esp_get_free_heap_size());

}

void setup() {
  //stdio_init_all();
  Serial.begin(115200);

  // Configurar y conectar WiFi
  ConnectToWiFi();

  // Obtener hora de servidor NTP y actualizar el RTC
  run_ntp_test();

  WiFi.disconnect(true);
  
  //mountLittleFS();
  //createFile(data); // Archivo de memoria permanente 
  //createFile(dimensions); // Archivo que almacena las métricas cada hora 
  //writeFile(dimensions, "hora,comp_df,comp_nova,prec_df,prec_nova,acc_df,acc_nova,uncer,concor");
  //writeFile(data, "fechaHora,pm25df,pm25nova");
  //createMQTTClient();
  //connectMQTTBroker();


  multicore_launch_core1(task2);
  task1();

  // Crea dos tareas y las asigna a diferentes núcleos
  //xTaskCreatePinnedToCore(task1, "Task1", 10000, NULL, 1, NULL, 0);
  //xTaskCreatePinnedToCore(task2, "Task2", 10000, NULL, 1, NULL, 1);

  // Iniciar el planificador de tareas
  //vTaskStartScheduler();
}

void loop() {
  //reconnectMQTTClient();
  //client.loop();
  //delay(1000);
}