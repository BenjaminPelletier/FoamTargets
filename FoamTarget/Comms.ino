#include "Comms.h"

void setupComms() {
  if (gameMaster) {
    setupGameMaster();
  } else {
    setupGameSlave();
  }
}

void setupGameMaster() {
  udp.begin(masterPort);
}

void setupGameSlave() {
  udp.begin(slavePort);

  incomingPacket[0] = MSG_SLAVE_ONLINE;
  WiFi.macAddress((uint8_t*)incomingPacket + 1);
  udp.beginPacket(broadcastIP, masterPort);
  udpWrite(incomingPacket, 1 + MAC_ADDRESS_LENGTH);
  udp.endPacket();
  Serial.println("Announced presence; awaiting acknowledgement from master...");

  while (true) {
    int packetSize = udp.parsePacket();
    if (packetSize > 0)
    {
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
      int len = udp.read(incomingPacket, 255);
      incomingPacket[len] = '\0';
      Serial.printf("UDP packet contents: %s\n", incomingPacket);
  
      if (incomingPacket[0] == MSG_SLAVE_ACCEPTED) {
        masterIP = udp.remoteIP();
        slaveID = incomingPacket[1];
        break;
      } else {
        Serial.println("Packet was not a response from the game master");
      }
    }
  }
}

void pollUDP() {
  int packetSize = udp.parsePacket();
  if (packetSize > 0)
  {
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    int len = udp.read(incomingPacket, 255);
    incomingPacket[len] = '\0';
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    if (gameMaster) {
      interpretPacketAsGameMaster(incomingPacket, len);
    } else {
      interpretPacketAsGameSlave(incomingPacket, len);
    }
  }
}

void interpretPacketAsGameMaster(char* packet, int len) {
  if (len == 0) {
    return;
  }
  
  char packetType = incomingPacket[0];
  if (packetType == MSG_SLAVE_ONLINE) {
    acceptOnlinePacket(udp.remoteIP(), incomingPacket + 1, len - 1);
  } else {
    Serial.println("Unrecognized packet type\n");
  }
}

void interpretPacketAsGameSlave(char* packet, int len) {
  Serial.println("Game slave is not yet implemented");
}

void acceptOnlinePacket(IPAddress ip, char* packet, int len) {
  if (len < 6) {
    Serial.println("Error in Online packet: packet was not long enough");
    return;
  }

  int clientID = -1;
  for (uint8_t c = 0; c < MAX_CLIENTS; c++) {
    if (clients[c].matchesMAC(packet)) {
      // Client is reconnecting (MAC already known)
      clientID = c;
      break;
    }
  }

  if (clientID < 0) {
    for (uint8_t c = 0; c < MAX_CLIENTS; c++) {
      if (!clients[c].previouslyRegistered) {
        // New client is using previously-unused client slot
        clientID = c;
        break;
      }
    }
  }

  if (clientID < 0) {
    for (uint8_t c = 0; c < MAX_CLIENTS; c++) {
      if (!clients[c].ok) {
        // New client is evicting down client
        clientID = c;
        break;
      }
    }
  }

  if (clientID < 0) {
    Serial.println("Could not onboard client; no available slots");
  }

  clients[clientID].setMAC(packet);
  clients[clientID].ip = ip;
  clients[clientID].ok = true;
  clients[clientID].previouslyRegistered = true;
  clients[clientID].sendAccept((uint8_t)clientID);
  Serial.print("Accepted new client ");
  Serial.print(clientID);
  Serial.print(" at ");
  Serial.println(ip.toString());
}
