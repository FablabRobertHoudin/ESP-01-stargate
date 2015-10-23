#ifndef PubNub_h
#define PubNub_h

#include <Arduino.h>
#include <WiFiClient.h>
#include "ESP8266WiFi.h"

class PubNub
{
  private:
    const char* _server = "pubsub.pubnub.com";
    String _publish, _subscribe;
    char timetoken[22];
    WiFiClient client;

  public:
    PubNub(String publish, String subscribe) {
      _publish = publish;
      _subscribe = subscribe;
      memcpy(timetoken, "0\0", 2);
      };

    //wget 'http://pubsub.pubnub.com/publish/pub-XXXXXXXXXXXXXXX/sub-XXXXXXXXXXXXXXXXXXXXXXXXX/0/ESP/0/"OK"' -O pubsub.txt
    bool publish(String _topic, String _json) {
      if (client.connect(_server, 80)) {
        client.print("GET /publish/");
        client.print(_publish);
        client.print("/");
        client.print(_subscribe);
        client.print("/0/");
        client.print(_topic);
        client.print("/0/");
        client.print(_json);
        client.print(" HTTP/1.0\r\n\r\n");
        while (client.available() <= 1) {};
        String line = client.readString();
Serial.print("PUBLISH ");
Serial.println(line);
        return true;
      }
      return false;
    };
    
    //wget 'http://pubsub.pubnub.com/subscribe/sub-XXXXXXXXXXXXXXXXXXXXXXXXX/ESP/0/14452477034226613' -O pubsub.txt
    String subscribe(String _topic) {
      int loop = 1000;
      if (client.connect(_server, 80)) {
        client.print("GET /subscribe/");
        client.print(_subscribe);
        client.print("/");
        client.print(_topic);
        client.print("/0/");
        client.print(timetoken);
        client.print(" HTTP/1.0\r\n\r\n");
Serial.print("SUBSCRIBE ");
        while (client.available() <= 1 && loop--) {};
        String line = client.readString();
Serial.print(loop);
Serial.println(line);
        if (loop>0) {
          //M.display(1, 0b00000100, "read:"+line.length());
          int l = line.length();
          int i = line.indexOf("\r\n\r\n");
          line = line.substring(i+4);
          line.substring(line.length()-19).toCharArray(timetoken, 18);
          timetoken[18] = '\0';
          return line;
        }
      }
      return "";
    };

};

#endif

