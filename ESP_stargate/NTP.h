#ifndef NTP_h
#define NTP_h

#include <Arduino.h>
#include <WiFiUdp.h>

// NTP time stamp is in the first 48 bytes of the message
#define NTP_PACKET_SIZE 48

// Time: UTC + 2(ete) / 1(hivers)
#define FUSEAU 1

class NTP
{
  private:
    unsigned long _epoch;
    WiFiUDP udp;
    unsigned int localPort = 2390; // local port to listen for UDP packets
    //const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
    byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

  
  public:
    bool parsed = false;
    byte _h, _m, _s;

    static IPAddress timeServer;

  void sendNTPpacket(IPAddress& address);
  void parse ();
  void update();
  void checkPacket();

};

extern NTP Ntp;

#endif

