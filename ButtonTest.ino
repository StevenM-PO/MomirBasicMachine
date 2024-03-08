/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground through 220 ohm resistor
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/
// 'Orangutans', 420x580px



// constants won't change. They're used here to set pin numbers:
int totalIn = 0;
int totalOut = 0;
int rNumber = 0;

const uint16_t numFiles[] = {28, 930};
int creatureNumber;

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <SPI.h>
#include <SD.h>
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"

#define TX_PIN 18 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 17 // Arduino receive   GREEN WIRE   labeled TX on printer

Adafruit_7segment matrix = Adafruit_7segment();
Adafruit_7segment matrix2 = Adafruit_7segment();

SoftwareSerial pSerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&pSerial);


int pWidth = 384;
int pHeight = 535;
String fileNumber = "";



const int btnArray[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 22};

File root;
String fileString;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  for (uint16_t n = 0; n < 13; n++)
  {
    pinMode(btnArray[n], INPUT);
  }
  Serial.begin(9600);
  pSerial.begin(9600);
  printer.begin();
  Serial.println("Serial Begin");
  matrix.begin(0x70);
  matrix2.begin(0x71);
  matrix.println("1");
  matrix2.println("2");
  matrix.writeDisplay();
  matrix2.writeDisplay();
  

  Serial.println("matrix begin");
  Serial.println("SD Begin");
  if (!SD.begin(53))
  {
    Serial.println("SD Failed to initialize!");
    while(1);
  }
  randomSeed(analogRead(0));
}

void loop() {
  
  // read the state of the pushbutton value:
  for (uint16_t n = 0; n < 13; n++)
  {
    buttonState = digitalRead(btnArray[n]);
    if(buttonState == HIGH)
    {
      if (n == 10)
      {
        totalIn = 0;
        totalOut = 0;
      }
      else
        if (n == 11)
          for (int x = 0; x < 50; x++)
          {
            totalOut = random(totalIn + 1);
            matrix2.println(totalOut);
            matrix2.writeDisplay();
            delay(10);
          }
      else
        if (n == 12)
        {
          for (int x = 0; x < 50; x++)
          {
            creatureNumber = random(numFiles[totalIn]);
            matrix2.println(creatureNumber);
            matrix2.writeDisplay();
            delay(10);
          }
          Serial.println(creatureNumber);
          Serial.println(totalIn);
          fileNumber = ("/" + String(totalIn) );
          fileNumber = (fileNumber + "/");
          fileNumber = (fileNumber + creatureNumber);
          fileNumber += ".bin";
          Serial.println(fileNumber);
          root = SD.open(fileNumber, FILE_READ);
          //int counter = 0;
          //Serial.println("PRINT BUTTON!!!");
         // Serial.println("Before fuke read");
          //Serial.println("After file read, before print commadn");
          printer.printBitmap(pWidth, pHeight, dynamic_cast<Stream*>(&root));
          printer.println(creatureNumber);
          //printer.printBitmap(pWidth, pHeight, control, true);
          //Serial.println("After print command");
          printer.feedRows(80);
          root.close();


        }
      else
        if (totalIn < 1000 & totalIn > -1) 
          totalIn = (totalIn * 10) + n;
      //matrix.println(totalIn);
      //matrix2.println(totalOut);
      //matrix.writeDisplay();
      //matrix2.writeDisplay();
      //Serial.println(totalIn);
     // Serial.println(totalOut);
      while(buttonState == HIGH)
      {
      buttonState = digitalRead(btnArray[n]);
      }
    delay(100);
    break;
    }


  }
}
