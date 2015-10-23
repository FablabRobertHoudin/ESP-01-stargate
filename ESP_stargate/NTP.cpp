#include "NTP.h"
#include <ESP8266WiFi.h>

// https://www.arduino.cc/en/Tutorial/UdpNtpClient
// https://github.com/sandeepmistry/esp8266-Arduino/blob/master/esp8266com/esp8266/libraries/ESP8266WiFi/examples/NTPClient/NTPClient.ino#L37


class NTP Ntp;

  void NTP::sendNTPpacket(IPAddress& address)
  {
    udp.begin(localPort);
    Serial.println("sending NTP packet...");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    memcpy((char*)packetBuffer, "\xE3\x00\x06\xEC\x00\x00\x00\x00\x00\x00\x00\x00\x31\x4E\x31\x34", 16);

  
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
  }

  void NTP::checkPacket()
  {
    int cb = udp.parsePacket();
    //Serial.print("checkPacket: "); Serial.println(cb);
    if (cb) {
      parse();
      parsed = true;
    } // if cb
    
  }

  void NTP::parse ()
  {
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long t1 = millis() / 1000; // seconds, now

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    // now convert NTP time into everyday time:
    //Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    _epoch = secsSince1900 - seventyYears;
    // print Unix time:
    //Serial.println(epoch);

    _epoch -= t1; // offset ESP
    _epoch += 2 * 3600; // +2h dÃ©callage

    Serial.print("NTP answer: epoch=");
    Serial.println(_epoch);
  }

  void NTP::update()
  {
    unsigned long t = _epoch + millis() / 1000; // time
    _h = (t % 86400L) / 3600; // hour
    _m = (t % 3600) / 60; // min
    _s = (t % 60); // sec
    // BOF _s = (t*20 % 30); // sec
  }

