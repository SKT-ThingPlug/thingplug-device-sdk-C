/**
   @file ArduinoMiddleware.ino

   @brief MangementAgent

   Copyright (C) 2016. SK Telecom, All Rights Reserved.
   Written 2016, by SK Telecom
*/

#include <SPI.h>
#include <Ethernet.h>

#include "src/MA/MA.h"
#include "src/SRA/GetTime.h"

void setup()
{
  byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x30, 0xDF}; // ex :
  Serial.begin(9600);
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }
  Serial.println("MA Start");
  Ethernet.begin(mac);
  setNtpTime();
  digitalClockDisplay();
}

void loop()
{
  MARun();
}

