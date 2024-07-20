#include "WiFi.h" // ESP32 WiFi include
#include "pico/cyw43_arch.h"
//#include "lwip/apps/sntp.h"
#include "WiFiConfig.h" // My WiFi configuration.


void ledBlink(int repe){

  for (int i = 0; i < repe; i++) {
    cyw43_arch_gpio_put(0, 1);  // Encender el GPIO 0
    sleep_ms(250);              // Esperar 250 ms
    cyw43_arch_gpio_put(0, 0);  // Apagar el GPIO 0
    sleep_ms(250);              // Esperar 250 ms
    }
}

void ConnectToWiFi(){

    delay(2000);
     
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, WiFiPassword);
    Serial.print("Connecting to "); Serial.println(SSID);
    
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(500);
        ledBlink(1);
    
        if ((++i % 10) == 0){
            Serial.println(F(" still trying to connect"));
            WiFi.disconnect(true);
            WiFi.begin(SSID, WiFiPassword);
        }
    }
    
    Serial.print(F("Connected. My IP address is: "));
    Serial.println(WiFi.localIP());
    ledBlink(3);

    delay(1000);
    
}
