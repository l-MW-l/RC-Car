#include <WiFiS3.h>
#include <Servo.h>
#include "webpage.h"

#define goPIN 3
#define reversePIN 2
#define servoPIN 7

char ssid[] = "Arduino_R4_Car";  // Name Ihres neuen Arduino-Hotspots
char pass[] = "12345678";       // Passwort für den Hotspot

WiFiServer server(80); //80 für http
Servo servo;

void setup() 
{
  
  Serial.begin(115200);

  pinMode(goPIN, OUTPUT);
  pinMode(reversePIN, OUTPUT);
  servo.attach(servoPIN);

  // Eigenes Netzwerk aufspannen
  Serial.println("Erstelle Access Point...");
  WiFi.beginAP(ssid, pass);

  server.begin();
  Serial.println("Access Point gestartet!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

}

void loop() 
{
  WiFiClient client = server.available();
  if (client) 
  {
    Serial.println("Neuer Client verbunden");
    String request = client.readStringUntil('\r');

    if (request.indexOf("GET / ") >= 0)
    {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println(HTML_CONTENT);
    }
    else if (request.indexOf("/go/on") >= 0 && digitalRead(reversePIN) == LOW)
    {
      digitalWrite(goPIN, HIGH);

      client.println("HTTP/1.1 204 No Content");
      client.println();
    }
    else if (request.indexOf("/go/off") >= 0 && digitalRead(reversePIN) == LOW)
    {
      digitalWrite(goPIN, LOW);

      client.println("HTTP/1.1 204 No Content");
      client.println();
    }   
    else if (request.indexOf("/back/on") >= 0 && digitalRead(goPIN) == LOW)
    {
      digitalWrite(goPIN, HIGH);
      digitalWrite(reversePIN, HIGH);

      client.println("HTTP/1.1 204 No Content");
      client.println();
    }
    else if (request.indexOf("/back/off") >= 0 && digitalRead(reversePIN) == HIGH)
    {
      digitalWrite(goPIN, LOW);
      digitalWrite(reversePIN, LOW);

      client.println("HTTP/1.1 204 No Content");
      client.println();
    }
    else if (request.indexOf("/left/on") >= 0)
    {
      servo.write(0);

      client.println("HTTP/1.1 204 No Content");
      client.println();
    }
    else if (request.indexOf("/left/off") >= 0)
    {
      servo.write(90);

      client.println("HTTP/1.1 204 No Content");
      client.println();
    }
    else if (request.indexOf("/right/on") >= 0)
    {
      servo.write(180);

      client.println("HTTP/1.1 204 No Content");
      client.println();
    }
    else if (request.indexOf("/right/off") >= 0)
    {
      servo.write(90);

      client.println("HTTP/1.1 204 No Content");
      client.println();
    }
    client.stop();
  }
}
