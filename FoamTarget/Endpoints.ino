#include "Endpoints.h"

void setupEndpoints() {
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);
  server.begin();
}

void handleRoot() {
  showHandlingRequest(true);
  server.send(200, "text/html", "<html><title>FoamTarget</title><body><h1>FoamTarget</h1>\n<p><a href=\"status\">Status</a></p></body></html>");
  showHandledRequest();
}

void handleStatus() {
  showHandlingRequest(true);
  String message = "<html><body>\n";
  
  message += "<p>\n<h2>";
  message += gameMaster ? "Game master" : "Game slave";
  message += "</h2></p>\n";

  if (gameMaster) {
    message += "<p>\n<h2>Clients</h2>\n<ul>\n";
    for (uint8_t c = 0; c < MAX_CLIENTS; c++) {
      message += "<li>Client ";
      message += c;
      if (!clients[c].ok && !clients[c].previouslyRegistered) {
        message += " (slot available)</li>\n";
      } else {
        message += clients[c].ok ? " (active)" : " (disconnected)";
        message += "\n<ul><li>IP: ";
        message += clients[c].ip.toString();
        message += "</li>\n<li>MAC: ";
        message += clients[c].macString();
        message += "</li>\n</ul>\n";
      }
    }
    message += "</p>\n";
  }
  
  message += "<p>\n<h2>Accelerometer targets</h2>\n";
  message += "Last check period: ";
  message += (tMinus1 - tMinus2);
  message += " ms<br>\n";
  message += "Longest check period: ";
  message += tMaxDelta;
  message += " ms<br>\n<ul>\n";
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
  showHandledRequest();
}

void handleNotFound() {
  showHandlingRequest(false);
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
  showHandledRequest();
}
