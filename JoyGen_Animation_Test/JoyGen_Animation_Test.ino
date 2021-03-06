/*
||
|| @author         Brett Hagman <bhagman@roguerobotics.com>
|| @url            http://roguerobotics.com/
|| @url            http://wiring.org.co/
||
|| @description
|| |
|| | Joy Gen Test - Tree Animation
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

#define IDLETIMEOUT 30000L

#define IDLECOLOUR 0x505050
#define IDLESPARKLECOLOUR 0xffffff
#define IDLENEXTSPARKLETIME 50



byte ctrlAddr[][6] = { "1Ctrl", "2Ctrl" };
byte treeAddr[][6] = { "1Tree", "2Tree", "3Tree" };

bool radioNumber = 0;

RF24 radio(9, 10);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(100, PIN, NEO_RGB + NEO_KHZ800);

uint8_t animationNumber = 0;
uint32_t param1 = 0;
uint32_t param2 = 0;
uint32_t param3 = 0;

bool prepared = false;
uint32_t lastInteractionTime = 0;
bool idle = false;



struct dataStruct
{
  char cmd;
  uint16_t value;
} myData;


uint8_t red(uint32_t c)
{
  return (c >> 16);
}


uint8_t green(uint32_t c)
{
  return (c >> 8);
}


uint8_t blue(uint32_t c)
{
  return (c);
}


uint32_t fadeOne(uint32_t c, float rate)
{
  uint16_t r, g, b;

  r = constrain(red(c) * rate, 0, 255);
  g = constrain(green(c) * rate, 0, 255);
  b = constrain(blue(c) * rate, 0, 255);

  return strip.Color(r, g, b);
}


void fadeAll(float rate)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, fadeOne(strip.getPixelColor(i), rate));
  }
}


void prepareIdleAnimation()
{
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, IDLECOLOUR);
  }

  strip.show();
}


void idleAnimation()
{
  static uint32_t lastTime = 0;
  static uint8_t lastSparklePixel = 0;

  if (!prepared)
  {
    prepareIdleAnimation();
    prepared = true;
    lastTime = millis();
  }

  if ((millis() - lastTime) > IDLENEXTSPARKLETIME)
  {
    strip.setPixelColor(lastSparklePixel, IDLECOLOUR);
    lastSparklePixel = random(strip.numPixels());
    strip.setPixelColor(lastSparklePixel, IDLESPARKLECOLOUR);
    strip.show();
    lastTime = millis();
  }

}


// Animation 0: light chase
// Param A: colour
// Param B: speed

void prepareAnimation0()
{
  strip.clear();

  strip.show();
}


void animation0()
{
  static uint32_t lastTime = 0;
  static uint8_t lastPixel = 0;

  if (!prepared)
  {
    prepareAnimation0();
    prepared = true;
    lastTime = millis();
    lastPixel = 0;
  }

  if ((millis() - lastTime) > param2)
  {
    lastPixel = lastPixel + 1;
    if (lastPixel == strip.numPixels())
      lastPixel = 0;

    fadeAll(0.85);
    strip.setPixelColor(lastPixel, param1);
    strip.show();
    lastTime = millis();
  }
}


// Animation 1: dual light chase
// Param A: colour
// Param B: speed

void prepareAnimation1()
{
  strip.clear();

  strip.show();
}


void animation1()
{
  static uint32_t lastTime = 0;
  static uint8_t lastPixel = 0;

  if (!prepared)
  {
    prepareAnimation1();
    prepared = true;
    lastTime = millis();
    lastPixel = 0;
  }

  if ((millis() - lastTime) > param2)
  {
    lastPixel = lastPixel + 1;
    if (lastPixel == strip.numPixels())
      lastPixel = 0;

    fadeAll(0.85);
    strip.setPixelColor(lastPixel, param1);
    strip.setPixelColor(strip.numPixels() - lastPixel - 1, 0x0000ff);
    strip.show();
    lastTime = millis();
  }
}


// Animation 2: random pixels with fade
// Param A: colour
// Param B: speed

void prepareAnimation2()
{
  strip.clear();

  strip.show();
}


void animation2()
{
  static uint32_t lastTime = 0;
//  static uint8_t lastPixel = 0;

  if (!prepared)
  {
    prepareAnimation2();
    prepared = true;
    lastTime = millis();
//    lastPixel = 0;
  }

  if ((millis() - lastTime) > param2)
  {
    fadeAll(0.85);
    strip.setPixelColor(random(strip.numPixels()), param1);
    strip.show();
    lastTime = millis();
  }
}


void setIdleMode()
{
  idle = true;
  prepared = false;
}


void setActiveMode()
{
  idle = false;
  prepared = false;
  lastInteractionTime = millis();
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void setup()
{
  Serial.begin(115200);
  Serial.println(F("JoyGen -- Animation Test V" VERSIONSTR));

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  radio.begin();

  radio.setPALevel(RF24_PA_MIN);

  radio.openWritingPipe(ctrlAddr[0]);
  radio.openReadingPipe(1, treeAddr[radioNumber]);

  radio.startListening();

  setIdleMode();
}


void loop()
{
  if (!idle && (millis() - lastInteractionTime) > IDLETIMEOUT)
  {
    setIdleMode();
  }

  if (idle)
  {
    idleAnimation();
  }
  else
  {
    switch (animationNumber)
    {
      case 0:
        animation0();
        break;
      case 1:
        animation1();
        break;
      case 2:
        animation2();
        break;
      default:
        break;
    }
  }


  if (Serial.available())
  {
    char c = Serial.read();

    switch (c)
    {
      case 'a':
        animationNumber = 0;
        param1 = Wheel(random(256));
        param2 = 20;
        setActiveMode();
        break;
      case 'b':
        animationNumber = 1;
        param1 = Wheel(random(256));
        param2 = 20;
        setActiveMode();
        break;
      case 'c':
        animationNumber = 2;
        param1 = Wheel(random(256));
        param2 = 20;
        setActiveMode();
        break;
    }
  }
/*
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
*/
}

