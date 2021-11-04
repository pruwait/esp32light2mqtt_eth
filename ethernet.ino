#ifdef lan
/*
 * Для WT32-ETH01 заняты пины:
 * IO0_RMII_EMAC_REF_CLK
 * IO13_RMII_EMAC_RX_ER
 * IO16_OSC_EN
 * IO18_RMII_MDIO
 * IO19_RMII_EMAC_TXD0
 * IO21_RMII_EMAC_TX_EN
 * IO22_RMII_EMAC_TXD1
 * IO23_RMII_MDC
 * IO25_RMII_EMAC_RXD0
 * IO26_RMII_EMAC_RXD1
 * IO27_RMII_EMAC_CRS_DV
 * 
 * 
 * 
 */


#include <ETH.h>

//для OTA
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

WiFiClient ethClient; //в библиотеке ETH.h он так называется!


// настройки сети
/*
     ETH_CLOCK_GPIO0_IN   - default: external clock from crystal oscillator
     ETH_CLOCK_GPIO0_OUT  - 50MHz clock from internal APLL output on GPIO0 - possibly an inverter is needed for LAN8720
     ETH_CLOCK_GPIO16_OUT - 50MHz clock from internal APLL output on GPIO16 - possibly an inverter is needed for LAN8720
     ETH_CLOCK_GPIO17_OUT - 50MHz clock from internal APLL inverted output on GPIO17 - tested with LAN8720
*/

//сеть
#define ETH_CLK_MODE    ETH_CLOCK_GPIO0_IN

// Pin# of the enable signal for the external crystal oscillator (-1 to disable for internal APLL source)
#define ETH_POWER_PIN   16

// Type of the Ethernet PHY (LAN8720 or TLK110)
#define ETH_TYPE        ETH_PHY_LAN8720

// I²C-address of Ethernet PHY (0 or 1 for LAN8720, 31 for TLK110)
#define ETH_ADDR        1

// Pin# of the I²C clock signal for the Ethernet PHY
#define ETH_MDC_PIN     23

// Pin# of the I²C IO signal for the Ethernet PHY
#define ETH_MDIO_PIN    18
// сеть

static bool eth_connected = false;
unsigned long timeElapsed;

//для OTA
AsyncWebServer serverHTTP(80); 


void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("Light-Controller");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      // webserver
      serverHTTP.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(200, "text/plain", "Hi! I am ESP32. For OTA go to http://<IPAddress>/update in browser.");
      });
      //OTA
      AsyncElegantOTA.begin(&serverHTTP);    // Start ElegantOTA
      serverHTTP.begin();
      Serial.println("HTTP server started");
      
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void setup_eth () {
 Serial.println("START");
  timeElapsed = millis();

  WiFi.onEvent(WiFiEvent);
  ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
}

#endif
