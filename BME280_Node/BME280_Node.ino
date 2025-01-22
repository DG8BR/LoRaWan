/*
* Programm für den BME280 von Bosch.
* Die Daten werden an einen TTN-Server geschickt.
* Der Payload-Formatter ist am Ende des Code.
* 
* Getestet mit  
* - Arduino Ide 2.3.4
* - ESP32 Core 3.1.1
* Einstellungen in der IDE: TTGO LoRa32-OLED V2.1(1.6.1)
*
* Beginn: 01.11.2024
* Ende:   18.11.2024
*
* Für die LoRaWanGroup im Darc 
* Bernd, DG8BR
*/

#include <Wire.h>
#include <Bosch_BME280_Arduino.h>  // Bosch_BME280_Arduino V.1.1.0
#include <SSD1306Wire.h>           // ThingPulse OLED SSD1306 - Library V.4.6.1
                                   // https://github.com/ThingPulse/esp8266-oled-ssd1306
#include <lmic.h>                  // LMIC library from the libary-manager V.5.0.1
#include <hal/hal.h>               // ist in the lmic library
#include "keys.h"

static osjob_t sendjob;

#define AUSGABE        // Wenn eine Ausgabe gewünscht, "//" loeschen. 
                         // Macht aber im Betrieb keinen Sinn
// Pin mapping
const lmic_pinmap lmic_pins = {
  // Pin mapping zum LoRaWAN-TRX-Modul
  .nss = 18,
  .rxtx = LMIC_UNUSED_PIN,  // gilt für TTGO LORA32 T3_V1.6.1 20210104 und T3_V1.6 20180606
  .rst = 23,
  .dio = { 26, 33, 32 }
};

const uint16_t TX_INTERVAL=300;           // Sendeintervall in Sekunden

// Die I2C-Anschlüsse sind schon über die Board-Auswahl definiert
SSD1306Wire display(0x3c, SDA, SCL);
BME::Bosch_BME280 bme{BME280_I2C_ADDR_PRIM, 249.67F, true };

void setup()
{
  #ifdef AUSGABE
  Serial.begin(19200);
  while (!Serial)
  {
    yield();
  }
  #endif

  Wire.setPins(SDA, SCL);
  Wire.begin();

  if (bme.begin() != 0)  // Konfiguration des Bosch BME 280 Sensor
  {
    #ifdef AUSGABE
    Serial.println("\n\t>>> FEHLER. Init des Sensors <<<");
    #endif
    while (1);
  }
  display.init();        // Konfiguration Display
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  display.clear();
  display.drawString(15, 22, "BME280");  // Falls man das nicht sieht. Fehler Sensor!!
  display.display();
  display.setFont(ArialMT_Plain_10);
  delay(2000);           // Damit man was sieht
  #ifdef AUSGABE
  Serial.println();
  #endif

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  os_init();

  LMIC_reset();

  do_send(&sendjob);
}

void loop()
{
  os_runloop_once();
  yield();
}

/*****************************************
 * LoRaWAN-Datenpaket aussenden
 */
void do_send(osjob_t* j) 
{
  volatile uint32_t zwischen = 0;
  uint8_t sendebuf[7] = {0x00};
  float temp = 00.00;
  float feucht = 00.00;
  float druck = 0000.00;
  char tempbuf[30] = {'\0'};
  char feuchtbuf[30] = {'\0'};
  char druckbuf[30] = {'\0'};

  bme.measure();
  #ifdef AUSGABE
  Serial.println();
  #endif

  temp = bme.getTemperature();
  snprintf(tempbuf, 29, "Temperatur: %.2f °C", temp);
  #ifdef AUSGABE
  Serial.println(tempbuf);
  #endif

  feucht = bme.getHumidity();
  snprintf(feuchtbuf, 29, "Feuchtigkeit: %.2f %%H", feucht);
  #ifdef AUSGABE
  Serial.println(feuchtbuf);
  #endif

  druck = bme.getPressure();
  snprintf(druckbuf, 29, "Luftdruck:    %.2f hPa", druck);
  #ifdef AUSGABE
  Serial.println(druckbuf);
  #endif

  zwischen = (temp * 100) + 3000;
  sendebuf[0] = zwischen >> 8;
  sendebuf[1] = zwischen;
  
  zwischen = feucht * 100;
  sendebuf[2] = zwischen >> 8;
  sendebuf[3] = zwischen;

  zwischen = druck * 100;
  sendebuf[4] = zwischen >> 16;
  sendebuf[5] = zwischen >> 8;
  sendebuf[6] = zwischen;

  display.clear();
  display.drawString(1, 1, tempbuf);
  display.drawString(1, 12, feuchtbuf);
  display.drawString(1, 24, druckbuf);
  display.display();
  zwischen = 0;

  // Check if there is not a current TX/RX job running

  if (LMIC.opmode & OP_TXRXPEND)
  {
    #ifdef AUSGABE
    Serial.println(F("OP_TXRXPEND, not sending"));
    #endif
  }
  else
  {
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, sendebuf, sizeof(sendebuf), 0);
    #ifdef AUSGABE
    Serial.println(F("Packet queued"));
    #endif
    // Next TX is scheduled after TX_COMPLETE event.
  }
}

/***********************************************
 * Payload-Formatter für TTN

------------------------------------------------
function Decoder(bytes, port) {

    var decode = {};
     decode.Temperatur = ( (bytes[0] << 8 | bytes[1]) - 3000) / 100;
     decode.Feuchtigkeit = (bytes[2] << 8 | bytes[3]) / 100;
     decode.Luftdruck = (bytes[4] << 16 | bytes[5] << 8 | bytes[6]) / 100;
     
     decode.metadata = []
     decode.metadata[0] = 
     {
       titel: "Zimmer",
       einheit: "&deg:C",
       label: "Temperatur"
     };
     decode.metadata[0] = 
     {
       titel: "Zimmer",
       einheit: "%",
       label: "Feuchtigkeit"
     };
     decode.metadata[0] = 
     {
       titel: "Zimmer",
       einheit: "mbar",
       label: "TLuftdruck"
     };

//decode.bytes = bytes; // Debugzwecke
    return decode;
}


------------------------------------------------*/
