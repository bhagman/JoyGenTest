/*
||
|| @author         Brett Hagman <bhagman@roguerobotics.com>
|| @url            http://roguerobotics.com/
|| @url            http://wiring.org.co/
||
|| @description
|| |
|| | Joy Gen Test - Sender
|| |
|| #
||
|| @license
|| |
|| | Copyright (c) 2016 - Brett Hagman (http://roguerobotics.com/)
|| |
|| | Permission is hereby granted, free of charge, to any person obtaining a copy of
|| | this software and associated documentation files (the "Software"), to deal in
|| | the Software without restriction, including without limitation the rights to
|| | use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
|| | the Software, and to permit persons to whom the Software is furnished to do so,
|| | subject to the following conditions:
|| |
|| | The above copyright notice and this permission notice shall be included in all
|| | copies or substantial portions of the Software.
|| |
|| | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
|| | IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
|| | FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
|| | COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
|| | IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
|| | CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
|| #
||
|| @notes
|| |
|| |
|| #
||
*/


#include <SPI.h>
#include "RF24.h"

#define VERSIONSTR "1.0"

byte ctrlAddr[][6] = { "1Ctrl", "2Ctrl" };
byte treeAddr[][6] = { "1Tree", "2Tree", "3Tree" };

bool radioNumber = 0;

RF24 radio(9, 10);

/**
* Create a data structure for transmitting and receiving data
* This allows many variables to be easily sent and received in a single transmission
* See http://www.cplusplus.com/doc/tutorial/structures/
*/
struct dataStruct
{
  char cmd;
  uint16_t value;
} myData;

uint16_t lastReading = 0;


void setup()
{
  Serial.begin(115200);
  Serial.println(F("JoyGen -- Sender Test1 V" VERSIONSTR));

  radio.begin();

  radio.setPALevel(RF24_PA_MIN);

  radio.openWritingPipe(treeAddr[0]);
  radio.openReadingPipe(1, ctrlAddr[radioNumber]);

  radio.startListening();

  lastReading = analogRead(0);

}


void loop()
{
  uint16_t reading;
  static uint32_t lastTime = 0;

  while (!Serial.available())
  {
    if ((millis() - lastTime) > 10)  // read only every x ms
    {
      lastTime = millis();
      reading = analogRead(0);
      if (reading != lastReading)
        break; // exit the while loop;
    }
  }

  while (Serial.available())
  {
    Serial.read(); // clear the buffer if we got serial data
  }

  radio.stopListening();                                    // First, stop listening so we can talk.

  lastReading = reading;

  Serial.print(F("Changed to "));
  Serial.print(reading);

  myData.cmd = 'X';
  myData.value = map(reading, 0, 1023, 0, 99);

  if (!radio.write(&myData, sizeof(myData)))
  {
    Serial.println(F("failed"));
  }
  else
  {
    Serial.println(F("Sent"));
  }

  radio.startListening();                                    // Now, continue listening
}

