// LIBRARIES ===================================================================
// ESP8266 for Arduino. (https://github.com/esp8266/arduino)
#include <ESP8266WiFi.h>

// Seven segment display. (https://github.com/bremme/arduino-tm1637)
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"

// NTPClient, Fabrice Weinberg. (https://github.com/arduino-libraries/NTPClient)
#include <NTPClient.h>
#include <WiFiUdp.h>

// IPGeolocation (https://github.com/dushyantahuja/IPGeolocation)
#include "IPGeolocation.h"

// DECLARATIONS AND CONSTANTS ==================================================
const String WIFI_SSID = "";
const String WIFI_PASS = "";
const String IPGEOLOCATION_KEY = "";
const char* NTP_POOL = "nl.pool.ntp.org";
const unsigned long NTP_POOL_INTERVAL = 300000;

const byte PIN_D_CLK = 5;
const byte PIN_D_DIO = 4;
SevenSegmentExtended display(PIN_D_CLK, PIN_D_DIO);

int tz_offset_seconds = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_POOL, tz_offset_seconds, NTP_POOL_INTERVAL);

IPGeolocation location(IPGEOLOCATION_KEY);
IPGeo IPG;

// SETUP =======================================================================
void setup() {
  // Serial communication
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESPClock booting up...");
  Serial.println("Copyright Tom Wishaupt, 2020.");
  Serial.print("Connecting to WiFi.");
  // Initialize display
  display.begin();
  display.print("BOOT");
  for (int i=0; i<= 100; i += 5) {
    display.setBacklight(i);
    delay(100);
  }
  display.print("CONN");
  // Start WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  display.print("INIT");
  timeClient.begin();
  // Get timezone
  Serial.println("Retrieving timezone...");
  location.updateStatus(&IPG);
  Serial.print("Timezone: ");
  Serial.println(IPG.tz);
  tz_offset_seconds = IPG.offset * 60 * 60;
  timeClient.setTimeOffset(tz_offset_seconds);
  // Update time, wait until success
  display.print("SYNC");
  Serial.println("Updating time...");
  while (timeClient.update() == false) {
    delay(500);
  }
}

// LOOP ========================================================================
void loop() {
  byte hours = timeClient.getHours();
  byte minutes = timeClient.getMinutes();
  byte seconds = timeClient.getSeconds();
  display.printTime(hours, minutes, false);
  if ((minutes == 30 or minutes == 0) and seconds == 0 ) {
    Serial.println("Updating time...");
    timeClient.update();
  }
  Serial.println(timeClient.getFormattedTime());
  delay(1000);
}
