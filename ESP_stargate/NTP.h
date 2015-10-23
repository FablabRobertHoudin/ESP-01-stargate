#ifndef NTP_h
#define NTP_h

#include <Arduino.h>
#include <WiFiUdp.h>

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

class NTP
{
  private:
    unsigned long _epoch;
    byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
    WiFiUDP udp;
    unsigned int localPort = 2390; // local port to listen for UDP packets

  
  public:
    bool parsed = false;
    byte _h, _m, _s;

  void sendNTPpacket(IPAddress& address);
  void parse ();
  void update();
  void checkPacket();

};

extern NTP Ntp;

#endif

