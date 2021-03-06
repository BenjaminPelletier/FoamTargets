#include "Network.h"

String wlStatusToString(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
    default: return "WL_<UNKNOWN>";
  }
}

bool connectToAP() {
  WiFi.begin(ssid, password);
  unsigned long t1 = millis() + CONNECTION_ATTEMPT_DURATION_MS;
  while (millis() < t1 && (WiFi.status() == WL_IDLE_STATUS || WiFi.status() == WL_DISCONNECTED || WiFi.status() == WL_NO_SHIELD)) {
    delay(500);
    //Serial.print('.');
    Serial.println(wlStatusToString(WiFi.status()));
  }
  if (WiFi.status() == WL_CONNECTED) {
      return true;
  }
  Serial.println();

  Serial.print("  ");
  Serial.println(wlStatusToString(WiFi.status()));
  return false;
}

void setupAP() {
  WiFi.softAP(ssid, password);
  myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}
