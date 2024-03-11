//Revised 3/8/2024

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <SPI.h>
#include <SD.h>
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include "ButtonMatrix.h"

using namespace RSys; //Button Matrix Dependencies

#define TX_PIN 18 // Arduino transmit  YELLOW WIRE  labeled RX on PRINTER
#define RX_PIN 17 // Arduino receive   WHITE WIRE   labeled TX on PRINTER

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

const uint16_t numFiles[] = {28, 930};
const int PWIDTH = 384;
const int PHEIGHT = 535;
const uint8_t BUTTON_COLUMNS = 4; /** Number of button matrix columns */
const uint8_t BUTTON_ROWS = 4; /** Number of button matrix rows */
uint8_t colPins[BUTTON_COLUMNS] = {5,4,3,2}; /** Button matrix column pins */
uint8_t rowPins[BUTTON_ROWS] = {8, 9, 10, 11}; /** Button matrix row pins */

RSys::Button buttons[BUTTON_ROWS][BUTTON_COLUMNS] = {
    { (1), (2), (3), (12)   },
    { (4), (5), (6), (13)   },
    { (7), (8), (9), (14)   },
    { (10), (0), (11), (15) }
};

ButtonMatrix BUTTON_MATRIX((Button*)buttons, rowPins, colPins, BUTTON_ROWS, BUTTON_COLUMNS);

const uint16_t numberOfButtons = BUTTON_MATRIX.getNumButtons();

/** Button matrix button definitons */

Adafruit_7segment MATRIX_1 = Adafruit_7segment();
Adafruit_7segment MATRIX_2 = Adafruit_7segment();

SoftwareSerial PRINTER_SERIAL(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal PRINTER(&PRINTER_SERIAL);

//Constant global variable declarations

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  PRINTER_SERIAL.begin(9600);
  PRINTER.begin();
  Serial.println("Serial Begin");
  MATRIX_1.begin(0x70);
  Serial.println("Matrix1 Begin");
  MATRIX_2.begin(0x71);
  Serial.println("Matrix2 Begin");
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

  BUTTON_MATRIX.init();
}


void loop() {
  // put your main code here, to run repeatedly:
  buttonRead();
}


void buttonRead()
{
  Button* pButton = NULL;
  if (BUTTON_MATRIX.update())
  {
    for(uint16_t n = 0; n < numberOfButtons; n++)
    {
      pButton = BUTTON_MATRIX.getButton(n);
      if(pButton->isPressed())
        {
          Serial.println("Button Pressed");
          uint16_t currentButton = pButton->getNumber();
          switch (currentButton)
          {
            //Clear all input
            case 10:
              clearInput();
              Serial.println("Case 10"); 
              break;

            //Calls randomizer without doing anything else. For debugging purposes.
            case 11:
              Serial.println("Case11");
              randomizeFromInput();
              break;

            //Call randomized print. Takes input as manavalue and returns 
            //a random creature ID from the designated mana value folder.
            case 12:
              Serial.println("Case 12");
              recallPath = printRandomCardFromInput();
              break;
              
            case 13:
              //Recalls last print job.
              Serial.println("Case 13");
              sendPrintJob(recallPath);
              break;

            case 14:
              //Prints specific card by ID and mana value. Must input mana value with Button 15 first.
              Serial.println("Case 14");
              sendPrintJob(buildFileString(String(targetPrintMV), String(totalIn)));
              break;

            case 15:
              //Takes the current input and stores it as a mana value to print a specific ID from that mana values folder.
              Serial.println("Case 15");
              targetPrintMV = totalIn;
              clearInput();
              break;

            default:
              //Input for numbers 0-9, numerical input.
              Serial.println("Case ");
              Serial.print(currentButton);
              inputNumeric(currentButton);
              break;       //MATRIX.println(totalIn);
          }
          waitUntilButtonRelease(pButton); //Calls function to halt program flow until the current button is released. 
          Serial.println("Total In = ");      //(cont.)Prevents an assortment of bugs and issues.
          Serial.print(totalIn);
          Serial.println("Selected Mana Value = ");
          Serial.println(targetPrintMV);
        }
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
  int randomValue;
  for (int x = 0; x < 50; x++)
  {
    randomValue = random(numFiles[totalIn]);
    printToMatrix(randomValue, 2);
    delay(10);
  }
  return randomValue;
}

void printToMatrix(int value, int MatrixNumber)
{
  switch(MatrixNumber)
  {
    case 1:
      MATRIX_1.println(value);
      break;
    case 2:
      MATRIX_2.println(value);
      break;
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
  path = ("/" + String(path) );
  path = (path + "/");
  path = (path + creatureID);
  path += ".bin";
  return path;
}

void sendPrintJob(String path)
{
    Serial.println(path);
    if (!SD.exists(path))
      return printToMatrix(8888, 1);
    root = SD.open(path, FILE_READ);
    PRINTER.printBitmap(PWIDTH, PHEIGHT, dynamic_cast<Stream*>(&root));
    PRINTER.println(path);
    PRINTER.feedRows(80);
    root.close();
}

void waitUntilButtonRelease(Button* rButton)
{
  while(rButton->isPressed())
    BUTTON_MATRIX.update();
}
