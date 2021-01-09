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

  packetBuffer[0] = MSG_SLAVE_ONLINE;
  WiFi.macAddress((uint8_t*)packetBuffer + 1);
  udp.beginPacket(broadcastIP, masterPort);
  udpWrite(packetBuffer, 1 + MAC_ADDRESS_LENGTH);
  udp.endPacket();
  Serial.println("Announced presence; awaiting acknowledgement from master...");

  while (true) {
    int packetSize = udp.parsePacket();
    if (packetSize > 0)
    {
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
      int len = udp.read(packetBuffer, 255);
      packetBuffer[len] = '\0';
      Serial.printf("UDP packet contents: %s\n", packetBuffer);
  
      if (packetBuffer[0] == MSG_SLAVE_ACCEPTED) {
        masterIP = udp.remoteIP();
        slaveID = packetBuffer[1];
        break;
      } else {
        Serial.println("Packet was not a response from the game master");
      }
    }
  }
}

void pollUDP(SlaveHitHandler onHit) {
  int packetSize = udp.parsePacket();
  if (packetSize > 0)
  {
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    int len = udp.read(packetBuffer, 255);
    packetBuffer[len] = '\0';
    //Serial.printf("UDP packet contents: %s\n", packetBuffer);

    if (gameMaster) {
      interpretPacketAsGameMaster(packetBuffer, len, onHit);
    } else {
      interpretPacketAsGameSlave(packetBuffer, len);
    }
  }
}

void interpretPacketAsGameMaster(char* packet, int len, SlaveHitHandler onHit) {
  if (len == 0) {
    return;
  }
  
  char packetType = packetBuffer[0];
  if (packetType == MSG_SLAVE_ONLINE) {
    acceptOnlinePacket(udp.remoteIP(), packetBuffer + 1, len - 1);
  } else if (packetType == MSG_SLAVE_TARGET_HIT) {
    onHit(GameTargetID(packetBuffer[1]));
  } else {
    Serial.println("Unrecognized packet type\n");
  }
}

void interpretPacketAsGameSlave(char* packet, int len) {
  if (len == 0) {
    return;
  }

  char packetType = packetBuffer[0];
  if (packetType == MSG_CHANGE_STYLE) {
    uint8_t t = (uint8_t)packetBuffer[1];
    TargetStyles::Style idleStyle = (TargetStyles::Style)packetBuffer[2];
    TargetStyles::Style hitStyle = (TargetStyles::Style)packetBuffer[3];
    targetDisplays[t].styleIdle = idleStyle;
    targetDisplays[t].styleHit = hitStyle;
    targetDisplays[t].resetAnimation();
  }
}

void acceptOnlinePacket(IPAddress ip, char* packet, int len) {
  if (len < 6) {
    Serial.println("Error in Online packet: packet was not long enough");
    return;
  }

  int clientID = 0;
  for (uint8_t c = 1; c < MAX_CLIENTS; c++) {
    if (clients[c].matchesMAC(packet)) {
      // Client is reconnecting (MAC already known)
      clientID = c;
      break;
    }
  }

  if (clientID < 1) {
    for (uint8_t c = 1; c < MAX_CLIENTS; c++) {
      if (!clients[c].previouslyRegistered) {
        // New client is using previously-unused client slot
        clientID = c;
        break;
      }
    }
  }

  if (clientID < 1) {
    for (uint8_t c = 1; c < MAX_CLIENTS; c++) {
      if (!clients[c].ok) {
        // New client is evicting down client
        clientID = c;
        break;
      }
    }
  }

  if (clientID < 1) {
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

void notifyGameMasterOfHit(uint8_t target) {
  GameTargetID id = GameTargetID(slaveID, target);

  packetBuffer[0] = MSG_SLAVE_TARGET_HIT;
  packetBuffer[1] = (char)id.data;
  udp.beginPacket(masterIP, masterPort);
  udpWrite(packetBuffer, 2);
  udp.endPacket();

  /*
  Serial.print("Informed master ");
  Serial.print(target);
  Serial.println("hit");
  */
}

void changeSlaveTargetStyle(GameTargetID id, TargetStyles::Style idle, TargetStyles::Style hit) {
  uint8_t client = id.client();

  packetBuffer[0] = MSG_CHANGE_STYLE;
  packetBuffer[1] = (char)id.target();
  packetBuffer[2] = (char)idle;
  packetBuffer[3] = (char)hit;
  
  udp.beginPacket(clients[client].ip, slavePort);
  udpWrite(packetBuffer, 4);
  udp.endPacket();

  /*
  Serial.print("Instructd slave ");
  Serial.print(client);
  Serial.print(" at ");
  Serial.print(clients[client].ip.toString());
  Serial.print(':');
  Serial.print(slavePort);
  Serial.print(" to change target ");
  Serial.print(id.target());
  Serial.println(" styles");
  */
}
