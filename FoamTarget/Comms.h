#ifndef Comms_h
#define Comms_h

bool gameMaster;

WiFiUDP Udp;
const unsigned int udpPort = 4210;
char incomingPacket[256];

const int MAC_ADDRESS_LENGTH = 6;

class FoamTargetClient {
  public:
    bool ok;
    bool previouslyRegistered;
    IPAddress ip;
    char mac[MAC_ADDRESS_LENGTH];

    bool matchesMAC(char* mac);
    void setMAC(char* otherMAC);
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
  
}

#endif
