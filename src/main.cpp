#include <Arduino.h>

#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"

#define BLED 4
#define LX A5
#define LY A4
#define RX A7
#define RY A6
#define LS 3
#define RS 2

//////Radio set-up//////////////////////////////////////////////////////
int SentMessage[6];

RF24 radio(10,9); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

const uint64_t pipe = 0xE7E7E6E6E6E6; // Needs to be the same for communicating between 2 NRF24L01
////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);

  pinMode( LS, INPUT_PULLUP);
  pinMode( RS, INPUT_PULLUP);
  pinMode(BLED, OUTPUT);

  digitalWrite(BLED, HIGH);

  radio.begin(); // Start the NRF24L01
  radio.openWritingPipe(pipe); // Get NRF24L01 ready to

}

void loop() {
  //Read joysticks
  int lx = analogRead(LX);
  int ly = analogRead(LY);
  int rx = analogRead(RX);
  int ry = analogRead(RY);
  //Read switches
  bool lS = digitalRead(LS);
  bool rS = digitalRead(RS);

  //remap joysticks from 0-1023 to -255 - 255
  int lX = map(lx, 1023, 0, -255, 255);
  int lY = map(ly, 0, 1023, -255, 255);
  int rX = map(rx, 0, 1023, -255, 255);
  int rY = map(ry, 1023, 0, -255, 255);

  //set message parts
  SentMessage[0] = lX;
  SentMessage[1] = lY;
  SentMessage[2] = rX;
  SentMessage[3] = rY;
  SentMessage[4] = lS;
  SentMessage[5] = rS;

  //send message
  radio.write(SentMessage, sizeof(SentMessage));

  //write what is being sent
  Serial.print("LX: ");
  Serial.print(lX);
  Serial.print("; LY: ");
  Serial.print(lY);
  Serial.print("; RX: ");
  Serial.print(rX);
  Serial.print("; RY: ");
  Serial.print(rY);
  Serial.print("; LS: ");
  Serial.print(lS);
  Serial.print("; RS: ");
  Serial.println(rS);
}
