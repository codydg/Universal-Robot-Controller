#include <Arduino.h>

#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"

#define PRINT_ORIGINAL_VALUES

#define BLED 4
#define LX A5
#define LY A4
#define RX A7
#define RY A6
#define LS 3
#define RS 2

//////Radio set-up//////////////////////////////////////////////////////
int message[6];

RF24 radio(10,9); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

const uint64_t pipe = 0xE7E7E6E6E6E6; // Needs to be the same for communicating between 2 NRF24L01

//////Controller functions//////////////////////////////////////////////
enum Control {
  LEFT_X = 0,
  LEFT_Y = 1,
  RIGHT_X = 2,
  RIGHT_Y = 3,
  LEFT_SWITCH = 4,
  RIGHT_SWITCH = 5
};
void updateController();
void mapController();

int left_center_x;
int left_center_y;
int right_center_x;
int right_center_y;

//////Misc functions////////////////////////////////////////////////////
void printValues(char* label, bool endLine = false);

void setup() {
  Serial.begin(115200);

  pinMode( LS, INPUT_PULLUP);
  pinMode( RS, INPUT_PULLUP);
  pinMode(BLED, OUTPUT);

  digitalWrite(BLED, HIGH);

  radio.begin(); // Start the NRF24L01
  radio.openWritingPipe(pipe); // Get NRF24L01 ready to

  // Center joysticks
  updateController();
  left_center_x = message[LEFT_X];
  left_center_y = message[LEFT_Y];
  right_center_x = message[RIGHT_X];
  right_center_y = message[RIGHT_Y];
}

void loop() {
  //Read joysticks
  updateController();

#ifdef PRINT_ORIGINAL_VALUES
  printValues("Original Values");
#endif

  mapController();

  //send message
  radio.write(message, sizeof(message));

  //write what is being sent
  printValues("Sent Values", true);
}

void updateController()
{
  message[LEFT_X] = analogRead(LX);
  message[LEFT_Y] = analogRead(LY);
  message[RIGHT_X] = analogRead(RX);
  message[RIGHT_Y] = analogRead(RY);
  message[LEFT_SWITCH] = digitalRead(LS);
  message[RIGHT_SWITCH] = digitalRead(RS);
}

void mapController()
{
  if (message[LEFT_X] > left_center_x)
    message[LEFT_X] = map(message[LEFT_X], 1023, left_center_x, -255, 0);
  else
    message[LEFT_X] = map(message[LEFT_X], left_center_x, 0, 0, 255);

  if (message[LEFT_Y] > left_center_y)
    message[LEFT_Y] = map(message[LEFT_Y], left_center_y, 1023, 0, 255);
  else
    message[LEFT_Y] = map(message[LEFT_Y], 0, left_center_y, -255, 0);

  if (message[RIGHT_X] > right_center_x)
    message[RIGHT_X] = map(message[RIGHT_X], right_center_x, 1023, 0, 255);
  else
    message[RIGHT_X] = map(message[RIGHT_X], 0, right_center_x, -255, 0);

  if (message[RIGHT_Y] > right_center_y)
    message[RIGHT_Y] = map(message[RIGHT_Y], 1023, right_center_y, -255, 0);
  else
    message[RIGHT_Y] = map(message[RIGHT_Y], right_center_y, 0, 0, 255);
}

void printValues(char* label, bool endLine)
{
  Serial.print(label);
  Serial.print(": LX ");
  Serial.print(message[LEFT_X]);
  Serial.print("; LY ");
  Serial.print(message[LEFT_Y]);
  Serial.print("; RX ");
  Serial.print(message[RIGHT_X]);
  Serial.print("; RY ");
  Serial.print(message[RIGHT_Y]);
  Serial.print("; LS ");
  Serial.print(message[LEFT_SWITCH]);
  Serial.print("; RS ");
  Serial.print(message[RIGHT_SWITCH]);
  if (endLine)
    Serial.println();
  else
    Serial.print(" | ");
}
