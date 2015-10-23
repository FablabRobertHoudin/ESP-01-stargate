#include "ESP8266WiFi.h"
#include <Ticker.h>
#include <WiFiUdp.h>

#include "NTP.h"
#include "Strip.h"
#include "PubNub.h"

const char* ssid1 = "freebox";
const char* password1 = "XXXXXXXXXXXX";
const char* ssid2 = "Livebox";
const char* password2 = "XXXXXXXXXXXXXXXXXXXX";

//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov NTP server
//IPAddress timeServer(192, 168, 0, 254); // mafreebox NTP server
//unsigned long epoch = 60L;

WiFiServer server(80);

Ticker tick;

PubNub pubnub("pub-XXXXXXXXXXXXXXXXXXXXXXXX", "sub-XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

#define GRBpin 2

byte GRB[3 * LEN];
Strip *sStrip, *mStrip, *hStrip, *waitStrip;

// ====================================================================================================

void setup() {
  int loop = 0;

  Serial.begin(115200);
  Serial.flush();

  pinMode(GRBpin, OUTPUT); digitalWrite(GRBpin, LOW);

  WiFi.begin(ssid1, password1);

  while ((WiFi.status() != WL_CONNECTED) && (loop++ < 20)) {
    delay(500);
    Serial.print(".");
  }

//#ifdef(ssid2)
if (WiFi.status() != WL_CONNECTED) {
  Serial.print("2");
  WiFi.begin(ssid2, password2);

  loop=0;
  while ((WiFi.status() != WL_CONNECTED) && (loop++ < 20)) {
    delay(500);
    Serial.print(".");
  }
}
//#endif

  /*
    // Start the server
    server.begin();
    Serial.println("Server started");
  */
  // Print the IP address
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  //Serial.end();

  Ntp.sendNTPpacket(timeServer); // send an NTP packet to a time server

  sStrip = new Strip(5, "SS"); sStrip->fill( 16,   0,  16); // sec
  mStrip = new Strip(1, "MM"); mStrip->fill(255,   0,   0); // min RED
  hStrip = new Strip(1, "HH"); hStrip->fill(255, 255, 255); // hour WHITE

  waitStrip = new Strip(20, "wait");

  server.begin();
  Serial.println("Server started");

  pubnub.publish("ESP", "\"OK\"");

  tick.attach(0.5, updateTime);
  //tick.attach(0.1, updateTime);
}

// ----------------------------------------------------------------------------------------------------

// GRB: 0 en bas avec 60, tourne dans le sens trigo...
void display() {
  byte *ptr = GRB, b;
  int i, j;
  for (i = 0; i < (LEN * 3); i++) {
    b = *ptr++;
    for (j = 0; j < 8; j++) {
      if (b & 0x80) { // bit 1
        digitalWrite(GRBpin, HIGH); // 4 HIGH 1 LOW ?
        digitalWrite(GRBpin, HIGH);
        digitalWrite(GRBpin, HIGH);
        digitalWrite(GRBpin, LOW);
      } else { // bit 0
        digitalWrite(GRBpin, HIGH); // 1 HIGH 4 LOW ?
        digitalWrite(GRBpin, LOW);
        digitalWrite(GRBpin, LOW);
        digitalWrite(GRBpin, LOW);
      }
      b *= 2;
    }
  }

}

// ----------------------------------------------------------------------------------------------------

int wPos; // Position attente
int wLoop; // nombre de fois qu'on itère

void updateWait() {
  waitStrip->writeGRB(wPos++);

  display();

  if (--wLoop == 0) { // finished
    tick.attach(0.5, updateTime);
  }
}
// ----------------------------------------------------------------------------------------------------

void updateTime() {
  //byte *ptr, b, v;
  int i;

  Ntp.update();
/*
  Serial.print("Time: ");
  Serial.print(Ntp._h);
  Serial.print(":");
  Serial.print(Ntp._m);
  Serial.print(":");
  Serial.print(Ntp._s);
  Serial.println();
*/
  // clear GRB
  memset(GRB, 0, LEN * 3);

  sStrip->writeGRB(Ntp._s); // +LEN - 5
  mStrip->writeGRB(Ntp._m);
  hStrip->writeGRB((Ntp._h % 12) * 60 / 12);
  //memcpy(GRB + 59 * 3, "\x00\x00\xFF", 3); // last: BLUE

  display();
}

// ----------------------------------------------------------------------------------------------------

unsigned long 
tx = 0;

void loop() {

  if (!Ntp.parsed) {
    Ntp.checkPacket();
    // Start the server
  }

  if (millis() - tx > 20000) {
    tx = millis();
    String line = pubnub.subscribe("ESP");
    Serial.println("ESP subscribe: " + line);
    
  }


  WiFiClient client = server.available();
  if (client) {
    // Wait until the client sends some data
    //Serial.println("new client");
    while (!client.available()) {
      delay(1);
    }

    // Read the first line of the request
    String req = client.readStringUntil('\r');
    Serial.println("REQ: " + req);
    client.flush();

    // GET /?color=%23ff0000&speed=5
    int idx = req.indexOf("color=");
    int idxc = req.indexOf("speed=");

    // Send the response to the client
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>");
    client.print("<head><title>Stargate</title></head>");
    client.print("<h3>Stargate</h3>my time: ");
    Ntp.update();
    client.print(Ntp._h);
    client.print(":");
    client.print(Ntp._m);
    client.print(":");
    client.print(Ntp._s);
    client.print("<br/><form method='GET'>");
    client.print("Couleur: <input type='color' name='color' value='#" + req.substring(idx + 9, idx + 15) + "' onchange='this.form.submit()'><br/>");
    //client.print("Vitesse: <input type='range' name='speed' min='0' max='9' value='"+req.substring(idxc+6, idxc+7)+"'><br/>");
    //client.print("<input type='submit'></form>");
    client.print("</html>");
    //client.close();

    if (idx > 0) {

      wPos = 0; // départ
      wLoop = 5 * 60; // X tours
      byte a, b;
      a = req.charAt(idx + 9);
      a = (a > '9') ? a - 'a' + 10 : a - '0';
      b = req.charAt(idx + 10);
      b = (b > '9') ? b - 'a' + 10 : b - '0';
      byte wR = a * 16 + b;
      a = req.charAt(idx + 11);
      a = (a > '9') ? a - 'a' + 10 : a - '0';
      b = req.charAt(idx + 12);
      b = (b > '9') ? b - 'a' + 10 : b - '0';
      byte wG = a * 16 + b;
      a = req.charAt(idx + 13);
      a = (a > '9') ? a - 'a' + 10 : a - '0';
      b = req.charAt(idx + 14);
      b = (b > '9') ? b - 'a' + 10 : b - '0';
      byte wB = a * 16 + b;
      //wG = atoi("0x"+req.substring(idx+9, idx+11));
      //wB = atoi("0x"+req.substring(idx+11, idx+13));
      Serial.print("wR=");
      Serial.print(wR, HEX);
      Serial.print(", wG=");
      Serial.print(wG, HEX);
      Serial.print(", wB=");
      Serial.print(wB, HEX);
      Serial.println();

      wPos = 0; // départ
      wLoop = 10 * 60; // X tours
      waitStrip->fill(wR, wG, wB, 0);
      tick.attach(0.01, updateWait);
    }

  } // if client


  if (Serial.available()) {
    while (Serial.available()) {
      Serial.read();
    }
    wPos = 0; // départ
    wLoop = 10 * 60; // X tours
    waitStrip->fill(0, 0, 255, 0);
    tick.attach(0.001, updateWait);
  }
  /*
    if (millis() - tx > 20000) {
      tx = millis();
      wPos = 0; // départ
      wLoop = 10 * 60; // X tours
      waitStrip->fill(0, 100, 255, 0);
      tick.attach(0.001, updateWait);
    }
  */

}

