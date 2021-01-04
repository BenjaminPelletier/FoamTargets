#include "Endpoints.h"

void setupEndpoints() {
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);
  server.begin();
}

void handlingRequest(bool ok) {
  leds[36] = ok ? CRGB::Green : CRGB::Red;
  FastLED.show();
}

void handledRequest() {
  leds[36] = CRGB::Black;
  FastLED.show();
}

void handleRoot() {
  handlingRequest(true);
  server.send(200, "text/html", "<html><title>FoamTarget</title><body>FoamTarget<br><a href=\"status\">Status</a></body></html>");
  handledRequest();
}

void handleStatus() {
  handlingRequest(true);
  String message = "<html><body>\n";
  message += "<p>Accelerometer targets\n<ul>\n";
  for (int m = 0; m < MPU_COUNT; m++) {
    message += "<li>Target ";
    message += m;
    message += " (A0 = pin ";
    message += mpuTargets[m].pinA0;
    message += ") ";
    if (mpuTargets[m].valid) {
      message += "valid\n<ul>\n";
      message += "<li>Current: &lt;";
      message += mpuTargets[m].x;
      message += ", ";
      message += mpuTargets[m].y;
      message += ", ";
      message += mpuTargets[m].z;
      message += "&gt;</li>\n<li>";
      message += mpuTargets[m].count;
      message += " activations</li>";
    } else {
      message += "INVALID\n<ul>\n";
    }
    String debug = mpuTargets[m].debug();
    int i0 = 0;
    while (i0 < debug.length()) {
      int i1 = debug.indexOf('\n', i0 + 1);
      if (i1 < 0) {
        break;
      }
      message += "<li>";
      message += debug.substring(i0, i1);
      message += "</li>\n";
      i0 = i1 + 1;
    }
    message += "</ul>";
    message += "</li>\n";
  }
  message += "</ul></p>\n";
  message += "</body></html>";
  server.send(200, "text/html", message);
  handledRequest();
}

void handleNotFound() {
  handlingRequest(false);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  handledRequest();
}
