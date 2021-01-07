#ifndef Hardware_h
#define Hardware_h

const int NUM_LEDS = 200;

#ifdef ESP32
  const int PIN_MPU_A0[] = {25, 26, 33};
  // D22 -> SCL
  // D21 -> SDA

  const int PIN_LED = 2;
  const int LED_OFF = LOW;
  const int LED_ON = HIGH;

  const uint8_t PIN_RGB_DATA = 4;
#endif

#ifdef ESP8266
  // NodeMCU 1.0 (ESP-12E Module)
  // https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
  
  const int PIN_MPU_A0[] = {16, 0, 12}; // D0=GPIO16, D3=GPIO0, D6=GPIO12, D7=GPIO13, D8=GPIO15
  const uint8_t PIN_SCL = 5; // D1=GPIO5 -> SCL
  const uint8_t PIN_SDA = 4; // D2=GPIO4 -> SDA
  
  const int PIN_LED = 2; // D4=GPIO2
  const int LED_OFF = HIGH;
  const int LED_ON = LOW;

  const uint8_t PIN_RGB_DATA = 14; // D5=GPIO14 (or D7=GPIO13)
#endif

#endif
