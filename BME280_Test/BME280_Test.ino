/*
* Kleines Testprogramm für den BME280 von Bosch.
* 
* Getestet mit  
* - Arduino Ide 2.3.3
* - ESP32 Core 3.0.7
*
* Beginn: 30.10.2024
* Ende:   31.10.2024
*
* Für die LoRaWanGroup im Darc Bernd, DG8BR
*/


#include <Wire.h>
#include <Bosch_BME280_Arduino.h> // Bosch_BME280_Arduino V.1.1.0
#include "SSD1306Wire.h"  // ThingPulse OLED SSD1306 - Library V.4.6.1
                          // https://github.com/ThingPulse/esp8266-oled-ssd1306

uint32_t interval = 300000;
uint32_t letzte_messung = 0;
uint32_t jetzt_zeit = interval;   // Damit wird eine Erstmessung ermoeglicht
float temp = 00.00;
float feucht = 00.00;
float druck = 0000.00;
char tempbuf[35];
char feuchtbuf[35];
char druckbuf[35];

// Die I2C-Anschlüsse sind schon über die Board-Auswahl definiert
 
BME::Bosch_BME280 bme{BME280_I2C_ADDR_PRIM, 249.67F, true};
SSD1306Wire display(0x3c, SDA, SCL); 

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    yield();
  }
  Wire.setPins(SDA, SCL);
  Wire.begin();
  if (bme.begin() != 0)                  // Konfiguration Bosch BME 280 Sensor
  {
     Serial.println("\n\t>>> FEHLER. Init des Sensors <<<");
     while(1);
  }
  display.init();                        // Konfiguration Display
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  display.clear();
  display.drawString(15,22,"BME280");    // Falls man das nicht sieht. Fehler Sensor!!
  display.display();
  delay(3000);
  display.setFont(ArialMT_Plain_10);
  Serial.println();
}

void loop() 
{
  if (jetzt_zeit - letzte_messung >= interval)
  {
    bme.measure();
    Serial.println();

    temp = bme.getTemperature();
    snprintf(tempbuf , 25, "Temperatur:  %.2f °C",temp);
    Serial.println(tempbuf);

    feucht = bme.getHumidity();
    snprintf(feuchtbuf, 28, "Feuchtigkeit: %.2f %%H", feucht);
    Serial.println(feuchtbuf);

    druck = bme.getPressure();
    snprintf(druckbuf, 30, "Luftdruck:     %.2f hPa",druck );
    Serial.println(druckbuf);

    display.clear();
    display.drawString(1, 1, tempbuf);
    display.drawString(1, 12, feuchtbuf);
    display.drawString(1, 24, druckbuf);
    display.display();
    letzte_messung = millis();
  }
  jetzt_zeit = millis();       // Ans Ende es Loops, weil sonst 5 Minuten warten bis erste Messung kommt
}
