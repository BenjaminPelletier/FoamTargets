#ifndef Network_h
#define Network_h

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
bool isAP;
IPAddress myIP;

#endif
