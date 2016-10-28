/*
||
|| @author         Brett Hagman <bhagman@roguerobotics.com>
|| @url            http://roguerobotics.com/
|| @url            http://wiring.org.co/
||
|| @description
|| |
|| | Joy Gen Test - Receiver/Tree
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
#include <Adafruit_NeoPixel.h>

#define VERSIONSTR "1.0"

#define PIN 6

byte ctrlAddr[][6] = { "1Ctrl", "2Ctrl" };
byte treeAddr[][6] = { "1Tree", "2Tree", "3Tree" };

bool radioNumber = 0;

RF24 radio(9, 10);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(100, PIN, NEO_GRB + NEO_KHZ800);

struct dataStruct
{
  char cmd;
  uint16_t value;
} myData;


void showPixel(int p)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, 0);
  }

  strip.setPixelColor(p, 0xffffff);
  strip.show();
}


void setup()
{
  Serial.begin(115200);
  Serial.println(F("JoyGen -- Receiver Test1 V" VERSIONSTR));

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  radio.begin();

  radio.setPALevel(RF24_PA_MIN);

  radio.openWritingPipe(ctrlAddr[0]);
  radio.openReadingPipe(1, treeAddr[radioNumber]);

  radio.startListening();
}


void loop()
{
  if (radio.available())
  {
    int n = radio.available();

    while (radio.available())
    {
      radio.read(&myData, sizeof(myData));             // Get the payload
    }

    Serial.print(F("Got: "));
    Serial.print(myData.cmd);
    Serial.print(' ');
    Serial.println(myData.value);
    showPixel(myData.value);
  }
}

