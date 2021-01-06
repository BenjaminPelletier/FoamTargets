#include "Network.h"

bool connectToAP() {
  WiFi.begin(ssid, password);
  unsigned long t1 = millis() + CONNECTION_ATTEMPT_DURATION_MS;
  while (millis() < t1 && (WiFi.status() == WL_IDLE_STATUS || WiFi.status() == WL_DISCONNECTED)) {
    delay(500);
    Serial.print('.');
  }
  if (WiFi.status() == WL_CONNECTED) {
      return true;
  }
  Serial.println();
  
  if (WiFi.status() == WL_NO_SSID_AVAIL) {
    Serial.println("  No SSID available");
  } else if (WiFi.status() == WL_CONNECT_FAILED) {
    Serial.println("  Password incorrect");
  } else if (WiFi.status() == WL_DISCONNECTED) {
    Serial.println("  Module indicates disconnected");
  } else if (WiFi.status() == WL_IDLE_STATUS) {
    Serial.println("  Module still in idle state");
  } else {
    Serial.println("  Unknown failure connecting to AP");
  }
  return false;
}

void setupAP() {
  WiFi.softAP(ssid, password);
  myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}
