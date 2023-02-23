#include <Wire.h>
#include "Adafruit_VL6180X.h"

const uint8_t redPin = 5;
const uint8_t greenPin = 4;
const uint8_t bluePin = 3;
const uint8_t relePin = 2;
bool releStatus = false;
const uint8_t distance = 90;

Adafruit_VL6180X vl = Adafruit_VL6180X();

void setup() 
{
  Serial.begin(115200);

  // wait for serial port to open on native usb devices
  while (!Serial)
    delay(1);
  
  Serial.println("Adafruit VL6180x test!");
  if (!vl.begin()) 
  {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("Sensor found!");

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(relePin, OUTPUT);

  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, HIGH);
  digitalWrite(relePin, HIGH);
}

void loop() 
{
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  if (status == VL6180X_ERROR_NONE) 
  {
    if (range < distance)
    {
      Serial.print("Range: "); 
      Serial.println(range);

      delay(100);
      range = vl.readRange();
      if (range < distance)
      {
        switchRele();
        while (range < distance)
        {
          range = vl.readRange();
          delay(100);
        }
      }
    }
  }
  

  // Some error occurred, print it out!
  
//  if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
//    Serial.println("System error");
//  }
//  else if (status == VL6180X_ERROR_ECEFAIL) {
//    Serial.println("ECE failure");
//  }
//  else if (status == VL6180X_ERROR_NOCONVERGE) {
//    Serial.println("No convergence");
//  }
//  else if (status == VL6180X_ERROR_RANGEIGNORE) {
//    Serial.println("Ignoring range");
//  }
//  else if (status == VL6180X_ERROR_SNR) {
//    Serial.println("Signal/Noise error");
//  }
//  else if (status == VL6180X_ERROR_RAWUFLOW) {
//    Serial.println("Raw reading underflow");
//  }
//  else if (status == VL6180X_ERROR_RAWOFLOW) {
//    Serial.println("Raw reading overflow");
//  }
//  else if (status == VL6180X_ERROR_RANGEUFLOW) {
//    Serial.println("Range reading underflow");
//  }
//  else if (status == VL6180X_ERROR_RANGEOFLOW) {
//    Serial.println("Range reading overflow");
//  }
  delay(100);
}

void switchRele()
{
  if(releStatus)
  {
    digitalWrite(relePin, HIGH);
    digitalWrite(bluePin, HIGH);
    digitalWrite(greenPin, LOW);
    releStatus = false; 
  }
  else
  {
    digitalWrite(relePin, LOW);
    digitalWrite(bluePin, LOW);
    digitalWrite(greenPin, HIGH);
    releStatus = true;
  }
}
