#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiAP.h>
#endif

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
#endif

const char *ssid = "FoamTargets";
const char *password = "foamflinger";

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

void setup() {
  Serial.begin(57600);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(wlStatusToString(WiFi.status()));
  }
  
  Serial.println("Connected.");
}

void loop() {
  delay(1000);
}
