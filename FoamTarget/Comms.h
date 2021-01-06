#ifndef Comms_h
#define Comms_h

bool gameMaster;

WiFiUDP udp;
char incomingPacket[256];

const unsigned int masterPort = 4210;

const unsigned int slavePort = 4211;
IPAddress masterIP;
uint8_t slaveID;

IPAddress broadcastIP(255, 255, 255, 255);
const int MAC_ADDRESS_LENGTH = 6;

const char MSG_SLAVE_ONLINE = 'S';
const char MSG_SLAVE_ACCEPTED = 'A';

class FoamTargetClient {
  public:
    bool ok;
    bool previouslyRegistered;
    IPAddress ip;
    char mac[MAC_ADDRESS_LENGTH];

    bool matchesMAC(char* mac);
    void setMAC(char* otherMAC);
    void copyMAC(char* destination); // TODO: remove
    String macString();
    void sendAccept(uint8_t id);
};

const int MAX_CLIENTS = 5;
FoamTargetClient clients[MAX_CLIENTS];

bool FoamTargetClient::matchesMAC(char* otherMAC) {
  for (uint8_t i = 0; i < MAC_ADDRESS_LENGTH; i++) {
    if (mac[i] != otherMAC[i]) {
      return false;
    }
  }
  return true;
}

void FoamTargetClient::setMAC(char* otherMAC) {
  for (uint8_t i = 0; i < MAC_ADDRESS_LENGTH; i++) {
    mac[i] = otherMAC[i];
  }
}

void FoamTargetClient::copyMAC(char* destination) {
  for (uint8_t i = 0; i < MAC_ADDRESS_LENGTH; i++) {
    destination[i] = mac[i];
  }
}

String FoamTargetClient::macString() {
  String response = "";
  for (uint8_t i = 0; i < MAC_ADDRESS_LENGTH; i++) {
    char h = (mac[i] >> 4) & 0x0F;
    char l = mac[i] & 0x0F;
    response += (char)(h < 10 ? '0' + h : '7' + h);
    response += (char)(l < 10 ? '0' + l : '7' + l);
    if (i < MAC_ADDRESS_LENGTH - 1) {
      response += ':';
    }
  }
  return response;
}

void FoamTargetClient::sendAccept(uint8_t id) {
  incomingPacket[0] = MSG_SLAVE_ACCEPTED;
  incomingPacket[1] = id;
  udp.beginPacket(clients[id].ip, slavePort);
  udp.write(incomingPacket, 2);
  udp.endPacket();
}

#endif
