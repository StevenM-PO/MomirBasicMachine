#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <SPI.h>
#include <SD.h>
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"

#define TX_PIN 18 // Arduino transmit  YELLOW WIRE  labeled RX on PRINTER
#define RX_PIN 17 // Arduino receive   GREEN WIRE   labeled TX on PRINTER

//Global variable declarations

int totalIn = 0;
int totalOut = 0;
int rNumber = 0;
int creatureNumber = 0;
int buttonState = 0;
int targetPrintMV = 0;

String fileNumber;
String fileString;
String recallPath;

File root;
//Constant global variable declarations
const uint16_t numFiles[] = {28, 930};
const int PWIDTH = 384;
const int PHEIGHT = 535;
const int BUTTON_ARRAY[] = {2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 22};

Adafruit_7segment MATRIX_1 = Adafruit_7segment();
Adafruit_7segment MATRIX_2 = Adafruit_7segment();

SoftwareSerial PRINTER_SERIAL(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal PRINTER(&PRINTER_SERIAL);






void setup() {
  // put your setup code here, to run once:
  for (uint16_t n = 0; n < sizeof(BUTTON_ARRAY); n++)
    pinMode(BUTTON_ARRAY[n], INPUT);
  Serial.begin(9600);
  PRINTER_SERIAL.begin(9600);
  PRINTER.begin();
  Serial.println("Serial Begin");
  MATRIX_1.begin(0x70);
  MATRIX_2.begin(0x71);
  MATRIX_1.println("1");
  MATRIX_2.println("2");
  MATRIX_1.writeDisplay();
  MATRIX_2.writeDisplay();

  Serial.println("MATRIX begin");
  Serial.println("SD Begin");
  if (!SD.begin(53))
  {
    Serial.println("SD Failed to initialize!");
    while(1);
  }
  randomSeed(analogRead(0));
}
0

void loop() {
  // put your main code here, to run repeatedly:
  buttonRead();
}


void buttonRead()
{
  for(uint16_t n = 0; n < 13; n++)
  {
    if(buttonState == HIGH)
      {
        switch (n)
        {
          case 10:
            clearInput();
            break;

          case 11:
            randomizeFromInput();
            break;

          case 12:
            recallPath = printRandomCardFromInput();
            break;
            
          case 13:
            sendPrintJob(recallPath);

          case 14:
            sendPrintJob(buildFileString(String(targetPrintMV), String(totalIn)));

          case 15:
            targetPrintMV = totalIn;
            clearInput();

          default:
            inputNumeric(n);        //MATRIX.println(totalIn);
        }
        while(buttonState == HIGH)
        {
        buttonState = digitalRead(BUTTON_ARRAY[n]);
        }
      delay(100);
      break;
      }
  }
}

void clearInput()
{
  totalIn = 0;
  totalOut = 0;
  printToMatrix(0, 1);
  printToMatrix(0, 2);
}

int randomizeFromInput()
{
  for (int x = 0; x < 50; x++)
  {
    int randomValue = random(numFiles[totalIn]);
    printToMatrix(randomValue, 2);
    delay(10);
    return randomValue;
  }
}

void printToMatrix(int value, int MatrixNumber)
{
  switch(MatrixNumber)
  {
    case 1:
      MATRIX_1.println(value);
    case 2:
      MATRIX_2.println(value);
  }
  MATRIX_1.writeDisplay();
  MATRIX_2.writeDisplay();
  delay(10);
}

String printRandomCardFromInput()
{
  int cardNumber = randomizeFromInput();
  String cardPath = buildFileString(String(totalIn), String(cardNumber));
  sendPrintJob(cardPath);
  return cardPath;
}

void inputNumeric(uint16_t inputValue)
{
  if (totalIn < 1000 & totalIn > -1) 
    totalIn = (totalIn * 10) + inputValue;
  printToMatrix(totalIn, 1);
}

String buildFileString(String path, String creatureID)
{
  path = ("/" + String(totalIn) );
  path = (path + "/");
  path = (path + creatureID);
  path += ".bin";
  return path;
}

void sendPrintJob(String path)
{
  root = SD.open(path, FILE_READ);
  PRINTER.printBitmap(PWIDTH, PHEIGHT, dynamic_cast<Stream*>(&root));
  PRINTER.println(path);
  PRINTER.feedRows(80);
  root.close();
}
